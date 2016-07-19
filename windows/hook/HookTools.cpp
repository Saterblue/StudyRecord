#include "HookTools.h"
#include <string.h>
#include <stdlib.h>
#include <WinBase.h>
#include <TlHelp32.h>
#include <stdio.h>
#include "Psapi.h"
#pragma comment(lib,"Psapi.lib")


HookTool::HookTool(_in_ char* target_in):target_h(0),Map_h(0),kity(NULL),
ErrorCode(0),targetName(0),targetProcessID(0),hookDllName(0)
{
	targetName = tolowerstr(target_in);
	init();
}

int HookTool::init()
{
	HANDLE SnapShot_h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,NULL);
	if( NULL  == SnapShot_h )
		return MessageErr("CreateSnapShot ERROR!");

	PROCESSENTRY32  Entry32; 
	Entry32.dwSize = sizeof(Entry32);

	if( FALSE == Process32First(SnapShot_h,&Entry32) )
		return MessageErr("Enum Snap ERROR!");

	while(Process32Next(SnapShot_h,&Entry32))
	{
		char* str1 = tolowerstr(Entry32.szExeFile);
		if( 0 == strcmp(str1,targetName))
		{
			free(str1);
			HANDLE target = OpenProcess(PROCESS_ALL_ACCESS,NULL,Entry32.th32ProcessID);
			if( NULL == target )
				return MessageErr("Can't OpenProcess!");

			targetProcessID = Entry32.th32ProcessID;
			target_h = target;
			CloseHandle(SnapShot_h);
			return 0;
		}
		free(str1);
	}
	CloseHandle(SnapShot_h);
	ErrorCode = -1;
	return MessageErr("Can't Find The Process");
}

HookTool::~HookTool()
{
	if(targetName != NULL)
		free(targetName);

	if(hookDllName != NULL)
		free(hookDllName);
	
	if(kity != NULL)
		UnmapViewOfFile(kity);

	if(Map_h != NULL)
		CloseHandle(Map_h);

	if(target_h != NULL)
		CloseHandle(target_h);
}

/*
a example to open fileMapping:

#define HOOKID "00484340166978069456"
typedef struct hellokity
{
DWORD ProcessID;
DWORD instructionAddress;
char Message[256];
char targetName[256];
int flag;
}HELLOKITTY;

HANDLE handle = OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,(LPCSTR)HOOKID);
LPVOID kity = MapViewOfFile(handle,FILE_MAP_ALL_ACCESS,0,0,0);

UnmapViewOfFile(kity);
CloseHandle(handle);

You can also flush datas by you self using function:

FlushViewOfFile(
_In_ LPCVOID lpBaseAddress,
_In_ SIZE_T  dwNumberOfBytesToFlush
);
*/

int HookTool::CreateMapMemory()
{
	Map_h = CreateFileMapping(INVALID_HANDLE_VALUE, NULL , PAGE_EXECUTE_READWRITE | SEC_COMMIT , 
		0 ,sizeof(HELLOKITTY),(LPCSTR)HOOKID);	
	if(NULL == Map_h)
		return MessageErr("CreateFileMapping   Error!");

	kity = (HELLOKITTY*)MapViewOfFile(Map_h,FILE_MAP_ALL_ACCESS,0,0,0);
	if(NULL == kity)
		return MessageErr("MapVieOfFile Error!");
	memset(kity,0,sizeof(HELLOKITTY));
	
	kity->ProcessID = targetProcessID;
	memcpy(kity->targetName,targetName,sizeof(char)*strlen(targetName));
	kity->instructionAddress = ADDRESS;

	return 0;	
}


//The xxx.dll file should in the same path
int HookTool::LoadDllToTarget(char* dllName)
{
	char dllPath[MAX_PATH]={0};
	GetFullPathName(dllName,sizeof(dllPath),dllPath,NULL);

	LPVOID  MemorySpace = VirtualAllocEx(target_h,NULL,sizeof(dllPath),MEM_COMMIT,PAGE_EXECUTE_READWRITE);
	if( NULL == MemorySpace)
		return MessageErr("LoadDll CreateVirtualMemory Error!");

	if( 0 == WriteProcessMemory(target_h,MemorySpace,dllPath,sizeof(dllPath),NULL))
		return MessageErr("LoadDll WriteMemoryError");

	HMODULE kernel32 = GetModuleHandle("Kernel32");
	if( NULL == kernel32)
		return MessageErr("LoadDll GetKerner32 ModuleHandle Error");

	HANDLE crt_h = CreateRemoteThread(target_h,0,0,(LPTHREAD_START_ROUTINE)GetProcAddress(kernel32,"LoadLibraryA"),MemorySpace,0,NULL);
	if( NULL == crt_h)
		return MessageErr("LoadDll createRemoteThread Error");

	WaitForSingleObject(crt_h,INFINITE);
	VirtualFreeEx(target_h,MemorySpace,sizeof(dllPath),MEM_RELEASE);
	CloseHandle(kernel32);
	CloseHandle(crt_h);
	
	size_t size = strlen(dllName);
	hookDllName = (char*)malloc(sizeof(char)*size + 1);
	memcpy(hookDllName,dllName,sizeof(char)*size);
	hookDllName[size]=0;

	return 0;
}

char* HookTool::tolowerstr(char* str)
{
	char* temp = (char*)malloc(sizeof(char)*(strlen(str)+1));
	unsigned int i=0;
	for(i=0; *(str+i) != 0; i++)
		*(temp+i)=tolower(*(str+i));
	*(temp+i)=0;
	return temp;
}

DWORD HookTool::GetTargetThreadID()
{
	if( targetProcessID == 0 )
		return 0;
	HANDLE snap_h =  CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD,targetProcessID);
	THREADENTRY32 t32;
	t32.dwSize = sizeof(t32);

	for(Thread32First(snap_h,&t32);Thread32Next(snap_h,&t32);)
	{
		if(t32.th32OwnerProcessID == targetProcessID)
		{
			CloseHandle(snap_h);
			return t32.th32ThreadID;
		}
	}
	CloseHandle(snap_h);
	return 0;
}

int HookTool::UnLoadDll(char* hookDllName_str)
{
	if(hookDllName_str == 0)
		hookDllName_str = hookDllName;

	size_t size  = strlen(hookDllName_str);
	char* DLLNAME = (char*)malloc(sizeof(char*)*size + 1);
	memcpy(DLLNAME,hookDllName_str,sizeof(char)*size);
	DLLNAME[size] = 0;

	LPVOID dllAlloc = VirtualAllocEx(target_h,NULL,size + 1 ,MEM_COMMIT,PAGE_EXECUTE_READWRITE);
	if( NULL == dllAlloc)
		return MessageErr("UnLoadDll VirtualAlloc Error!");

	BOOL result = FALSE;
	result = WriteProcessMemory(target_h,dllAlloc,DLLNAME, size + 1 ,NULL);
	if(result == FALSE)
		return MessageErr("UnLoadDll Write Error!");
	free(DLLNAME);

	HMODULE kernel32 = GetModuleHandle("Kernel32");

	HANDLE th = CreateRemoteThread(target_h,NULL,0,
		(LPTHREAD_START_ROUTINE)GetProcAddress(kernel32,"GetModuleHandleA"),dllAlloc,0,0);
	if(th == NULL)
		return MessageErr("Unload GetModuleHandle Error!");

	WaitForSingleObject(th,INFINITE);

	HMODULE dll_M = 0;
	GetExitCodeThread(th,(LPDWORD)&dll_M);
	CloseHandle(th);
	VirtualFreeEx(target_h,dllAlloc,0,MEM_RELEASE);

	th  = CreateRemoteThread(target_h,NULL,0,
		(LPTHREAD_START_ROUTINE)GetProcAddress(kernel32,"FreeLibrary"),dll_M,0,0);
	WaitForSingleObject(th,INFINITE);
	if(th == NULL)
		return MessageErr("Unload freelibrary Error!");

	CloseHandle(kernel32);
	CloseHandle(th);
	return 0;
}

HMODULE HookTool::GetTargetModule(char* ModuleName)
{
	if(ModuleName == 0)
		ModuleName = targetName;

	HMODULE  hMod[1024]={0};    
	DWORD len = 0;
	EnumProcessModules(target_h,hMod,sizeof(hMod),&len);
	DWORD times = len/sizeof(DWORD);

	for(DWORD i = 0; i < times ; i++)
	{
		char name[1024]={0};
		GetModuleBaseName(target_h,hMod[i],name,sizeof(name));
		char* str1 =tolowerstr(name);
		char* str2 =tolowerstr(ModuleName);
		int result = strcmp(str1,str2);
		free(str1);
		free(str2);
		if(result == 0)
			return hMod[i];
	}
	return 0;
}


#ifdef DDBUG
int HookTool::MessageErr(char* message)
{
	ErrorCode = GetLastError();
	MessageBoxA(NULL,message,"Error",MB_OK); 
	return ErrorCode;
}
#else
int HookTool::MessageErr(char* message)
{return 0;}
#endif

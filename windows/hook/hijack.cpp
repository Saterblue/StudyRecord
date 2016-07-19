#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#define _in_
#define HOOKID "00484340166978069456"


void logFile(const char *str) {
	time_t t = time(NULL);
	tm *now = localtime(&t); 
	int len=0;
	char arr[1024]={0};
	len=sprintf(arr,"%d-%d-%d %d:%d:%d",now->tm_year+1900,now->tm_mon+1,now->tm_mday,now->tm_hour,now->tm_min,now->tm_sec);
	arr[len]=' ';
	arr[len+1]=0;
	for(int i=0;;++i){
		arr[len+1+i]=*(str+i*sizeof(char));
		if( *(str+i*sizeof(char)) == 0 ) 
			break;
	}
	FILE *file;
	fopen_s( &file,"HellokittyLog.txt", "a+");
	fprintf(file,"%s\n",arr);
	fclose(file);	
}


typedef struct hellokity
{
	DWORD ProcessID;
	DWORD instructionAddress;
	char Message[256];
	char targetName[256];
	int flag;
}HELLOKITTY;



DWORD Offset_Original = 0;
DWORD instructionAddress = 0;
DWORD Call_Original_Address = 0;
HANDLE map_h = NULL;
HELLOKITTY* kity=NULL;
HANDLE handle = 0;



char* GetLastMark(_in_ char* fullName,char mark)
{
	char* count = 0;
	char* flag = 0;
	for(count = fullName ; *count != 0; count++)
	{
		if(*count == mark)
			flag = count + 1;		
	}
	return flag;
}

void __stdcall spyCall(DWORD val)
{
	DWORD ECX_Hold;
	_asm
	{
		mov ECX_Hold,ecx;
	}

	DWORD value= val;
	char temp[256]={0};
	ReadProcessMemory(handle,(LPCVOID)value,temp,sizeof(temp)-1,NULL);
	int len = strlen(temp);



	char hah[125]={0};
	sprintf(hah,"len = %d",len);
	logFile(hah);


	if(len > 0)
	{
		char fuck[256]={0};
		memcpy(fuck,temp,sizeof(char)*len);
		logFile(fuck);
	}


	_asm
	{
		push val;
		mov ecx,ECX_Hold;
		call Call_Original_Address;
	}
}




BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpReserved )  // reserved
{
	// Perform actions based on the reason for calling.
	char DllName[255]={0};
	GetModuleFileNameA(hinstDLL,(LPCH)DllName,sizeof(DllName));
	char* dllname = GetLastMark(DllName,'\\');
	switch( fdwReason ) 
	{ 
	case DLL_PROCESS_ATTACH:
		{
			map_h = OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,(LPCSTR)HOOKID);
			if( NULL == map_h )
				logFile("OpenFileMapping Failed!");

			kity= (HELLOKITTY*)MapViewOfFile(map_h,FILE_MAP_ALL_ACCESS,0,0,0);
			if( NULL == kity)
				logFile("MapViewOfFile Failed!");

			handle = OpenProcess(PROCESS_ALL_ACCESS,NULL,kity->ProcessID);
			if(handle == NULL)
				logFile("getHandle error!");

			instructionAddress = kity->instructionAddress;
			//instructionAddress + Offset + sizeof(DWORD) + 1 = CallAdrress;
			BOOL result =ReadProcessMemory(handle,(LPVOID)(instructionAddress + 1),&Offset_Original,sizeof(DWORD),NULL);
			if(result == FALSE)
			{
				char errormessage[255]={0};
				sprintf(errormessage,"ReadError ISADDR:%X ErrorCode",instructionAddress,GetLastError());
				logFile(errormessage);
			}
			Call_Original_Address = instructionAddress + 1 + Offset_Original + sizeof(DWORD);

			DWORD NewCallAddress = (DWORD)spyCall;
			DWORD Offset_New = NewCallAddress - instructionAddress - sizeof(DWORD) - 1;
			result = WriteProcessMemory(handle,(LPVOID)(instructionAddress + 1),&Offset_New,sizeof(DWORD),NULL);
			if(result == FALSE)
				logFile("Write ProcessMemory Failed!");

			char message[512]={0};
			sprintf(message,"%s Load %s Success!",kity->targetName,dllname);
			MessageBoxA(NULL,message,"tips",MB_OK);
		}
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		// Do thread-specific cleanup.
		break;

	case DLL_PROCESS_DETACH:
		{
			char message[512]={0};
			sprintf(message,"%s Unload %s Success!",kity->targetName,dllname);
			MessageBoxA(NULL,message,"tips",MB_OK);
			WriteProcessMemory(handle,(LPVOID)(instructionAddress + 1),&Offset_Original,sizeof(DWORD),NULL);
			CloseHandle(handle);
			UnmapViewOfFile(kity);
			CloseHandle(map_h);	
		}
		break;
	}
	return TRUE;  
}

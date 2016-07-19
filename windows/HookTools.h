#include "windows.h"

#define _in_
#define _out_

#define HOOKID "00484340166978069456"

#ifndef MAX_PATH
#define MAX_PATH 255
#endif

#ifndef MAX_LENGTH
#define MAX_LENGTH 255
#endif

#define ADDRESS 0x40DB29
#define DDBUG

typedef struct hellokity
{
	DWORD ProcessID;
	DWORD instructionAddress;
	char Message[256];
	char targetName[256];
	int flag;
}HELLOKITTY;

//dumpbin -exports dllname.dll
class HookTool
{
public:
	HookTool(_in_ char* target);
	~HookTool();

	//Using CreateMapMemory() before you use the function! 
	HELLOKITTY* GetStruct(){return kity;}
	HANDLE GetTargetHandle(){return target_h;}
	DWORD GetTargetProcessID(){return targetProcessID;}
	int CreateMapMemory();
	DWORD GetTargetThreadID();
	HMODULE GetTargetModule(char* ModuleName = 0);
	int LoadDllToTarget(char* dllName);
	int UnLoadDll(char* hookDllName_str = 0);

	int GetLastError(){return ErrorCode;}	
	
private:
	char* hookDllName;
	char* targetName;
	HANDLE target_h;
	HANDLE Map_h;
	DWORD targetProcessID;
	HELLOKITTY* kity;
	int ErrorCode;

	int init();
	//When didn't need the point any more,You Should Free it!
	char* tolowerstr(char* str);
	int MessageErr(char* message);
};

// SockSupport.cpp: implementation of the CSockSupport class.
//
//////////////////////////////////////////////////////////////////////

#include "SockSupport.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSockSupport::CSockSupport(bool bAuto)
{
	m_bSupported = FALSE;

	if(bAuto)
		Init();
}

CSockSupport::~CSockSupport()
{
	if(m_bSupported)
		Uninit();
}

int CSockSupport::Init(WORD wVersion /* = DEFAULT_VERSION */)
{
	WSADATA wsaData;
	if (WSAStartup(wVersion, &wsaData) != 0)
	{
		return WSAGetLastError();
	}

	//Confirm that the WinSock DLL supports 2.0
	if(LOBYTE(wsaData.wVersion)!=LOBYTE(wVersion) ||
		HIBYTE(wsaData.wVersion)!=HIBYTE(wVersion) )
	{
		WSACleanup();
		return WSAGetLastError();
	}

	m_bSupported = TRUE;
	return 0;
}

int CSockSupport::Uninit()
{
	WSACleanup();
	return 0;
}

BOOL CSockSupport::IsSupported()
{
	return m_bSupported;
}


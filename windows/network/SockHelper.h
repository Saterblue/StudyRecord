/* -------------------------------- FILE HEADER --------------------------------
//
// Copyright (c) One2Three4 Corporation. All Rights Reservesd.
//
// File        : SockHelper.h
//
// Project     : Library.
//
// Description : Header file for the class CSockHelper.
//
// History     : 11-15-2002 Created. By RedTom.
//
// ---------------------------------------------------------------------------*/

#ifndef _CSOCKHELPER_H_
#define _CSOCKHELPER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SockSupport.h"

// Initialize the socket environment.
static CSockSupport g_sockSpt;

typedef void (CALLBACKONRECVDATA)(void* pTheOwner,
								  char* pData,
								  int   len);

const int DEFAULT_CAP_PORT = 56789;

typedef struct _tagNETDATA
{
	int   ndLen;
	char* ndBuf;
	_tagNETDATA()
	{
		ndLen = 0;
		ndBuf = NULL;
	}
}NETDATA, *PNETDATA;

class CSockHelper  
{
public:
	CSockHelper();
	virtual ~CSockHelper();

	// Helper for capturing net packets.
	// Initialize the callback function for capturing.
	void InitForCap(void* pTheOwner, CALLBACKONRECVDATA* pfOnRecvData);
	int  StartCapture(int capPort = DEFAULT_CAP_PORT, bool hasHeader = true);
	void StopCapture();
	void ParseIPPack(char* pData, int len,
		char* szProtocol, char* szSource, char* szDest, int* pHeaderLen);
	
	void SendPseudoTCPData();
	void SendPseudoUDPData();
	void Ping();

	static char* GetLocalIP(void);

protected:
	static DWORD WINAPI CaptureThread(LPVOID ctx);
	char* CheckProtocol(int iProtocol);

	HANDLE m_hCapThread;
	bool   m_bCaptureThreadRun;
	SOCKET m_sockCap;
	int    m_nCapPort;
	

private:
	void*               m_pTheOwner;
	CALLBACKONRECVDATA* m_pfOnRecvData;
};

#endif // !defined(_CSOCKHELPER_H_)

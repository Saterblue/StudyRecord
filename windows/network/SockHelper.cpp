// SockHelper.cpp: implementation of the CSockHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "SockHelper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSockHelper::CSockHelper()
{
	m_pTheOwner    = NULL;
	m_pfOnRecvData = NULL;
	m_hCapThread   = NULL;
	m_sockCap      = INVALID_SOCKET;
	m_bCaptureThreadRun = false;
}

CSockHelper::~CSockHelper()
{
	StopCapture();
	
}

void CSockHelper::InitForCap(void* pTheOwner, CALLBACKONRECVDATA* pfOnRecvData)
{
	m_pTheOwner    = pTheOwner;
	m_pfOnRecvData = pfOnRecvData;
}

DWORD WINAPI CSockHelper::CaptureThread(LPVOID ctx)
{
	CSockHelper* pSH = (CSockHelper*)ctx;
	char recvBuf[MAX_PACK_LEN];
	int read;
	while(pSH->m_bCaptureThreadRun)
	{
		if(pSH->m_bCaptureThreadRun==false)
			break;
		read = recv(pSH->m_sockCap, recvBuf , MAX_PACK_LEN, 0);
		if(read==SOCKET_ERROR)
		{
			if(WSAGetLastError()==WSAEWOULDBLOCK)
			{
				continue;
			}
			pSH->m_bCaptureThreadRun = false;
			break;
		}
		else
		{
			if(pSH->m_pTheOwner && pSH->m_pfOnRecvData)
				pSH->m_pfOnRecvData(pSH->m_pTheOwner,
				recvBuf,
				read);
		}
	}
	
	return 0;
}

int CSockHelper::StartCapture(int capPort /* = DEFAULT_CAP_PORT */,
							  bool hasHeader /* = true */)
{
	int err; 
	
	m_sockCap = socket(AF_INET , SOCK_RAW , IPPROTO_IP);
	if(m_sockCap==INVALID_SOCKET)
		return WSAGetLastError();
	
	char name[MAX_HOSTNAME_LAN];
	err = gethostname(name, MAX_HOSTNAME_LAN);
	if(err==SOCKET_ERROR)
		return WSAGetLastError();
	
	struct hostent FAR * pHostent;
	pHostent = gethostbyname(name);
	
	m_nCapPort = capPort;
	
	SOCKADDR_IN sa;
	sa.sin_family = AF_INET;
	sa.sin_port   = htons(m_nCapPort);
	memcpy(&sa.sin_addr.S_un.S_addr, pHostent->h_addr_list[0], pHostent->h_length);
	
	err = bind(m_sockCap, (PSOCKADDR)&sa, sizeof(sa));
	if(err==SOCKET_ERROR)
		return WSAGetLastError();
	
	BOOL bopt=TRUE;
	err = setsockopt(m_sockCap, SOL_SOCKET, SO_REUSEADDR, (char*)&bopt, sizeof(bopt));
	
	bopt = TRUE;
	err = setsockopt(m_sockCap, IPPROTO_IP, IP_HDRINCL, (char*)&bopt, sizeof(bopt));
	if(err==SOCKET_ERROR)
		return WSAGetLastError();
	
	// Set this option to receive all the IP packets.
	DWORD dwBufferLen[10];
	DWORD dwBufferInLen   = 1;
	DWORD dwBytesReturned = 0;
	err = WSAIoctl(m_sockCap,
		SIO_RCVALL,
		&dwBufferInLen,
		sizeof(dwBufferInLen),
		dwBufferLen,
		sizeof(dwBufferLen),
		&dwBytesReturned,
		NULL,
		NULL);
	if(err==SOCKET_ERROR)
		return WSAGetLastError();
	
	m_bCaptureThreadRun = true;
	m_hCapThread = CreateThread(NULL, 0, CaptureThread, this, 0, NULL);
	if(m_hCapThread==NULL)
	{
		m_bCaptureThreadRun = false;
		closesocket(m_sockCap);
		m_sockCap = INVALID_SOCKET;
		return -1;
	}
	
	return 0;
}

void CSockHelper::StopCapture()
{
	m_bCaptureThreadRun = false;
	if(m_hCapThread)
	{
		TerminateThread(m_hCapThread, 0);
		CloseHandle(m_hCapThread);
		m_hCapThread = NULL;
	}
	if(m_sockCap)
		closesocket(m_sockCap);
}

char* CSockHelper::GetLocalIP()
{
	char szLocname[MAX_HOSTNAME_LAN];
	LPHOSTENT lphp;
	struct in_addr in_addrIP;
	int iRc = gethostname(szLocname, MAX_HOSTNAME_LAN);
	if (iRc == SOCKET_ERROR)
	{
		return NULL;
	}
	lphp = gethostbyname(szLocname);
	in_addrIP = *(struct in_addr far *) (lphp->h_addr);
	
	return inet_ntoa(in_addrIP);
}

char* CSockHelper::CheckProtocol(int iProtocol)
{
	for(int i=0; i<MAX_PROTO_NUM; i++)
		if(ProtoMap[i].ProtoNum==iProtocol)
			return ProtoMap[i].ProtoText;
		return "";
}

void CSockHelper::ParseIPPack(char* pData, int len,
							  char* szProtocol,
							  char* szSource, char* szDest,
							  int* pHeaderLen)
{
	IP_HEADER *pIpheader;
	char* ptr=NULL;
	int iProtocol, iTTL, i;//, iFlag;
	unsigned char FlagMask;
	char TcpFlag[6]={'F','S','R','P','A','U'}; // The TCP flags.
/*
	char szProtocol[MAX_PROTO_TEXT_LEN];
	char szSource[MAX_ADDR_LEN]={0};
	char szDest[MAX_ADDR_LEN]={0};
*/
	char szSourceIP[MAX_ADDR_LEN]={0};
	char szDestIP[MAX_ADDR_LEN]={0};
	
	unsigned int seq;
	SOCKADDR_IN saSource, saDest;
	
	pIpheader = (IP_HEADER *)pData;
	//Check Proto
	iProtocol = pIpheader->proto;
	strncpy(szProtocol, CheckProtocol(iProtocol), MAX_PROTO_TEXT_LEN);
	
	//Check Source IP
	saSource.sin_addr.s_addr = pIpheader->sourceIP;
	strncpy(szSourceIP, inet_ntoa(saSource.sin_addr), MAX_ADDR_LEN);
	
	//Check Dest IP
	saDest.sin_addr.s_addr = pIpheader->destIP;
	strncpy(szDestIP, inet_ntoa(saDest.sin_addr), MAX_ADDR_LEN);
	iTTL = pIpheader->ttl;
    
	char szInfo[60]={0};
	char strMsg[100]={0};
	unsigned short srcPort,destPort;
    srcPort=0;destPort=0;
	
	//Calculate IP Header Length
	int iIphLen = sizeof(unsigned long) * (pIpheader->h_lenver & 0xf);
	*pHeaderLen = iIphLen;
	
	//Decode Sub Protocol:TCP, UDP, ICMP, etc
	switch(iProtocol)
	{
	case IPPROTO_TCP :
		TCP_HEADER * pTcpHeader;
		strcpy(szInfo,"flag:");
		pTcpHeader = (TCP_HEADER * )(pData+iIphLen);
		srcPort=ntohs(pTcpHeader->th_sport);
		destPort=ntohs(pTcpHeader->th_dport);
		FlagMask = 1;
		for(i=0; i<6; i++ )
		{
			if((pTcpHeader->th_flag) & FlagMask) 
				sprintf(szInfo,"%s%c",szInfo,TcpFlag[i]);
			else 
				sprintf(szInfo,"%s%c",szInfo,'-');
			FlagMask=FlagMask<<1;
			seq=ntohl(pTcpHeader->th_seq);
		}
		sprintf(szSource,"%s:%d",szSourceIP,srcPort);
		sprintf(szDest,"%s:%d",szDestIP,destPort);
		ptr=pData+iIphLen+(sizeof(unsigned long) * ((pTcpHeader->th_lenres & 0xf0)>>4|0));
		memcpy(strMsg,ptr,3);
		break;
	case IPPROTO_UDP :
		UDP_HEADER *pUdpHeader;
        pUdpHeader = (UDP_HEADER * )(pData+iIphLen);
        srcPort=ntohs(pUdpHeader->uh_sport);
        destPort=ntohs(pUdpHeader->uh_dport);
        sprintf(szInfo,"Len=%d\n", ntohs(pUdpHeader->uh_len));
		sprintf(szSource,"%s:%d",szSourceIP,srcPort);
		sprintf(szDest,"%s:%d",szDestIP,destPort);
		break;
	case IPPROTO_ICMP :
		ICMP_HEADER *icmp_header;
        icmp_header = (ICMP_HEADER * )(pData+iIphLen);
		sprintf(szSource,"%s",szSourceIP);
		sprintf(szDest,"%s",szDestIP);
        sprintf(szInfo,"type=%d,code=%d", icmp_header->i_type,icmp_header->i_code);
		break;
	default :
		sprintf(szSource,"%s",szSourceIP);
		sprintf(szDest,"%s",szDestIP);
		break;
	}
	
}

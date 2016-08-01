/* -------------------------------- FILE HEADER --------------------------------
//
// Copyright (c) One2Three4 Corporation. All Rights Reservesd.
//
// File        : SockSupport.h
//
// Project     : Library.
//
// Description : Header file for the class CSockSupport.
//               If your application is to use winsock, just declare a global
//               object of this class, and you could use the winsock functions
//               freely.
//
// History     : 11-09-2002 Add Init and Unint functions.
//                          Add the structures. By RedTom.
//               10-30-2002 Created. By RedTom.
//
// ---------------------------------------------------------------------------*/

#ifndef _CSOCKSUPPORT_H_
#define _CSOCKSUPPORT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mstcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#define MAX_PACK_LEN       65535 // The max IP packet to receive.
#define MAX_ADDR_LEN       16    // The dotted addres's length.
#define MAX_PROTO_TEXT_LEN 16    // The length of sub protocol name(like "TCP").
#define MAX_PROTO_NUM      12    // The count of sub protocols.
#define MAX_HOSTNAME_LAN   256   // The max length of the host name.

/*
// The IP packet is like this. Took from RFC791.
    0                   1                   2                   3   
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |Version|  IHL  |Type of Service|          Total Length         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |         Identification        |Flags|      Fragment Offset    |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |  Time to Live |    Protocol   |         Header Checksum       |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                       Source Address                          |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                    Destination Address                        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                    Options                    |    Padding    |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/

typedef struct _tagIPHEADER        // 20 Bytes
{
	unsigned char  h_lenver;
	unsigned char  tos;
	unsigned short total_len;
	unsigned short ident;
	unsigned short frag_and_flags;
	unsigned char  ttl;
	unsigned char  proto;
	unsigned short checksum;
	unsigned int   sourceIP;
	unsigned int   destIP;
}IP_HEADER, *PIP_HEADER;


/*
// The TCP packet is like this. Took from RFC793.
    0                   1                   2                   3   
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |          Source Port          |       Destination Port        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                        Sequence Number                        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                    Acknowledgment Number                      |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |  Data |           |U|A|P|R|S|F|                               |
   | Offset| Reserved  |R|C|S|S|Y|I|            Window             |
   |       |           |G|K|H|T|N|N|                               |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |           Checksum            |         Urgent Pointer        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                    Options                    |    Padding    |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                             data                              |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/

typedef struct _tagTCPHEADER   // 20 Bytes
{
	unsigned short th_sport;
	unsigned short th_dport;
	unsigned int   th_seq;
	unsigned int   th_ack;
	unsigned char  th_lenres;
	unsigned char  th_flag;
	unsigned short th_win;
	unsigned short th_sum;
	unsigned short th_urp;
}TCP_HEADER;

/*
// The TCP's pseudo header is like this. Took from RFC793.
                     +--------+--------+--------+--------+
                     |           Source Address          |
                     +--------+--------+--------+--------+
                     |         Destination Address       |
                     +--------+--------+--------+--------+
                     |  zero  |  PTCL  |    TCP Length   |
                     +--------+--------+--------+--------+
*/
typedef struct _tagPSD_HEADER // 16 Bytes
{
	unsigned long  saddr;
	unsigned long  daddr;
	char           mbz;
	char           ptcl;
	unsigned short tcpl;
}PSD_HEADER;

/*
// The UDP packet is lick this. Took from RFC768.
                  0      7 8     15 16    23 24    31  
                 +--------+--------+--------+--------+ 
                 |     Source      |   Destination   | 
                 |      Port       |      Port       | 
                 +--------+--------+--------+--------+ 
                 |                 |                 | 
                 |     Length      |    Checksum     | 
                 +--------+--------+--------+--------+ 
                 |                                     
                 |          data octets ...            
                 +---------------- ...                 
*/
typedef struct _tagUDPHEADER  // 8 Bytes
{
	unsigned short uh_sport;
	unsigned short uh_dport;
	unsigned short uh_len;
	unsigned short uh_sum;
} UDP_HEADER;


typedef struct _tagICMPHEADER
{
	unsigned char  i_type;
	unsigned char  i_code;
	unsigned short i_cksum;
	unsigned short i_id;
	unsigned short i_seq;
	unsigned long  timestamp;
}ICMP_HEADER;


// The protocol's map.
typedef struct _tagPROTOMAP
{
	int  ProtoNum;
	char ProtoText[MAX_PROTO_TEXT_LEN];
}PROTOMAP;

static PROTOMAP ProtoMap[MAX_PROTO_NUM]=
{
	{ IPPROTO_IP   , "IP "  },
	{ IPPROTO_ICMP , "ICMP" }, 
	{ IPPROTO_IGMP , "IGMP" },
	{ IPPROTO_GGP  , "GGP " }, 
	{ IPPROTO_TCP  , "TCP " }, 
	{ IPPROTO_PUP  , "PUP " }, 
	{ IPPROTO_UDP  , "UDP " }, 
	{ IPPROTO_IDP  , "IDP " }, 
	{ IPPROTO_ND   , "NP "  }, 
	{ IPPROTO_RAW  , "RAW " }, 
	{ IPPROTO_MAX  , "MAX " },
	{ NULL         , ""     }
};


const WORD DEFAULT_VERSION = MAKEWORD(2,0);

class CSockSupport
{
public:
	CSockSupport(bool bAuto = true);
	virtual ~CSockSupport();

	int Init(WORD wVersion = DEFAULT_VERSION);
	int Uninit();

	// Check whether the winsock is initialized(supported).
	BOOL IsSupported();

private:
	BOOL m_bSupported;
};

#endif // !defined(_CSOCKSUPPORT_H_)

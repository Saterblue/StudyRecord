 /* Internet Header Format(ip header)
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
*///RFC791 

typedef struct ip_head
{
	unsigned int version:			4; 	//version of ip
	unsigned int IHL:			4;	//ip header length,  ip_length = 4*sizeof(char) * IHL; 
	unsigned int service_type: 		8;	//can't sure what is use for; since about Precedence,more detail read the RFC
	unsigned int total_length:		16; 	//Total Length,include ip header; data_length = sizeof(char) * total_length
	unsigned int identification:		16; 	//An identifying value assigned by the sender
	unsigned int flags:			3;	//Various Control Flags. Fragment or not.see RFC;
	unsigned int fragment_offset:		13; 	//indicates where in the datagram this fragment belongs
	unsigned int time_to_live:		8;	
	unsigned int protocol:			8;	//see RFC 790
	unsigned int checksum:			16;
	unsigned int source_address:		32;
	unsigned int destination_address:	32;
	unsigned int options:			24;	//The options may appear or not in datagrams. 
	unsigned int padding:			8; 	//ensure the ip header ends on a 32 bit boundary... padding is zero
}IP_HEAD;


/*
  TCP Header Format

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
*///RFC793

typedef struct tcp_header
{
	unsigned int source_port: 			16;
	unsigned int destination_port:			16;
	unsigned int sequence_number:			32;
	unsigned int acknowledgment_number:		32;
	unsigned int data_offset:			4;	//This indicates where the data begins.see RFC 793
	unsigned int reserved:				6;
	unsigned int flags:				6;	//see RFC793
	unsigned int window:				16;
	unsigned int checksum:				16;
	unsigned int urgent_point:			16;
	unsigned int options:				24;
	unsigned int padding:				8;
}TCP_HEADER;


/* UDP header
                           
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
*///RFC 768

typedef struct udp_header
{
	unsigned int source_port:		16;
	unsigned int destination_port:		16;
	unsigned int length:			16;
	unsigned int checksum: 			16;
}UDP_HEADER;


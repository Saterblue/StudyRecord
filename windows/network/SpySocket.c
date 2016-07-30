/* -------------------------------- FILE HEADER --------------------------------
//
// Copyright (c) One2Three4 Corporation. All Rights Reservesd.
//
// Description : ReWrite the source files parameter's name  07-30-2016 By Unknow
//               
// History     : 11-09-2002 Add Init and Unint functions.
//                          Add the structures. By RedTom.
//               10-30-2002 Created. By RedTom.
//
// ---------------------------------------------------------------------------*/



const int DEFAULT_PORT =56789;

typedef void RECVDATA_CALLBACK(void* classEntity,char* data,int length);

typedef struct _tagnetdata
{
	int 	NetData_length;
	char* 	NetData_buffer;
}NETDATA;





class SocketListenTool
{
	void Capture_Init(void* classEntity,RECVDATA_CALLBACK* classEntityFunction);
	int StartCapture(int capture_port = DEFAULT_PORT,bool hasHeader = true);
	void StopCapture();
	void ParseIPPack(char* data,int length,char* Protocol_Type,char* Source_ip,char* dest_ip,int* headerLength);

	static char* GetLocalIP(void);
	protected:
		static DWORD WINAPI Capture_Thread(LPVOID Object_self);
		char* CheckProtocolType(int Protocol_int);
		HANDLE  	CapThread_handle;
		bool		isCapThreadRun;
		SOCKET 		Spy_Socket; 	
		int 		Port_to_Listen;
	private:
		void* 				theClassEntity;
		RECVDATA_CALLBACK*	theClassEntity_function;
};

void Capture_Init(void* classEntity,RECVDATA_CALLBACK* classEntityFunction)
{
	theClassEntity = classEntity;
	theClassEntity_function = classEntityFunction;
}

int StartCapture(int capture_port = DEFAULT_PORT,bool hasHeader = true)
{
	int function_return_value;
	Spy_Socket = socket(AF_INET,SOCK_RAW,IPPROTO_IP);
	if( Spy_Socket == INVALID_SOCKET )
		return WSAGetLastError();

	char host_name[MAX_HOSTNAME_LAN];
	function_return_value = gethostname(name,MAX_HOSTNAME_LAN);
	if( function_return_value == SOCKET_ERROR )
		return WSAGetLastError();

	struct hostent FAR* host_info;
	host_info = gethostbyname(host_name);

	Port_to_Listen = capture_port; // default = 56789
	SOCKADDR_IN sock_address_info;
	sock_address_info.sin_family = AF_INET;
	sock_address_info.sin_port	 = htons(Port_to_Listen); 
	memcpy(&sock_address_info.sin_addr.S_un.s_addr, host_info->h_addr_list[0], host_info->h_length);

	function_return_value = bind(Spy_Socket , (PSOCKADDR)&sock_address_info , sizeof(sock_address_info));
	if( function_return_value == SOCKET_ERROR )
		return WSAGetLastError();

	BOOL Parameter = TRUE;
	function_return_value = setsockopt(Spy_Socket , SOL_SOCKET, SO_REUSEADDR , (char*)&Parameter, sizeof(Parameter));
	if(function_return_value  == SOCKET_ERROR)
		return WSAGetLastError();

	Parameter = TRUE;
	function_return_value = setsockopt(Spy_Socket , IPPROTO_IP , IP_HDRINCL, (char*)&Parameter, sizeof(Parameter));
	if(function_return_value == SOCKET_ERROR)
		return WSAGetLastError();


	DWORD Buffer_out[10] = {0};
	DWORD Buffer_in = 1 ;
	DWORD BytesReturned 	 = 0 ;


	function_return_value = WSAIoctl(Spy_Socket,SIO_RCVALL,&Buffer_in,sizeof(Buffer_in),
							Buffer_out,sizeof(Buffer_out),&BytesReturned,NULL,NULL);
	if(function_return_value == SOCKET_ERROR)
		return WSAGetLastError();

	isCapThreadRun = true;
	CapThread_handle = CreateThread(NULL,0,Capture_Thread, this , 0 , NULL);
	if( CapThread_handle == NULL)
	{
		isCapThreadRun = false;
		closesocket(Spy_Socket);
		Spy_Socket = INVALID_SOCKET;
		return -1;
	}

	return 0;
}


DWORD WINAPI Capture_Thread(LPVOID Object_self)
{
	SocketListenTool* this_point = (SocketListenTool*)Object_self;
	char recv_buffer[MAX_PACK_LEN] = { 0 };
	int recv_length;

	while( this_point -> isCapThreadRun )
	{
		recv_length = recv( this_point->Spy_Socket , recv_buffer , MAX_PACK_LEN , 0 )
		if(recv_length == SOCKET_ERROR)
		{
			if( WSAGetLastError() == WSAEWOULDBLOCK )
				continue;
			this_point->isCapThreadRun = false;
		}
		else
		{
			if( this_point->theClassEntity && this_point->theClassEntity_function )
				this_point->theClassEntity_function(this_point->theClassEntity , recv_buffer ,recv_length);
		}
	}

	return 0;
}


char* GetLocalIP()
{
	char host_name[MAX_HOSTNAME_LAN] = {0};
	LPHOSTENT host_info_point;
	struct in_addr address_info;
	int result = gethostname(host_name , MAX_HOSTNAME_LAN);
	if( result == SOCKET_ERROR)
		return NULL;

	host_info_point = gethostbyname(host_name,MAX_HOSTNAME_LAN);
	if( host_info_point == NULL)
		return NULL;

	address_info = *(struct in_addr far*)(host_info_point->h_addr)
	return inet_ntoa(address_info);
}


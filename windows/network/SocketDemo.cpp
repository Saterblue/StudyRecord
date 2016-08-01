#include "SocketDemo.h"

bool isCmp(char* ip_1,char* ip_2);
int GetPort(char* ip_and_port,char* ip);
bool checkPort(char* ip_info,int port);

CSockHelper* SocketDemo::socket_help = new CSockHelper();

SocketDemo::SocketDemo(){}
SocketDemo::~SocketDemo(){ delete socket_help;}

void SocketDemo::init()
{
	socket_help->InitForCap(this,DataFunction);
	socket_help->StartCapture();
}

void SocketDemo::DataFunction(void* ClassEntity,char* data,int length)
{
	char protocol[256]={0};
	char Source[256] = {0};
	char Dest[256] = {0};
	int header_length;
	

	socket_help->ParseIPPack(data,length,protocol,Source,Dest,&header_length);
	
	
	if( checkPort(Dest,3305))
	{
		 if(length > 300 || length < 120)
		 {
			 printf("超长数据包:%d\n",length);
			  return;
		 }
		 unsigned int* val = (unsigned int*)&data[104];
		 if(*val != 0xffffffff)
			 return ;
			
		 printf("协议: %s\n源地址: %s     目标地址: %s\n数据包头长度: %d      数据包数据长度: %d\n",protocol,Source,Dest,header_length,length);
		 unsigned char* u_data = (unsigned char*)malloc(sizeof(char)*length);
		 memcpy(u_data,data,length);
		 for(int i= header_length; i< length ; i++)
			 printf("%02X ",u_data[i]);
		 printf("\n");
		 printf("-------------------------------------------------------------------------------\n");
		 free(u_data);
	}

	
}


bool isCmp(char* ip1_z,char* ip2_z)
{
	for(int i =0; ip1_z[i]!=0  && ip2_z[i]!=0 ;i++)
		if(ip1_z[i] != ip2_z[i])
			return false;
	return true;
}

bool checkPort(char* ip_info,int port)
{
	int length= strlen(ip_info);
	int index = 0;
	if(ip_info == "" && ip_info == NULL)
		return false;
	for( ; index< length && ip_info[index++] != ':' ;)
	{
		
	}
	int ip_port =0;
	sscanf(&ip_info[index],"%d",&ip_port);

	if( ip_port == port)
		return true;
	return false;  
}



int GetPort(char* ip_and_port,char* ip)
{
	int port = 0;
	char format[1024]={0};
	sprintf(format,"%s:%s",ip,"%d");
	//Example format: "192.168.0.1:%d"
	sscanf(ip_and_port,format,&port);
	return port;
}
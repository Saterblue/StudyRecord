

#ifndef		JHGLKKJGHGQ_JKLLQ 
#define		JHGLKKJGHGQ_JKLLQ
#include	"SockHelper.h"

class SocketDemo
{
public:
	SocketDemo();
	~SocketDemo();

	static CSockHelper* socket_help;

	void init();
	static void DataFunction(void* ClassEntity,char* data,int length);
};

#endif
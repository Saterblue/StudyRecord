#include <stdio.h>
#include "SocketDemo.h"

int main()
{
	SocketDemo* sc = new SocketDemo;
	sc->init();
	
	while(1);
	delete(sc);
	return 0;
}
#include <stdio.h>

#define true 1
#define false 0
typedef char bool;

/*
@ Give two point and cache size;
*/
bool isCacheMatch(const void* addr1,const void* addr2,size_t len) {
	while( len-- ) {
		if( *(char*)addr1++ != *(char*)addr2++)
			return false;
	}
	return true;
} 


// Example below

struct test{ int var1; char var2; double var3;};

int main(void) {
  	struct test t1={1,'a',3.4f};	
  	struct test t2={1,'a',3.4f};
  
  	isCacheMatch(&t1,&t2,sizeof(t1)) ? printf("Match!\n") : printf("Not Match!\n") ;
  	
  	return 0;
}

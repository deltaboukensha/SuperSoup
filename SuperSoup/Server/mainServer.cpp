#pragma comment(lib, "Ws2_32.lib" ) //windows sockets v2 32 bit tcp-network library

#include <crtdbg.h>

#include "GameServer.hpp"

int main(int argc, char** argv)
{
	//_CrtSetBreakAlloc(150);
    //_RPT0(_CRT_WARN, "hello world\n");
	printf("main: start\n");

	try
	{
		GameServer gs;
		gs.run();
	}
	catch(char* ex)
	{
		printf("%s\n", ex);
		getchar();
	}

	printf("main: end\n");

    //printing memory leaks takes a lot of time
    //also deallocationg memory in a proper manner also takes alot of time
	if( _CrtDumpMemoryLeaks() ){
        printf("main: you have memory leaks!\n");
        getchar();
    }

	return 1337;
}
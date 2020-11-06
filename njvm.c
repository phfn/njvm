#include <stdio.h>
#include <string.h>
#include <time.h>

int main(int argc, char* argv[])
{

    if (argc > 1)
    {
    	if (strcmp(argv[1], "--help") == 0)
    		printf("usage: ../njvm [option] [option] ...\n  --version        show version and exit\n  --help           show this help and exit\n");
    		
    	else if (strcmp(argv[1], "--version") == 0)
    		printf("Ninja Virtual Machine version 0 (compiled Sep 23 2015, 10:36:52)\n");
    		
    	else
    		printf("unknown command line argument '%s', try './njvm --help'\n", argv[1]);
    }
		else
		{
    	printf("Ninja Virtual Machine started\n");
    	printf("Ninja Virtual Machine stopped\n");
		}
    return 0;
}

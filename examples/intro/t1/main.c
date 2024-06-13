#include <stdio.h>
#include <string.h>

#include "shell.h"

int main(void){
	puts("HIIIIII");
	
	char buf[SHELL_DEFAULT_BUFSIZE];
	shell_run(NULL, buf, SHELL_DEFAULT_BUFSIZE);
	return 0;
}


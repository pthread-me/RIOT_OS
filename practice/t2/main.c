#include <stdio.h>
#include <string.h>
#include "shell.h"

int cmd_handler(int argc, char** argv){
	for(int i=1; i<= argc; i++){
		if(argv[i] == NULL || strlen(argv[i]) > 2048){
			break;
		}
	
		printf("%s\n", argv[i]);
	}
	return 0;
}

int main(void){
	puts("py terminal begining");
	
	shell_command_t commands[] = {
		{"echo", "prints whatever comes after it", cmd_handler}
	};

	char buf[SHELL_DEFAULT_BUFSIZE];
	shell_run(commands, buf, SHELL_DEFAULT_BUFSIZE);

	return 0;
}

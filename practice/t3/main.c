#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "shell.h"
#include "thread.h"


char thread_stack[THREAD_STACKSIZE_MAIN];

void* thread_handler(void* arg){

	puts("In thread");
	return arg;
}

int main(void){
	kernel_pid_t id = thread_create(thread_stack, sizeof(thread_stack),
			THREAD_PRIORITY_MAIN - 2, THREAD_CREATE_STACKTEST, thread_handler,
			NULL, "my thread");


	char* p;
	if(0 > asprintf(&p, "\nThe pid is: %d", id)){
		exit(1);
	}

	puts(p);
	free(p);


	char buf[SHELL_DEFAULT_BUFSIZE];
	shell_run(NULL, buf, SHELL_DEFAULT_BUFSIZE);



	return 0;
}

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <uchar.h>

#include "shell.h"
#include "thread.h"
#include "xtimer.h"

char stack[THREAD_STACKSIZE_MAIN];


void* handler(void* k){
	//not using the fnction input
	(void) k;


	uint32_t time = 0;
	uint32_t* current_time = calloc(1, sizeof(uint32_t));

	while(true){
		time =  xtimer_now_usec();
		char* time_string;

		if(0 > asprintf(&time_string, "%d", time)){
			exit(64);
		}


		puts(time_string);
		free(time_string);
		*current_time = xtimer_now_usec(),	
		xtimer_periodic_wakeup(current_time, 2e+6);
	}
}

int main(void){
	thread_create(stack, 
		sizeof(char) * THREAD_STACKSIZE_MAIN, THREAD_PRIORITY_MAIN -1, 
		THREAD_CREATE_STACKTEST, handler/*handler*/, NULL/* input*/, "thread 1");			
		char buffer[SHELL_DEFAULT_BUFSIZE];
		shell_run(NULL, buffer, SHELL_DEFAULT_BUFSIZE);
}

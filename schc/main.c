#include <stdio.h>
#include "shell.h"
#include "msg.h"

#define THREAD_QUEUE_SIZE (8)

static msg_t main_buffer[THREAD_QUEUE_SIZE];

extern int udp_cmds(int, char**);


static const shell_command_t shell_commands[] = {
	{"udp", "looks up the next argument to either start a udp server or send a udp packet", udp_cmds},
	{NULL, NULL, NULL}
};

int main(void){

	msg_init_queue(main_buffer, THREAD_QUEUE_SIZE);
	
	char shell_buffer[SHELL_DEFAULT_BUFSIZE];
	shell_run(shell_commands, shell_buffer, SHELL_DEFAULT_BUFSIZE);

}

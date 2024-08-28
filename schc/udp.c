#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "pkt_factory.h"

#include "schc.h"
#include "compressor.h"
#include "bit_operations.h"

#define MAX_PACKET_SIZE 128
static char sender_thread_stack[THREAD_STACKSIZE_DEFAULT];

//Function is used to print the raw packets accessed via gnrc_pktsnip_t->data
//in a bits format
void print_bits(int input){
	uint8_t mask = 128;
	int output;

	for(int i=0; i<8; i++){
		output = mask & input;
		output = output >> (8 - (i+1));
		printf("%d", output);
		mask = mask >> 1;
	}
}


//Sends a udp packets over ipv6 to the inputted address and port
void compress(gnrc_pktsnip_t* ip){

/*-----------------------------------------------------------------------------
	#ADDING THE SCHC COMPRESSTION BELLOW
-----------------------------------------------------------------------------*/

	schc_compressor_init();

	struct schc_compression_rule_t* comp_rule;
	uint8_t* out_buf = calloc(ip->size , sizeof(uint8_t));
	schc_bitarray_t bit_array = SCHC_DEFAULT_BIT_ARRAY(ip->size , out_buf);

	// the actual compression call
	comp_rule = schc_compress(ip->data, ip->size, &bit_array, 0x01, DOWN);
	if(comp_rule){
        printf("\ncompresion rule used: %d\n", comp_rule->rule_id);
    }

    (void) comp_rule;
}


void* run(void* argv){
//-----------------------------------------------------------------------------
//  HANDLING USER INPUT AND CHECKING ITS FORMATS
//-----------------------------------------------------------------------------
	ipv6_addr_t addr;

	//reading in the thread arguments
	char** thread_arg = (char**)argv;

	//char* dest_mac = thread_arg[1];
	char* dest_addr = thread_arg[1];

    if(ipv6_addr_from_str(&addr, dest_addr) == NULL){
		printf("Invalid ipv6 address\n");
		return NULL;
	}


    gnrc_pktsnip_t* packet;
    printf("\n--------------------------------------------------------------\n");
    printf("Client POST request:\n");
    printf("\n--------------------------------------------------------------\n");
    packet = build_pkt(addr, 4);
    compress(packet);

    printf("\n--------------------------------------------------------------\n");
    printf("Server piggybacked response:\n");
    printf("\n--------------------------------------------------------------\n");
    packet = build_pkt(addr, 5);
    compress(packet);

    return NULL;
}

/**
  * CLI command handling
**/
int udp_cmds(int argc, char** argv){

	if(argc == 2){
		kernel_pid_t sender_thread = thread_create(sender_thread_stack,
				sizeof(sender_thread_stack), THREAD_PRIORITY_MAIN-1,
				THREAD_CREATE_STACKTEST, run, argv, "sending a udp packet");

	}
  /*  else if(argc == 3 && strcmp(argv[1], "server") ==0){
		kernel_pid_t server_thread = thread_create(server_thread_stack,
				sizeof(server_thread_stack), THREAD_PRIORITY_MAIN-2,
				THREAD_CREATE_STACKTEST, udp_server, argv, "create udp server");
	}*/
    else{
		printf("invalid command\n");
		return -1;
	}


	return 0;
}

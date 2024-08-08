#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#include "net/ipv6/addr.h"
#include "net/gnrc.h"
#include "net/gnrc/ipv6.h"
#include "net/ipv6.h"
#include "net/gnrc/udp.h"
#include "net/nanocoap.h"
#include "net/coap.h"

#include "schc.h"
#include "compressor.h"
#include "bit_operations.h"

#define MAX_PACKET_SIZE 128


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
void* udp_send(void* argv){

//-----------------------------------------------------------------------------
//  HANDLING USER INPUT AND CHECKING ITS FORMATS
//-----------------------------------------------------------------------------
	uint16_t port;
	ipv6_addr_t addr;

	//reading in the thread arguments
	char** thread_arg = (char**)argv;

	//char* dest_mac = thread_arg[1];
	char* dest_addr = thread_arg[1];
	char* dest_port = thread_arg[2];

    if(ipv6_addr_from_str(&addr, dest_addr) == NULL){
		printf("Invalid ipv6 address\n");
		return NULL;
	}

	//checks port format and range
	for(size_t i=0; i<strlen(dest_port); i++){
		if(!isdigit(dest_port[i])){
			printf("Invalid port format, must be an int\n");
			return NULL;
		}
	}
	port = (int16_t) strtol(dest_port, NULL, 10);


//-----------------------------------------------------------------------------
//  CREATING AND CUSTOMIZING THE PACKET
//-----------------------------------------------------------------------------

	gnrc_pktsnip_t *payload, *coap, *udp, *ip;


    uint64_t token = 70;
	coap_hdr_t* coap_hdr = calloc(1, sizeof(coap_hdr_t));
	int coap_size = coap_build_hdr(coap_hdr, COAP_TYPE_NON, NULL, 0, COAP_CODE_EMPTY, 2);

	coap = gnrc_pktbuf_add(NULL, (void*)coap_hdr, coap_size, GNRC_NETTYPE_UNDEF);

	//adding udp header
	if((udp = gnrc_udp_hdr_build(coap, 6666, port)) == NULL){
		printf("cant add udp hdr\n");
		return NULL;
	}

	//adding ip header
	if((ip = gnrc_ipv6_hdr_build(udp,  &addr , &addr)) == NULL){
		printf("cant add ipv6 hdr\n");
		return NULL;
	}

	if(ip){

        ipv6_hdr_t*  data = (ipv6_hdr_t*) ip->data;
        data->nh = PROTNUM_UDP;
	}

	if(udp){

		udp_hdr_t* data = (udp_hdr_t*)udp->data;

		//dst port num is stored as a big-endian 16 bit int, in the
		//network_uint16_t or be_uint16_t struct (same thing just typedef ed)
		network_uint16_t be_dst= data->dst_port;
		uint16_t dst = (byteorder_btols(be_dst)).u16;
		printf("%d\n", dst);
		//converting the big endian dst to little endian then extracting the
		//actual port num from the struct
		//uint16_t dst = (byteorder_btols(be_dst)).u16;

	}

	if(coap){
		coap_hdr_t* data = (coap_hdr_t*)coap->data;
	    coap_hdr_set_code(data, COAP_METHOD_GET);
		coap_hdr_set_type(data, COAP_TYPE_NON);

        // hardcoding the token length to be 8 bytes
        uint8_t coap_hdr_tkl = data->ver_t_tkl;
        uint8_t mask = 8;
        coap_hdr_tkl = coap_hdr_tkl | mask;
    }


	//merging ip, udp and coap headers so its a single gnrc_pktsnip_t since
    //libscshc doesn' work on seperate headers
	if(gnrc_pktbuf_merge(ip) != 0){
		printf("couldn't merge ip and udp packets\n");
	}
/*-----------------------------------------------------------------------------
	#ADDING THE SCHC COMPRESSTION BELLOW
-----------------------------------------------------------------------------*/

	int ip_hdr_size = ip->size;
	int udp_hdr_size = udp->size;
	printf("\nIPv6 hdr size: %d bytes\nUDP hdr size: %d bytes\nCOAP hdr size: %d bytes\n",
			ip_hdr_size, udp_hdr_size, coap_size);

	schc_compressor_init();

	struct schc_compression_rule_t* comp_rule;
	uint8_t* out_buf = calloc(ip->size , sizeof(uint8_t));
	schc_bitarray_t bit_array = SCHC_DEFAULT_BIT_ARRAY(ip->size , out_buf);

	// the actual compression call
	comp_rule = schc_compress(ip->data, ip->size, &bit_array, 0x01, DOWN);
	(void) comp_rule;

/*-----------------------------------------------------------------------------
	#END OF SCHC
-----------------------------------------------------------------------------*/
	return NULL;
}




/**
  * CLI command handling
**/
int udp_cmds(int argc, char** argv){

	if(argc == 3){
		kernel_pid_t sender_thread = thread_create(sender_thread_stack,
				sizeof(sender_thread_stack), THREAD_PRIORITY_MAIN-1,
				THREAD_CREATE_STACKTEST, udp_send, argv, "sending a udp packet");

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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#include "net/ipv6/addr.h"
#include "net/gnrc.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/udp.h"
#include "net/gnrc/pktdump.h"
#include "utlist.h"

#include "schc.h"
#include "compressor.h"
#include "bit_operations.h"

#define MAX_PACKET_SIZE 128
#define SERVER_MSG_QUEUE_SIZE 8
#define SERVER_BUFFER_SIZE 128


static char sender_thread_stack[THREAD_STACKSIZE_DEFAULT];

static bool server_status = false; //true == on, false == off
static char server_thread_stack[THREAD_STACKSIZE_DEFAULT];
static char server_buffer[SERVER_BUFFER_SIZE];

static msg_t server_msg_queue[SERVER_MSG_QUEUE_SIZE];





/**
  * Function is used to print the raw packets accessed via gnrc_pktsnip_t->data
  * in a bits format
**/
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

void* udp_server(void* argv){

	msg_init_queue(server_msg_queue, SERVER_MSG_QUEUE_SIZE);

	//creating and initializing a static net registry for the calling thread  
	gnrc_netreg_entry_t server = GNRC_NETREG_ENTRY_INIT_PID(GNRC_NETREG_DEMUX_CTX_ALL, thread_getpid());
	gnrc_netreg_register(GNRC_NETTYPE_IPV6, &server);

	while(true){
		msg_t message;
		printf("waiting for message\n");
		msg_receive(&message);

		printf("got a message\n");
	}
}

void* udp_send(void* argv){
	
	//reading in the thread arguments
	char** thread_arg = (char**)argv;
	//char* dest_mac = thread_arg[1];
	char* dest_addr = thread_arg[1];
	char* dest_port = thread_arg[2];
	char* data = thread_arg[3];

	uint16_t port;
	ipv6_addr_t addr;

	//checks the ip address format
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

	gnrc_pktsnip_t *payload, *udp, *ip;
	
	payload = gnrc_pktbuf_add(NULL, (void*)data, strlen(data), GNRC_NETTYPE_UNDEF);
	

	//adding udp header
	if((udp = gnrc_udp_hdr_build(payload, 8000, port)) == NULL){
		printf("memory allocation error\n");
		return NULL;
	}

	//adding ip header 
	if((ip = gnrc_ipv6_hdr_build(udp,  NULL , &addr)) == NULL){
		printf("memory allocation error\n");
		return NULL;
	}

	//merging ip and udp headers so its a single gnrc_pktsnip_t since the 
	//libschc compression requires that udp is paired with ip 
	if(gnrc_pktbuf_merge(ip) != 0){
		printf("couldn't merge ip and udp packets\n");
	}


/*-----------------------------------------------------------------------------
	#ADDING THE SCHC COMPRESSTION BELLOW
-----------------------------------------------------------------------------*/
	
	
	int ip_hdr_size = ip->size;
	int udp_hdr_size = udp->size;


	printf("\nIPv6 hdr size:%d\nUDP hdr size:%d\n", ip_hdr_size, udp_hdr_size);
	
	//printing the original header in bit format
	for(int i=0; i<ip_hdr_size; i++){
		print_bits(	((int*)ip->data)[i]	);
	}
	printf("\n");

	
	schc_compressor_init();

	struct schc_compression_rule_t* comp_rule;
	uint8_t* out_buf = calloc(ip->size * 8, sizeof(uint8_t));
	
	schc_bitarray_t bit_array = SCHC_DEFAULT_BIT_ARRAY(ip->size, out_buf);

	printf("compressing\n");
	comp_rule = schc_compress(ip->data, ip->size, &bit_array, 0x01, DOWN);
	

	//simple check to ensure that the rules is correctly implemented
	if(comp_rule->udp_rule){
		printf("udp rule exists\n");
		uint8_t udp_f_len = comp_rule->udp_rule->content[1].target_value[0];
		printf("udp source port length should be 16 and is %d\n", udp_f_len);
	}

		
/*-----------------------------------------------------------------------------
	#END OF SCHC
-----------------------------------------------------------------------------*/


//	BELLOW CODE IS FOR DISPATCHING PACKETS WHICH I AM STILL FIGURING OUT HOW 
//	TO DO WHEN THE PKT IS COMPRESSED, an issue is that we doent have ethertypes
//	for schc

/*
	puts("getting mac addr");
	uint8_t* l2addr = calloc(GNRC_NETIF_L2ADDR_MAXLEN, sizeof(uint8_t));

	printf("The mac addr length is %d\n", GNRC_NETIF_L2ADDR_MAXLEN);


	int addr_len = (GNRC_NETIF_L2ADDR_MAXLEN*3) -1;
	
	int index = 0;
	for(int i=0; i<addr_len; i+=3){
		l2addr[index] = dest_mac[i]*16 + dest_mac[i+1];
		index++;
	}


	//creating the netif header
	gnrc_netif_t* netif = gnrc_netif_iter(NULL);
	gnrc_pktsnip_t* netif_hdr = gnrc_netif_hdr_build(NULL, 0, NULL, 0);
	((gnrc_netif_hdr_t*)netif_hdr->data)->if_pid = netif->pid;


	//prepending ip packet to the net interface and sending it to layer 2
	gnrc_pkt_prepend(ip, netif_hdr);
	
	//printf("\n\nNetif header:\n");
//	gnrc_netif_hdr_print((gnrc_netif_hdr_t*)netif_hdr->data);


	
	if(gnrc_netapi_dispatch_send(GNRC_NETTYPE_IPV6, GNRC_NETREG_DEMUX_CTX_ALL, netif_hdr) <0){
		printf("couldnt dispatch packet\n");
		return NULL;
	}
*/

	return NULL;
}
int udp_cmds(int argc, char** argv){

	if(argc == 4){
		kernel_pid_t sender_thread = thread_create(sender_thread_stack, 
				sizeof(sender_thread_stack), THREAD_PRIORITY_MAIN-1, 
				THREAD_CREATE_STACKTEST, udp_send, argv, "sending a udp packet");

	}else if(argc == 3 && strcmp(argv[1], "server") ==0){
		kernel_pid_t server_thread = thread_create(server_thread_stack, 
				sizeof(server_thread_stack), THREAD_PRIORITY_MAIN-2, 
				THREAD_CREATE_STACKTEST, udp_server, argv, "create udp server");
	}else{
		printf("invalid command\n");
		return -1;
	}


	return 0;
}

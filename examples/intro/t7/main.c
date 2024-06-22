/*
 *	Mini gnrc application that counts the number of UDP messages it recieves
 *	uses sync waits, port and protocol specific net registries, 
 *	basic packet snips 
 */

#include <stdio.h>

#include "msg.h"
#include "net/ipv6/addr.h"
#include "net/gnrc.h"
#include "net/gnrc/netif.h"
#include "net/netif.h"

#include "net/gnrc/netreg.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/neterr.h"

#define SERVER_QUEUE_SIZE  (8)
msg_t server_queue[SERVER_QUEUE_SIZE ];


int main(void){

    puts("RIOT network stack example application");

    /* print all IPv6 addresses */
    printf("{\"IPv6 addresses\": [\"");
    netifs_print_ipv6("\", \"");
    puts("\"]}");

 

/*---------------------------------------------------------------------------*/	
	//initialize a message queue which is needed to accepts packets
	msg_init_queue(server_queue, SERVER_QUEUE_SIZE);
	uint32_t msg_counter = 0;
	uint32_t packet_release_error = GNRC_NETERR_SUCCESS;
/*---------------------------------------------------------------------------*/	
	//here we register to only take in a specific type of packets from a 
	//specific port

	//static initialization of a server's netregistry
	gnrc_netreg_entry_t server = GNRC_NETREG_ENTRY_INIT_PID(8080, 
			thread_getpid());
	gnrc_netreg_register(GNRC_NETTYPE_UDP, &server);

/*---------------------------------------------------------------------------*/	

	while(true){
		//blocks pragram execution until a message inturupt is triggered, aka sync
		//message wait
		msg_t message;
		printf("about to wait\n");
		msg_receive(&message);
		msg_counter ++;
		printf("Current number of messages received: %d\n", msg_counter);


		gnrc_pktsnip_t* packet= (gnrc_pktsnip_t*) message.content.ptr;
		gnrc_pktbuf_release_error(packet, packet_release_error);
	
		if(packet_release_error != GNRC_NETERR_SUCCESS){
			printf("error %d while attempting to release packet\n", 
					packet_release_error);
		}else{
			puts("packet released successfully\n");
		}
	}
/*---------------------------------------------------------------------------*/	
}

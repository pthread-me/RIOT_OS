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
#include "rules/my_rules.h"

#define DIRECTION (0) //up, from device to network gateway
#define MAX_PACKET_SIZE (128)

#define BYTE 

//extern uint8_t 
static void udp_send(char* dest_addr, char* dest_port, char* data){
	
	uint16_t port;
	ipv6_addr_t addr;

	//checks the ip address format
	if(ipv6_addr_from_str(&addr, dest_addr) == NULL){
		printf("Invalid ipv6 address\n");
		return;
	}


	//checks port format and range
	for(size_t i=0; i<strlen(dest_port); i++){
		if(!isdigit(dest_port[i])){
			printf("Invalid port format, must be an int\n");
			return;
		}
	}	
	port = (int16_t) strtol(dest_port, NULL, 10);

	gnrc_pktsnip_t *payload, *udp, *ip;
	
	payload = gnrc_pktbuf_add(NULL, (void*)data, strlen(data), GNRC_NETTYPE_UNDEF);
	

	if((udp = gnrc_udp_hdr_build(payload, 8000, port)) == NULL){
		printf("memory allocation error\n");
		return;
	}

	//made test change 
	if((ip = gnrc_ipv6_hdr_build(udp,  NULL , &addr)) == NULL){
		printf("memory allocation error\n");
		return;
	}


	gnrc_netif_t* netif = gnrc_netif_iter(NULL);
	gnrc_pktsnip_t* netif_hdr = gnrc_netif_hdr_build(NULL, 0, NULL, 0);
	((gnrc_netif_hdr_t*)netif_hdr->data)->if_pid = netif->pid;
	gnrc_pkt_prepend(ip, netif_hdr);


/*-----------------------------------------------------------------------------
	#ADDING THE SCHC COMPRESSTION BELLOW
-----------------------------------------------------------------------------*/
	int first = ip->size;
	int second = udp->size;
	printf("first:%d\nSecond:%d\n", first, second);
	schc_compressor_init();

	struct schc_compression_rule_t* comp_rule;
	uint8_t* out_buf = calloc(ip->size, sizeof(uint8_t));
	
	schc_bitarray_t bit_array = SCHC_DEFAULT_BIT_ARRAY(ip->size, out_buf);

	comp_rule = schc_compress(ip->data, ip->size, &bit_array, 0x45, UP);
	
	printf("devices : %d\n", DEVICE_COUNT);
	if(comp_rule == NULL){
		printf("not compressed\n");
	}
/*
	if(gnrc_netapi_dispatch_send(GNRC_NETTYPE_UDP, GNRC_NETREG_DEMUX_CTX_ALL, netif_hdr) <0){
		printf("couldnt dispatch packet\n");
		return;
	}
*/

	
/*-----------------------------------------------------------------------------
	#END OF SCHC
-----------------------------------------------------------------------------*/


}

int udp_cmds(int argc, char** argv){

	if(argc != 4){
		return -1;
	}

	udp_send(argv[1], argv[2], argv[3]);
	return 0;
}

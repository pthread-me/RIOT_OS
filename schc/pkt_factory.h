#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>

#include "net/ipv6/addr.h"
#include "net/gnrc.h"
#include "net/gnrc/ipv6.h"
#include "net/ipv6.h"
#include "net/gnrc/udp.h"
#include "net/nanocoap.h"
#include "net/coap.h"

typedef enum Packet_type {
    client_request,
    server_response,
    request_ack,
    response_ack
}Pkt_type;

typedef struct pkt_specifications{
    uint16_t src_port;
    uint16_t dest_port;
    uint8_t coap_code;
    uint8_t coap_type;
    uint16_t coap_id;
}pkt_specs;


pkt_specs* get_specs(uint8_t id){
    uint16_t src_port;
    uint16_t dest_port;
    uint8_t coap_code;
    uint8_t coap_type;
    uint16_t coap_id;

    switch (id) {
        //client request
        case 0:
            src_port = 60000;
            dest_port = 5684;
            coap_code = COAP_METHOD_GET;
            coap_type = COAP_TYPE_CON;
            coap_id = 0xffff;
            break;

        //server ack
        case 1:
            src_port = 5684;
            dest_port = 60000;
            //In hind sight there's a macro COAP_METHOD_VALID that does the same, but it looks prettier this way
            //aiming for the sequence 010 00011, first 3 bits are the type(2 == ack), (5 == valid)
            coap_code = COAP_CODE_VALID;//(COAP_TYPE_ACK << 5) | 3;
            coap_type = COAP_TYPE_ACK;
            coap_id = 0xffff;
            break;

        //server response
        case 2:
            src_port = 5684;
            dest_port = 60000;
            coap_code = COAP_METHOD_POST;
            coap_type = COAP_TYPE_CON;
            coap_id = 0xfffd;
            break;

        //client ack
        case 3:
            src_port = 60000;
            dest_port = 5684;
            coap_code = (COAP_TYPE_ACK << 5) | 3;
            coap_type = COAP_TYPE_ACK;
            coap_id = 0xfffd; // the reply to server response
            break;

        default:
            src_port =0;
            dest_port = 0;
            coap_code = COAP_CODE_EMPTY;
            coap_type = COAP_TYPE_NON;
            coap_id = 0x0000;
    }

    pkt_specs* pkt = calloc(1, sizeof(pkt_specs));

    pkt->src_port = src_port;
    pkt->dest_port = dest_port;
    pkt->coap_code = coap_code;
    pkt->coap_type = coap_type;
    pkt->coap_id = coap_id;

    return pkt;
}


gnrc_pktsnip_t* build_pkt(ipv6_addr_t ip_addr, uint8_t packet_id){

    gnrc_pktsnip_t *coap, *udp, *ip;
    pkt_specs* specs = get_specs(packet_id);

    uint64_t token_value = 0x0000000000000001;
    //token = gnrc_pktbuf_add(NULL, (void*) &token_value, 8, GNRC_NETTYPE_UNDEF);

	coap_hdr_t* coap_hdr = calloc(1, sizeof(coap_hdr_t));
	int coap_size = coap_build_hdr(coap_hdr, specs->coap_type, (void*)&token_value, 8, specs->coap_code, specs->coap_id);
	coap = gnrc_pktbuf_add(NULL, (void*)coap_hdr, coap_size, GNRC_NETTYPE_UNDEF);


    // hardcoding the token length to be 8 bytes
    uint8_t coap_hdr_tkl = ((coap_hdr_t*)coap->data)->ver_t_tkl;
    printf("original token length: %d\n", coap_hdr_tkl);
    uint8_t mask = 0x08;
    coap_hdr_tkl = coap_hdr_tkl | mask;
    printf("modified token length: %d\n", coap_hdr_tkl);

    //adding udp header
	if((udp = gnrc_udp_hdr_build(coap, specs->src_port, specs->dest_port)) == NULL){
		printf("cant add udp hdr\n");
		return NULL;
	}

    network_uint16_t be_dst= ((udp_hdr_t*)udp->data)->dst_port;
	uint16_t dst = (byteorder_btols(be_dst)).u16;
	printf("%d\n", dst);


	//adding ip header
	if((ip = gnrc_ipv6_hdr_build(udp,  &ip_addr , &ip_addr)) == NULL){
		printf("cant add ipv6 hdr\n");
		return NULL;
	}
     ((ipv6_hdr_t*)ip->data)->nh = PROTNUM_UDP;


	//merging ip, udp and coap headers so its a single gnrc_pktsnip_t since
    //libscshc doesn' work on seperate headers
	if(ip && gnrc_pktbuf_merge(ip) == 0){
	    printf("\nsize of all headers combined is: %d bytes\n",
			ip->size);

	    return ip;
    }

	printf("couldn't merge ip and udp packets\n");
    return NULL;
}

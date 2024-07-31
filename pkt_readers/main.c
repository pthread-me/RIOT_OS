#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define IPV6_HDR_SIZE 40

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

char* read_pkt(int size){
	char* line = calloc(size, sizeof(char));
	size_t len = 0;

	ssize_t chars_read = getline(&line, &len, stdin);
	if(chars_read <0){
		printf("no chars read");
		return NULL;
	}

	return line;
}

void nice_ipv6(char* pkt){
	printf("\nversion:\n");
	for(int i=0; i<4; i++){
		printf("%c", pkt[i]);
	}

	printf("\ntraffic class\n");
	for(int i=4; i<12; i++){
		printf("%c", pkt[i]);
	}

	printf("\nflow label\n");
	for(int i=12; i<32; i++){
		printf("%c", pkt[i]);
	}

	printf("\npayload len\n");
	for(int i=32; i<48; i++){
		printf("%c", pkt[i]);
	}

	printf("\nnext hdr\n");
	for(int i=48; i<56; i++){
		printf("%c", pkt[i]);
	}

	printf("\nhop limit\n");
	for(int i=56; i<64; i++){
		printf("%c", pkt[i]);
	}

	printf("\nsrc addr\n");
	for(int i=64; i<192; i++){
		printf("%c", pkt[i]);
	}

	printf("\ndst len\n");
	for(int i=192; i<289; i++){
		printf("%c", pkt[i]);
	}


}

int main(){
	char* pkt = read_pkt(IPV6_HDR_SIZE);
	nice_ipv6(pkt);	


}

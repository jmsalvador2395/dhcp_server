#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <string.h>

#pragma comment(lib,"ws2_32.lib") //winsock library

#define BUFLEN 1024
#define SERVER_IP 0xC0A80101 //your ip (192.168.1.1 for now)
#define NETMASK 0xFFFFFF00 //netmask (255.255.255.0 for now)

#define BROADCAST "255.255.255.255"
#define END 0xff
#define SRC_PORT 67
#define DST_PORT 68

/*
 * note:
 * sizeofbyte == sizeofchar == sizeofhex*2
 *
 */

void print_client_message(unsigned char* buff){

	//read the discover message
	int index=0;
	unsigned char msg_type=buff[index++];
	unsigned char hw_type=buff[index++];
	unsigned char hw_addr_len=buff[index++];
	unsigned char hops=buff[index++];

	//xid is always 4 bytes (8 hex characters)
	//store this
	unsigned char xid[4];
	for(int i=0;i<4;i++)
		xid[i]=buff[index++];

	
	//seconds elapsed is always 2 byte (4 hex characters)
	unsigned char seconds_elapsed[2]={buff[index], buff[index+2]};
	index+=2;
	
	//bootp flags are always 2 bytes
	//usually all zeros but may need to set the first bit to 1 due to my implementation (broadcast flag)
	unsigned char flags[2]={buff[index], buff[index+1]};
	index+=2;

	//always 4 bytes (32 bits, 8 hex characters)
	unsigned char client_ip[4];
	for(int i=0;i<4;i++)
		client_ip[i]=buff[index++];

	//always 4 bytes
	unsigned char your_ip[4];
	for(int i=0;i<4;i++)
		your_ip[i]=buff[index++];

	//always 4 bytes
	unsigned char next_server_ip[4];
	for(int i=0;i<4;i++)
		next_server_ip[i]=buff[index++];

	//usually all zeros. won't worry about this until maybe later
	unsigned char relay_ip[4];
	for(int i=0;i<4;i++)
		relay_ip[i]=buff[index++];

	//client mac is always 6 bytes but 16 bytes total is used (last 10 are used for padding)
	unsigned char mac_address[16];
	for(int i=0;i<16;i++)
		mac_address[i]=buff[index++];

	//uses 64 bytes total. shouldn't have to worry about putting anything in this
	unsigned char server_hostname[64];
	for(int i=0;i<64;i++)
		server_hostname[i]=buff[index++];

	//uses 128 bytes total. also shouldn't have to worry about this yet
	unsigned char boot_file_name[128];
	for(int i=0;i<128;i++)
		boot_file_name[i]=buff[index++];

	//indicates that it's a dhcp message. always 63 82 53 63
	unsigned char magic_cookie[4];
	for(int i=0;i<4;i++)
		magic_cookie[i]=buff[index++];

	int count=0;
	int temp_index=index;
	unsigned char temp;
	while(!buff[temp_index]&0xff){
		count++;
		temp_index++;

	}
	printf("%02x\n",buff[temp_index]);
	printf("%d\n",count);

	//read dhcp options until 0xFF (end) is read
	unsigned char option;
	option=buff[index++];
	
	while(option!=0xff){
		printf("Option: %02x\n",option);
		int length=0|buff[index++];
		printf("\tLength: %d\n",length);
		for(int i=0;i<length;i++)
			printf("\t\t%02x\n",buff[index++]);
		option=buff[index++];
	}
	printf("Option: %02x (end)\n",option);
	printf("***********************************************************************************************************************\n\n");
	//end reading discover message
	return;
}



int main(int argc, char *argv[]){
	SOCKET skt;
	int slen, recv_len;
	struct sockaddr_in rcvr, tsmtr, client;
	unsigned char buffer[BUFLEN];
	WSADATA wsa;
	int error;

	slen=sizeof(rcvr);
	printf("Initialising...\n\n");
	if(WSAStartup(MAKEWORD(2,2),&wsa)!=0){

		printf("Failed. Error Code: %d",WSAGetLastError());
		return 1;
	}

	printf("Winsock Initialised\n\n");

	skt=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); //IPPROTO_UDP was 0 originally
	if(skt == INVALID_SOCKET){
		error=WSAGetLastError();
		printf("Creating socket failed: %d\n", error);
		return 1;
	}
	printf("Socket created\n\n");
	
	memset(&rcvr, 0, sizeof(rcvr));
	rcvr.sin_family=AF_INET;
	rcvr.sin_addr.s_addr=INADDR_ANY;
	rcvr.sin_port=htons(SRC_PORT);

	if(bind(skt,(struct sockaddr *)&rcvr, sizeof(rcvr))==SOCKET_ERROR){
		error=WSAGetLastError();
		printf("Bind failed. Error code: %d\n",error);
		exit(EXIT_FAILURE);
	}
	puts("Bind done");

	while(1){
		printf("Waiting for data...\n");
		fflush(stdout);

		memset(buffer,'\0',BUFLEN);
		if((recv_len=recvfrom(skt, buffer, BUFLEN, 0, (struct sockaddr*) &client, &slen))==SOCKET_ERROR){
			error=WSAGetLastError();
			printf("recvfrom() failed. Error code: %d\n", error);
			exit(EXIT_FAILURE);
		}
		else{
			printf("Received packet from %s:%d\n\n",inet_ntoa(client.sin_addr), ntohs(client.sin_port));
			printf("***********************************************************************************************************************\n\n");
			print_client_message(buffer);
			unsigned char reply[BUFLEN] = "hi\n";
			if(sendto(skt, reply, recv_len, 0, (struct sockaddr*) &client, slen) == SOCKET_ERROR){
				error=WSAGetLastError();
				printf("recvfrom() failed. Error code: %d\n", error);
				exit(EXIT_FAILURE);
			}
			/*
			if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &amp;si_other, slen) == SOCKET_ERROR){
					printf(&quot;sendto() failed with error code : %d&quot; , WSAGetLastError());
					exit(EXIT_FAILURE);
			}
			*/
		}
	}
	return 0;
	
}


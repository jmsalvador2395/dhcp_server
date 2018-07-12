#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <string.h>

#pragma comment(lib,"ws2_32.lib") //winsock library

#define BUFLEN 1024
#define SERVER_IP 0xc0a80101 //your ip (192.168.1.1)
#define NETMASK 0xffffff00 //netmask (255.255.255.0)

#define END 0xff

void process_client_message(unsigned char *buff);

void send_offer_message();
int main(int argc, char *argv[]){
	int server_fd;
	SOCKET server_skt;
	int slen, recv_len;
	struct sockaddr_in server, si_other;
	unsigned char buffer[BUFLEN];
	WSADATA wsa;
	int error;

	slen=sizeof(si_other);
	printf("Initialising Winsock...\n\n");
	if(WSAStartup(MAKEWORD(2,2),&wsa)!=0){

		printf("Failed. Error Code: %d",WSAGetLastError());
		return 1;
	}

	printf("Winsock Initialised\n\n");

	if((server_skt=socket(AF_INET, SOCK_DGRAM, 0))==INVALID_SOCKET){
		error=WSAGetLastError();
		printf("Creating socket failed: %d\n", error);
		return 1;
	}
	printf("Socket created\n\n");
	
	server.sin_family=AF_INET;
	server.sin_addr.s_addr=INADDR_ANY;
	server.sin_port=htons(67);

	if(bind(server_skt,(struct sockaddr *)&server, sizeof(server))==SOCKET_ERROR){
		error=WSAGetLastError();
		printf("Bind failed. Error code: %d\n",error);
		exit(EXIT_FAILURE);
	}
	puts("Bind done");

	while(1){
		printf("Waiting for data...\n");
		fflush(stdout);

		memset(buffer,'\0',BUFLEN);
		if((recv_len=recvfrom(server_skt, buffer, BUFLEN,0, (struct sockaddr *) &si_other, &slen))==SOCKET_ERROR){
			error=WSAGetLastError();
			printf("recvfrom() failed. Error code: %d\n", error);
			exit(EXIT_FAILURE);
		}
		else{
			printf("Received packet from %s:%d\n\n",inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
			printf("***********************************************************************************************************************\n\n");
			process_client_message(buffer);

		}
	}
	return 0;
	
}

void process_client_message(unsigned char *buff){






	int endloop; //delete this




	//read the discover message
	int index=0;
	unsigned char msg_type=buff[index++];
	unsigned char hw_type=buff[index++];
	unsigned char hw_addr_len=buff[index++];
	unsigned char hops=buff[index++];

	unsigned char xid[4];
	for(int i=0;i<4;i++)
		xid[i]=buff[index++];

	unsigned char seconds_elapsed[2]={buff[index], buff[index+2]};
	index+=2;
	
	unsigned char flags[2]={buff[index], buff[index+1]};
	index+=2;

	unsigned char client_ip[4];
	for(int i=0;i<4;i++)
		client_ip[i]=buff[index++];

	unsigned char your_ip[4];
	for(int i=0;i<4;i++)
		your_ip[i]=buff[index++];

	unsigned char next_server_ip[4];
	for(int i=0;i<4;i++)
		next_server_ip[i]=buff[index++];

	unsigned char relay_ip[4];
	for(int i=0;i<4;i++)
		relay_ip[i]=buff[index++];

	unsigned char mac_address[16];
	for(int i=0;i<16;i++)
		mac_address[i]=buff[index++];

	unsigned char server_hostname[64];
	for(int i=0;i<64;i++)
		server_hostname[i]=buff[index++];

	unsigned char boot_file_name[128];
	for(int i=0;i<128;i++)
		boot_file_name[i]=buff[index++];
	
	unsigned char magic_cookie[4];
	for(int i=0;i<4;i++)
		magic_cookie[i]=buff[index++];

	int count=0;
	int temp_index=index;
	unsigned char temp;
	printf("hi\n");
	while(!buff[temp_index]&0xff){
		count++;
		temp_index++;

	}
	printf("%02x\n",buff[temp_index]);
	printf("%d\n",count);

//	unsigned char option;
//	option=buff[index++];
	
	/*
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
	*/
	//end reading discover message
	return;
}

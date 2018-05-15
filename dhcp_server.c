#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <string.h>

#pragma comment(lib,"ws2_32.lib") //winsock library

#define BUFLEN 1024
#define SERVER_IP 0xc0a80101 //your ip (192.168.1.1)
#define NETMASK 0xffffff00 //netmask (255.255.255.0)

#define END 0xff

void proccess_client_message(unsigned char *buff);

void send_offer_message(
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
			proccess_client_message(buffer);

		}
	}
	return 0;
	
}

void proccess_client_message(unsigned char *buff){
	int index=0;
	printf("Message Type:\n\t%02x\n", buff[index++]);

	printf("Hardware Type:\n\t%02x\n", buff[index++]);

	printf("Hardware Address Length:\n\t%02x\n", buff[index++]);

	printf("Hops:\n\t%02x\n", buff[index++]);

	int endloop=index+4;
	printf("Transaction ID:\n\t");
	while(index<endloop)
		printf("%02x ",buff[index++]);
	printf("\n");

	printf("Seconds Elapsed:\n\t%02x %02x\n",buff[index],buff[index+1]);
	index+=2;

	printf("Flags:\n\t%02x %02x\n",buff[index], buff[index+1]);
	index+=2;

	endloop=index+4;
	printf("Client IP:\n\t");
	while(index<endloop)
		printf("%02x ",buff[index++]);
	printf("\n");

	endloop=index+4;
	printf("Your (client) IP:\n\t");
	while(index<endloop)
		printf("%02x ",buff[index++]);
	printf("\n");

	endloop=index+4;
	printf("Next Server IP:\n\t");
	while(index<endloop)
		printf("%02x ",buff[index++]);
	printf("\n");

	endloop=index+4;
	printf("Relay Agent IP:\n\t");
	while(index<endloop)
		printf("%02x ",buff[index++]);
	printf("\n");

	endloop=index+16;
	printf("Client MAC Address:\n\t");
	while(index<endloop)
		printf("%02x ",buff[index++]);
	printf("\n");
	
	endloop=index+64;
	printf("Server Host Name:\n\t");
	while(index<endloop)
		printf("%02x ",buff[index++]);
	printf("\n");

	endloop=index+128;
	printf("Boot File Name:\n\t");
	while(index<endloop)
		printf("%02x ",buff[index++]);
	printf("\n");
	
	endloop=index+4;
	printf("Magic Cookie:\n\t");
	while(index<endloop)
		printf("%02x ",buff[index++]);
	printf("\n");

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

	return;
}

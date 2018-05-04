#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <string.h>

#pragma comment(lib,"ws2_32.lib") //winsock library

#define BUFLEN 1024
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
			int index=0;
			printf("OP:\n\t%02x\n", buffer[index++]);

			printf("HTYPE:\n\t%02x\n", buffer[index++]);

			printf("HLEN:\n\t%02x\n", buffer[index++]);

			printf("HOPS:\n\t%02x\n", buffer[index++]);

			int endloop=index+4;
			printf("XID:\n\t");
			while(index<endloop)
				printf("%02x ",buffer[index++]);
			printf("\n");

			printf("Seconds Elapsed:\n\t%02x %02x\n",buffer[index],buffer[index+1]);
			index+=2;

			printf("FLAGS:\n\t%02x %02x\n",buffer[index], buffer[index+1]);
			index+=2;

			endloop=index+4;
			printf("Client IP:\n\t");
			while(index<endloop)
				printf("%02x ",buffer[index++]);
			printf("\n");

			endloop=index+4;
			printf("Your (client) IP:\n\t");
			while(index<endloop)
				printf("%02x ",buffer[index++]);
			printf("\n");

			endloop=index+4;
			printf("Next Server IP:\n\t");
			while(index<endloop)
				printf("%02x ",buffer[index++]);
			printf("\n");

			endloop=index+4;
			printf("Relay Agent IP:\n\t");
			while(index<endloop)
				printf("%02x ",buffer[index++]);
			printf("\n");
	
			endloop=index+16;
			printf("Client MAC Address:\n\t");
			while(index<endloop)
				printf("%02x ",buffer[index++]);
			printf("\n");
			
			endloop=index+80;
			printf("Server Host Name:\n\t");
			while(index<endloop)
				printf("%02x ",buffer[index++]);
			printf("\n");
		
			endloop=index+144;
			printf("Boot File Name:\n\t");
			while(index<endloop)
				printf("%02x ",buffer[index++]);
			printf("\n");
			
			endloop=index+4;
			printf("Magic Cookie:\n\t");
			while(index<endloop)
				printf("%02x ",buffer[index++]);
			printf("\n");


			printf("***********************************************************************************************************************\n\n");
		}
	}
	return 0;
	
}

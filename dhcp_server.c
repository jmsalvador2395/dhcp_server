#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <string.h>

#pragma comment(lib,"ws2_32.lib") //winsock library

#define BUFLEN 512
int main(int argc, char *argv[]){
	int server_fd;
	SOCKET server_skt;
	int slen, recv_len;
	struct sockaddr_in server, si_other;
	unsigned char buffer[BUFLEN];
	WSADATA wsa;

	printf("Initialising Winsock...\n\n");
	if(WSAStartup(MAKEWORD(2,2),&wsa)!=0){
		printf("Failed. Error Code: %d",WSAGetLastError());
		return 1;
	}

	printf("Winsock Initialised\n\n");

	if((server_skt=socket(AF_INET, SOCK_DGRAM, 0))==INVALID_SOCKET){
		printf("Creating socket failed: %d\n", WSAGetLastError());
		return 1;
	}
	printf("Socket created\n\n");
	
	server.sin_family=AF_INET;
	server.sin_addr.s_addr=INADDR_ANY;
	server.sin_port=htons(67);

	if(bind(server_skt,(struct sockaddr *)&server, sizeof(server))==SOCKET_ERROR){
		printf("Bind failed. Error code: %d\n",WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	puts("Bind done");

	while(1){
		printf("Waiting for data...\n");
		fflush(stdout);

		memset(buffer,'\0',BUFLEN);
		if((recv_len=recvfrom(server_skt, buffer, BUFLEN,0, (struct sockaddr *) &si_other, &slen))==SOCKET_ERROR){
			printf("recvfrom() failed. Error code: %d\n", WSAGetLastError());
			exit(EXIT_FAILURE);
		}
		else{
			printf("******************************************************************\n\n");
			printf("Received packet from %s:%d\n",inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
			int index=0;
			printf("OP: %x\n", buffer[index++]);
			printf("HTYPE: %x\n", buffer[index++]);
			printf("HLEN: %x\n", buffer[index++]);
			printf("HOPS: %x\n", buffer[index++]);
			printf("XID: %x %x %x %x\n",buffer[index],buffer[index+1],buffer[index+2],buffer[index+3]);
			index+=4;
			printf("SECS: %x %X\n",buffer[index],buffer[index+1]);
			index+=2;
			printf("FLAGS: %x %x\n",buffer[index], buffer[index+1]);
			index+=2;
			printf("Client IP: %x %x %x %x\n",buffer[index],buffer[index+1],buffer[index+2],buffer[index+3]);
			index+=4;
			printf("Server IP: %x %x %x %x\n",buffer[index],buffer[index+1],buffer[index+2],buffer[index+3]);
			index+=4;
			printf("Gateway IP: %x %x %x %x\n",buffer[index++],buffer[index++],buffer[index++],buffer[index++]);
			printf("Client HW Address: \n\t%x %x %x %x\n\t%x %x %x %x\n\t%x %x %x %x\n\t%x %x %x %x\n"
					,buffer[index],buffer[index+1],buffer[index+2],buffer[index+3]
					,buffer[index+4],buffer[index+5],buffer[index+6],buffer[index+7]
					,buffer[index+8],buffer[index+9],buffer[index+10],buffer[index+11]
					,buffer[index+12],buffer[index+13],buffer[index+14],buffer[index+15]
				  );
			index+=16;
			printf("******************************************************************\n\n");
		}
	}
	return 0;
	
}

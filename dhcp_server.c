#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <string.h>
#include "dhcpcontroller.h"

#pragma comment(lib,"ws2_32.lib") //winsock library

#define BUFLEN 512
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

/*
 * need to figure out how to deal with invalid packets. 
 * I was think to probably use the build_reply function 
 * to return an integer based on whether the message was good or not.
 * it would probably be more correct to have a separate function for checking
 * if the message is valid but i think it would be more efficient to do 
 * it the first way.
 */


/*
 * holds client data
 * data types probably aren't right but i'll come back to fix it
 */
struct client{
	int ip;
	char hostname[63];
	unsigned char macaddress[6];
	long int time_leased;
};

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

	printf("Bind done\n");

	while(1){
		printf("Waiting for data...\n");
		fflush(stdout);

		memset(buffer,'\0',BUFLEN);
		//receive dhcp message
		if((recv_len=recvfrom(skt, buffer, BUFLEN, 0, (struct sockaddr*) &client, &slen))==SOCKET_ERROR){
			error=WSAGetLastError();
			printf("recvfrom() failed. Error code: %d\n", error);
			exit(EXIT_FAILURE);
		}
		//gets here if there weren't any errors receiving
		else{
			printf("Received packet from %s:%d\n\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
			printf("***********************************************************************************************************************\n\n");
			
			unsigned char reply[BUFLEN];
			memset(reply,'\0',BUFLEN);

			int replylen=BUFLEN;//i don't really need this but i'm also not sure why i put this in here so i'll leave it
			buildreply(buffer, BUFLEN, reply, &replylen);
			//print my reply
			printf("*********after copy**********\n");
			for(int i=0; i<32; i++){
				for(int j=0; j<16; j++){
					printf("%02x ", reply[(16*i)+j]);
				}
				printf("\n");
			}

			//client.sin_addr.s_addr=inet_addr("255.255.255.255");//not sure if i have to keep this
			/*
			if(sendto(skt, reply, strlen(reply), 0, (struct sockaddr*) &client, slen) == SOCKET_ERROR){
				error=WSAGetLastError();
				printf("sendto() failed. Error code: %d\n", error);
				exit(EXIT_FAILURE);
			}
			*/
			printf("Response sent (not really)\n");
			/*
			if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == SOCKET_ERROR){
					printf("sendto() failed with error code : %d" , WSAGetLastError());
					exit(EXIT_FAILURE);
			}
			*/
			free(reply);
		}
	}
	return 0;
	
}

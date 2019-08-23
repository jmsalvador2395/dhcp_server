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

unsigned char* buildreply(unsigned char* clientmsg, int client_msg_len){
	int msgposition=0;//keeps track of the position in the reply array
	int replyposition=0;
	
	char reply[BUFLEN];
	memset(reply, '\0', BUFLEN);
	reply[0]='h';//delete this later

	unsigned char msg_type=clientmsg[msgposition++];
	unsigned char hw_type=clientmsg[msgposition++];
	unsigned char hw_addr_len=clientmsg[msgposition++];
	unsigned char hops=clientmsg[msgposition++];

	//xid is always 4 bytes (8 hex characters)
	unsigned char xid[4];
	for(int i=0;i<4;i++)
		xid[i]=clientmsg[msgposition++];

	
	//seconds elapsed is always 2 byte (4 hex characters)
	unsigned char seconds_elapsed[2]={clientmsg[msgposition], clientmsg[msgposition+2]};
	msgposition+=2;
	
	//bootp flags are always 2 bytes
	//usually all zeros but may need to set the first bit to 1 due to my implementation (broadcast flag)
	unsigned char flags[2]={clientmsg[msgposition], clientmsg[msgposition+1]};
	msgposition+=2;

	//always 4 bytes (32 bits, 8 hex characters)
	unsigned char client_ip[4];
	for(int i=0;i<4;i++)
		client_ip[i]=clientmsg[msgposition++];

	//always 4 bytes
	unsigned char your_client_ip[4];
	for(int i=0;i<4;i++)
		your_client_ip[i]=clientmsg[msgposition++];

	//always 4 bytes
	unsigned char next_server_ip[4];
	for(int i=0;i<4;i++)
		next_server_ip[i]=clientmsg[msgposition++];

	//usually all zeros. won't worry about this until maybe later
	unsigned char relay_ip[4];
	for(int i=0;i<4;i++)
		relay_ip[i]=clientmsg[msgposition++];

	//client mac is always 6 bytes but 16 bytes total is used (last 10 are used for padding)
	unsigned char mac_address[16];
	for(int i=0;i<16;i++)
		mac_address[i]=clientmsg[msgposition++];

	//uses 64 bytes total. shouldn't have to worry about putting anything in this
	unsigned char server_hostname[64];
	for(int i=0;i<64;i++)
		server_hostname[i]=clientmsg[msgposition++];

	//uses 128 bytes total. also shouldn't have to worry about this yet
	unsigned char boot_file_name[128];
	for(int i=0;i<128;i++)
		boot_file_name[i]=clientmsg[msgposition++];

	//indicates that it's a dhcp message. always 63 82 53 63
	unsigned char magic_cookie[4];
	for(int i=0;i<4;i++)
		magic_cookie[i]=clientmsg[msgposition++];
	
	//read dhcp options until 0xFF (end) is read
	unsigned char option;
	option=clientmsg[msgposition++];

	/*
	 * this is where we process the dhcp options
	 */
	while(option!=0xff){
		int optionlength=clientmsg[msgposition++];
		unsigned char *hostname, *requestlist;
		char bitflags='0';
		switch(option){
			case 0x0c://host name
				hostname=malloc(optionlength*sizeof(char));
				for(int i=0; i<optionlength; i++)
					hostname[i]=clientmsg[msgposition++];
				printf("2\n");
				break;

			case 0x35://dhcp message type
				unsigned char msg_type=clientmsg[msgposition++];
				printf("1\n");
				break;

			case 0x37://parameter request list
				requestlist=malloc(optionlength*sizeof(char));
				int listlength=optionlength;
				for(int i=0; i<optionlength; i++)
					requestlist[i]=clientmsg[msgposition++];
				printf("3\n");
				break;
		}
		option=msgposition++;
	}

	free(requestlist);
	free(hostname);




	return reply;
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
			
			unsigned char* reply=malloc(BUFLEN*sizeof(char));
			int replylen=BUFLEN;//i don't really need this but i'm also not sure why i put this in here so i'll leave it
			reply=buildreply(buffer, BUFLEN);

			client.sin_addr.s_addr=inet_addr("255.255.255.255");//not sure if i have to keep this
			if(sendto(skt, reply, strlen(reply), 0, (struct sockaddr*) &client, slen) == SOCKET_ERROR){
				error=WSAGetLastError();
				printf("sendto() failed. Error code: %d\n", error);
				exit(EXIT_FAILURE);
			}
			printf("Response sent\n");
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


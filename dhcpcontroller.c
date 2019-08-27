#include <stdio.h>
#include <stdlib.h>

#include "dhcpcontroller.h"


unsigned char* buildreply(unsigned char* clientmsg, int client_msg_len){
	int msgpos=0;//keeps track of the position in the reply array

	unsigned char msg_type=clientmsg[msgpos++];
	unsigned char hw_type=clientmsg[msgpos++];
	unsigned char hw_addr_len=clientmsg[msgpos++];
	unsigned char hops=clientmsg[msgpos++];

	//xid is always 4 bytes (8 hex characters)
	unsigned char xid[4];
	for(int i=0;i<4;i++)
		xid[i]=clientmsg[msgpos++];

	
	//seconds elapsed is always 2 byte (4 hex characters)
	unsigned char seconds_elapsed[2]={clientmsg[msgpos], clientmsg[msgpos+2]};
	msgpos+=2;
	
	//bootp flags are always 2 bytes
	//usually all zeros but may need to set the first bit to 1 due to my implementation (broadcast flag)
	unsigned char flags[2]={clientmsg[msgpos], clientmsg[msgpos+1]};
	msgpos+=2;

	//always 4 bytes (32 bits, 8 hex characters)
	unsigned char client_ip[4];
	for(int i=0;i<4;i++)
		client_ip[i]=clientmsg[msgpos++];

	//always 4 bytes
	unsigned char your_client_ip[4];
	for(int i=0;i<4;i++)
		your_client_ip[i]=clientmsg[msgpos++];

	//always 4 bytes
	unsigned char next_server_ip[4];
	for(int i=0;i<4;i++)
		next_server_ip[i]=clientmsg[msgpos++];

	//usually all zeros. won't worry about this until maybe later
	unsigned char relay_ip[4];
	for(int i=0;i<4;i++)
		relay_ip[i]=clientmsg[msgpos++];

	//client mac is always 6 bytes but 16 bytes total is used (last 10 are used for padding)
	unsigned char mac_address[16];
	for(int i=0;i<16;i++)
		mac_address[i]=clientmsg[msgpos++];

	//uses 64 bytes total. shouldn't have to worry about putting anything in this
	unsigned char server_hostname[64];
	for(int i=0;i<64;i++)
		server_hostname[i]=clientmsg[msgpos++];

	//uses 128 bytes total. also shouldn't have to worry about this yet
	unsigned char boot_file_name[128];
	for(int i=0;i<128;i++)
		boot_file_name[i]=clientmsg[msgpos++];

	//indicates that it's a dhcp message. always 63 82 53 63
	unsigned char magic_cookie[4];
	for(int i=0;i<4;i++)
		magic_cookie[i]=clientmsg[msgpos++];
	
	//read dhcp options until 0xFF (end) is read
	unsigned char option;
	option=clientmsg[msgpos++];

	/*
	 * this is where we process the dhcp options
	 */
	while(option!=0xff){
		int optionlength=(int) clientmsg[msgpos++];
		printf("option: %d\noptionlength: %d\n", option, optionlength);
		unsigned char *hostname, *requestlist;
		char bitflags='0';
		switch(option){
			case 0x0c://host name
				printf("2\n");
				hostname=(char *)malloc(optionlength*sizeof(char));
				for(int i=0; i<optionlength; i++)
					hostname[i]=clientmsg[msgpos++];
				break;
				free(hostname);

			case 0x35://dhcp message type
				printf("1\n");
				unsigned char msg_type=clientmsg[msgpos++];
				printf("message type: %d\n", (int) msg_type);
				break;

			case 0x37://parameter request list
				printf("3\n");
				requestlist=(char *)malloc(optionlength*sizeof(char));
				int listlength=optionlength;
				for(int i=0; i<optionlength; i++)
					requestlist[i]=clientmsg[msgpos++];
				free(requestlist);
				break;
			default:
				printf("defaulted to option: %d\n", (int) option);
				for(int i=0; i<optionlength; i++)
					msgpos++;
				break;
		}
		option=clientmsg[msgpos++];
	}

	/*
	 * start building the reply message in this section
	 */
	int repos=0; //position in reply string
	char reply[BUFLEN];
	memset(reply, '\0', BUFLEN);


	//work on this later
	if(msg_type == 1){
		printf("BOOT REQUEST\n");
		reply[repos++]=0x2;//0x2 indicates a Boot Reply message
	}
	else{
		return "invalid message";//temporary
	}

	reply[repos++]=hw_addr_len;
	reply[repos++]=hops;
	for(int i=0; i<4; i++)
		reply[repos++]=xid[i];
	reply[repos++]=seconds_elapsed;

	/*
	 * i'm pretty sure that the broadcast flag should be set to 1 but
	 * i'm keeping the line below just in case
	 */
	//reply[repos++]=0x0;
	reply[repos++]=0x8;
	reply[repos++]=0;//reserved bootp flags
	
	//TODO currently here at building the reply message
	
	

	return "sup\n";
	//return reply;
}


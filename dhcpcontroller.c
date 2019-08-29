#include <stdio.h>
#include <stdlib.h>

#include "dhcpcontroller.h"

#define BUFLEN 512


int buildreply(unsigned char* clientmsg, int clientmsglen, unsigned char* replymsg, int* replymsglen){
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
	unsigned char dhcp_msg_type;

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
				dhcp_msg_type=clientmsg[msgpos++];
				printf("message type: %d\n", (int) dhcp_msg_type);
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
	unsigned char reply[BUFLEN];
	memset(reply, '\0', BUFLEN);


	//work on this later
	if(msg_type == 1){
		printf("BOOT REQUEST\n");
		reply[repos++]=0x2;//0x2 indicates a Boot Reply message
	}
	else{
		return 1;//temporary
	}

	reply[repos++]=hw_addr_len;
	reply[repos++]=hops;
	for(int i=0; i<4; i++)
		reply[repos++]=xid[i];

	reply[repos++]=seconds_elapsed[0];
	reply[repos++]=seconds_elapsed[1];

	/*
	 * i'm pretty sure that the broadcast flag should be set to 1 but
	 * i'm keeping the line below just in case
	 */
	//reply[repos++]=0x0;
	reply[repos++]=0x8;
	reply[repos++]=0;//reserved bootp flags
	
	//client ip address
	//the packets i've looked at seem to all leave this at 0.0.0.0
	//kinda feels cheesy
	for(int i=0; i<4; i++)
		reply[repos++]=0x0;	

	/*
	 * offered client ip
	 * definitely cheezin it
	 * add function later to pick from a list of IPs
	 */
	reply[repos++]=192;
	reply[repos++]=168;
	reply[repos++]=0;
	reply[repos++]=2;

	/*
	 * server ip (192.168.0.1 for now)
	 * feels like i'm cheezin here too tbh
	 * but i'll do something about it later
	 */
	reply[repos++]=192;
	reply[repos++]=168;
	reply[repos++]=0;
	reply[repos++]=1;

	/*
	 * relay agent IP
	 * maybe it's not that cheesy idk
	 */ 
	for(int i=0; i<4; i++)
		reply[repos++]=0x0;	

	//client mac address (with 10 bytes of padding)
	for(int i=0; i<16; i++)
		reply[repos++]=mac_address[i];

	//server hostname	
	//captured packets seem to always keep this at 0
	for(int i=0; i<64; i++)
		reply[repos++]=0;

	//magic cookie
	reply[repos++]=0x63;
	reply[repos++]=0x82;
	reply[repos++]=0x53;
	reply[repos++]=0x63;

	/*
	 * this is the section where i start adding in options
	 * for now i'll just manually put them in (mega cheese)
	 */

	//message type option (53)
	reply[repos++]=0x35;//option 53
	reply[repos++]=0x1;//length 1
	if(dhcp_msg_type == 0x1){
		reply[repos++]=0x2;//offer
	}
	else if(dhcp_msg_type == 0x3){
		reply[repos++]=0x5;//ack
	}

	//subnet mask (option 1)
	reply[repos++]=0x1;//option 1
	reply[repos++]=4;//option length
	reply[repos++]=0xff;
	reply[repos++]=0xff;
	reply[repos++]=0xff;
	reply[repos++]=0x0;

	//time offset (option 2)
	reply[repos++]=0x2;
	reply[repos++]=0x4;
	reply[repos++]=0;
	reply[repos++]=0;
	reply[repos++]=0;
	reply[repos++]=0;

	
	//router (option 3)
	reply[repos++]=0x3;//option 3
	reply[repos++]=0x4;//length
	//192.168.0.254
	reply[repos++]=192;
	reply[repos++]=168;
	reply[repos++]=0;
	reply[repos++]=254;

	//TTL (option 23)
	reply[repos++]=0x17;
	reply[repos++]=0x1;
	reply[repos++]=0x40;//64
	
	//lease time (option 51)
	reply[repos++]=0x33;
	reply[repos++]=0x4;
	//14 days or 1209600 seconds
	reply[repos++]=0x00;
	reply[repos++]=0x12;
	reply[repos++]=0x75;
	reply[repos++]=0x00;

	//server identifier (option 54)
	reply[repos++]=0x36;
	reply[repos++]=0x4;
	//192.168.0.1
	reply[repos++]=192;
	reply[repos++]=168;
	reply[repos++]=0;
	reply[repos++]=1;

	//renewal time (option 58)
	reply[repos++]=0x3a;
	reply[repos++]=0x4;
	//7 days or 604800 seconds
	reply[repos++]=0x00;
	reply[repos++]=0x09;
	reply[repos++]=0x3a;
	reply[repos++]=0x80;
	
	//rebindingn time (option 59)
	reply[repos++]=0x3b;
	reply[repos++]=0x4;
	//12 days, 6 hours or 1058400 seconds
	reply[repos++]=0x00;
	reply[repos++]=0x10;
	reply[repos++]=0x26;
	reply[repos++]=0x60;

	//end
	reply[repos++]=0xff;
	//TODO currently here at building the reply message
	
	
	for(int i=0; i<repos; i++)
		replymsg[i]=reply[i];
	*replymsglen=repos;

	return 0;
}


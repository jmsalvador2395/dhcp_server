#ifndef _DHCPCONTROLLER_H_
#define _DHCPCONTROLLER_H_

int buildreply(unsigned char* clientmsg, int clientmsglen, unsigned char* replymsg, int* replymsglen);

#endif

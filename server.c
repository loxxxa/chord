/*
 *	server
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "util.h"

/*
typedef union {
    unsigned int uint;
    unsigned char byte[4];
} conversion;
*/

/* transforms an integer to byte array (4 bytes) */
unsigned int 
btoi(const char *byte, const int init) 
{
//    unsigned int num = 0;

/*    num = (num << 8) + byte[init+3];
    num = (num << 8) + byte[init+2];
    num = (num << 8) + byte[init+1];
    num = (num << 8) + byte[init];

    return num; 
*/
    return (((byte[init] & 0xFF) << 24) | ((byte[init+1] & 0xFF) << 16) | 
                ((byte[init+2] & 0xFF) << 8) | (byte[init+3] & 0xFF));
}

int
main(int argc, char **argv)
{
	struct sockaddr_in localaddr;	/* our address */
	struct sockaddr_in remoteaddr;	/* remote address */
	socklen_t addrlen = sizeof(remoteaddr);		/* length of addresses */
	int recvlen;			/* # bytes received */
	int fd;					/* our socket */
	int msgcnt = 0;			/* count # of messages we received */
	unsigned char buf[BUFLEN];	/* receive buffer */

    char bytes[5];
    unsigned int cod;
    unsigned int iid;
//    conversion dw;


	/* create a UDP socket */

	if ((fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		fprintf(stderr, "Cannot create socket\n");
		return 1;
	}

	/* bind the socket to any valid IP address and a specific port */
	memset((char *)&localaddr, 0, sizeof(localaddr));
	localaddr.sin_family = AF_INET;
	localaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	localaddr.sin_port = htons(PORT);

	if (bind(fd, (struct sockaddr *)&localaddr, sizeof(localaddr)) < 0) {
		fprintf(stderr, "Bind failed\n");
		return 1;
	}

	/* now loop, receiving data and printing what we received */
	for (;;) {
		printf("waiting on port %d\n", PORT);
		recvlen = recvfrom(fd, &bytes, BUFLEN, 0, (struct sockaddr *)&remoteaddr, &addrlen);
		if (recvlen > 0) {
			buf[recvlen] = 0;
            
            cod = bytes[0];
            iid = ntohl(btoi(bytes, 1));
			printf("received message: [%s] [%c][%d] (%d bytes)\n", bytes, cod, iid, recvlen);
		
            switch (cod) {
                case SEND_LEAVE:
                    sendleave(buf);
                    // leave
                    break;
            
                case RECV_LEAVE:
                    recvleave(buf);
                    break

                case SEND_LOOKUP:
                    sendloockup(buf);
                    // lookup
                    break;

                case RECV_LOOKUP:
                    recvlookup(buf);
                    break;

                case SEND_JOIN:
                    sendjoin(buf);
                    // join
                    break;

                case RECV_JOIN:
                    recvjoin(buf);
                    break;

                case SEND_UPDATE:
                    sendupdate(buf);
                    // update
                    break;

                case RECV_UPDATE:
                    recvupdate(buf);
                    break;

            }
           
        } else
            printf("error on message receiving!\n");

//		sprintf(buf, "ack %d", msgcnt++);
//		printf("sending response \"%s\"\n", buf);
//		if (sendto(fd, buf, strlen(buf), 0, (struct sockaddr *)&remoteaddr, addrlen) < 0)
//			perror("sendto");
//	}
	/* never exits */
}

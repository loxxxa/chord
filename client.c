/*
 * client
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <openssl/md5.h>
#include <time.h>
#include "util.h"


struct datagram {
    unsigned char cod;
    unsigned int id;
};


/* transforms an integer to byte array (4 bytes) */
void 
itob(unsigned char *byte, const unsigned int n, const int init)
{
    byte[init] = (n >> 24) & 0xFF;
    byte[init+1] = (n >> 16) & 0xFF;
    byte[init+2] = (n >> 8) & 0xFF;
    byte[init+3] = n & 0xFF;
}

/*
int hashtoint(const char *hash) {
	int v1, v2, v3, v4;
	unsigned int aux;

	sscanf(&hash[0], "%02x", &v1);
	sscanf(&hash[8], "%02x", &v2);
	sscanf(&hash[16], "%02x", &v3);
	sscanf(&hash[24], "%02x", &v4);

	aux = (v1 ^ v2 ^ v3 ^ v4);

	return v4;
}
*/

/*
int concint(int x,int y) {
    int m = 10;

    while (y / m > 0)
        m *= 10;

    return  (y + m * x);
}
*/

char *md5sum(const char *str, int len) {
    unsigned char digest[MD5_DIGEST_LENGTH];
	MD5_CTX mdContext;
	char *aux = malloc(MD5_DIGEST_LENGTH * 2 + 1);
	int i;

    MD5_Init(&mdContext);
	MD5_Update(&mdContext, str, len);
	MD5_Final(digest, &mdContext);

	for(i = 0; i < MD5_DIGEST_LENGTH; i++)
		snprintf(&(aux[i*2]), MD5_DIGEST_LENGTH * 2, "%02x", (unsigned int)digest[i]);

	return aux;
}


int main(int argc, char *argv[])
{
	struct sockaddr_in localaddr;
    struct sockaddr_in remoteaddr;
    socklen_t srvlen = sizeof(remoteaddr);

    struct timeval timeout;      
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

	unsigned char buf[BUFLEN];	/* message buffer */
    char server[255];	/* server ip */
	int recvlen;		/* # bytes in acknowledgement message */
    int fd;
    int slen = sizeof(remoteaddr);

//	char *id;				/* the complete md5 of ip */
//	char shortid[IDLEN+1];	/* first 4 characters of id */
	unsigned int iid;		/* int id */
	unsigned char bytes[5];
//    unsigned int ibuf;

//    struct datagram teste;


	srandom(time(NULL));

    if (argc != 2) {
        fprintf(stderr, "usage: client [serverip]\n");
        return 1;
    }
    strcpy(server, argv[1]);

	/* create a socket */
    fprintf(stderr, "Creating a socket...\n");
	if ((fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		fprintf(stderr, "Cannot create socket\n");
        return 1;
    }

	/* bind it to all local addresses and pick any port number */
	memset((char *)&localaddr, 0, sizeof(localaddr));
	localaddr.sin_family = AF_INET;
	localaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	localaddr.sin_port = htons(0);
	if (bind(fd, (struct sockaddr *)&localaddr, sizeof(localaddr)) < 0) {
		fprintf(stderr, "Bind failed\n");
		return 1;
	}       

	/* now define remaddr, the address to whom we want to send messages */
	/* For convenience, the host address is expressed as a numeric IP address */
	/* that we will convert to a binary format via inet_aton */
	memset((char *) &remoteaddr, 0, sizeof(remoteaddr));
	remoteaddr.sin_family = AF_INET;
	remoteaddr.sin_port = htons(PORT);
	if (inet_aton(server, &remoteaddr.sin_addr) == 0) {
		fprintf(stderr, "inet_aton() failed\n");
		return 1;
	}

	/* calculate md5 sum and generate id */
///	id = (char *) malloc(MD5_DIGEST_LENGTH * 2 + 1);
///	id = md5sum(server, strlen(server));
///	strncpy(shortid, id, IDLEN);
///	shortid[IDLEN] = '\0';	/* terminator character */
//	iid = strtol(shortid, NULL, 16);
	iid = random() % 10000000;

	fprintf(stderr, "Int ID: %d\n", iid);

    bytes[0] = SEND_LEAVE;
    itob(bytes, htonl(iid), 1);

    fprintf(stderr, "Bytes: [%s]\n", bytes);
//    pack(bytes, send_leave, iid);
    
	if (sendto(fd, &bytes, sizeof(bytes), 0, (struct sockaddr *)&remoteaddr, slen) < 0) {
        fprintf(stderr, "Error sending packet\n");
		return 1;
	}
	
    /* now receive an acknowledgement from the server */
    fprintf(stderr, "Waiting for ACK...\n");
    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO,&timeout,sizeof(timeout)) < 0) {
        fprintf(stderr, "Error setting rcvtimeout\n");
        return 1;
    }
	recvlen = recvfrom(fd, buf, BUFLEN, 0, (struct sockaddr *)&remoteaddr, &slen);
    if (recvlen >= 0) {
        buf[recvlen] = 0;	/* expect a printable string - terminate it */
        fprintf(stderr, "received message: \"%s\"\n", buf);
    }
	
	close(fd);
	return 0;
}

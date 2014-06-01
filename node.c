#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

#define PORT    12345

int
main(int argc, char *argv[])
{
    struct sockaddr_in clientaddr;
    struct sockaddr_in serveraddr;
    socklen_t addrlen = sizeof(serveraddr);

    int snd, rcv;
    int fd;


    /* create a UDP socket */
    if ((fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("Cannot create socket\n");
        return 1;
    }

    /* bind the socket to any valid IP address and a specific port */
    memset((char *)&clientaddr, 0, sizeof(clientaddr));
    clientaddr.sin_family = AF_INET;
    clientaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    clientaddr.sin_port = htons(PORT);

    if (bind(fd, (struct sockaddr *)&clientaddr, sizeof(clientaddr)) < 0) {
        perror("Bind failed");
                return 0;
        }

}




/**************************************
udp_ser.c: the source file of the server in udp transmission
**************************************/

#include "headsock.h" // include the header file for UDP transmission

void str_ser4(int sockfd); // transmitting and receiving function

int main(int argc, char *argv[])
{

    int sockfd; // store the values returned by the socket() system call
    struct sockaddr_in my_addr;

    /* 
            struct sockaddr_in {
               sa_family_t    sin_family;  // address family: AF_INET (unsigned integer)
               in_port_t      sin_port;    // port in network byte order (uint16_t)
               struct in_addr sin_addr;    // internet address
               char           sin_zero[8]  
           };
    */

    /* 
            struct in_addr {
               uint32_t       s_addr;      // address in network byte order
           };
    */

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    // int socket (int family, int type, int protocol);
    // address type, network byte order, internet addr
    // specify the type of communication protocol

    if (sockfd == -1)
    {
        printf("error in socket");
        exit(1);
    }

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(MYUDP_PORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    // The htonl() function converts the unsigned integer hostlong from host byte order to network byte order.
    // The htons() function converts the unsigned short integer hostshort from host byte order to network byte order.
    // The ntohl() function converts the unsigned integer netlong from network byte order to host byte order.
    // The ntohs() function converts the unsigned short integer netshort from network byte order to host byte order.

    bzero(&(my_addr.sin_zero), 8);
    // sets all values in the buffer to zero

    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
    {
        printf("error in binding");
        exit(1);
    }
    // bind socket
    // int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

    printf("waiting for data\n");
    while (1)
    {
        str_ser4(sockfd); // send and receive
    }
    close(sockfd); // close a socket and terminate
    exit(0);
}

void str_ser4(int sockfd)
{

    char buf[BUFSIZE];
    FILE *fp;
    char recvs[DATALEN];
    struct ack_so ack;
    int end = 0, n = 0;
    long lseek = 0;
    int stage = 1; //1 for 1DU stage and 2 for 2DU stage

    while (!end)
    {

        if ((n = recv(sockfd, &recvs, DATALEN, 0)) == -1) //receive the packet
        {
            printf("error when receiving\n");
            exit(1);
        }
        // ssize_t recv(int sockfd, void *buf, size_t nbytes, int flags);
        // returns the length of the messages in bytes.

        if (recvs[n - 1] == '\0') // end-of-file
        {
            end = 1;
            n--;
        }
        memcpy((buf + lseek), recvs, n);
        lseek += n;
        // void *memcpy(void *str1, const void *str2, size_t n)
        // copies n characters from memory area str2 to memory area str1

        if (stage == 2)
        {
            ack.num = 1;
            ack.len = 0;
            if ((n = send(sockfd, &ack, 2, 0)) == -1)
            {
                printf("send error!");
                exit(1);
            }
            stage = 1;
        }
        if ((fp = fopen("myTCPrecevive.txt", "wt")) == NULL)
        {
            printf("File doesn't exist\n");
            exit(0);
        }
        fwrite(buf, 1, lseek, fp); // write data into file
        fclose(fp);
        // FILE *fopen(const char *filename, const char *mode)
        // size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
        // int fclose(FILE *stream)
        printf("a file has been successfully received!\nthe total data received is %d bytes\n", (int)lseek);
    }
}
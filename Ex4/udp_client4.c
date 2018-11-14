/*******************************
udp_client.c: the source file of the client in udp transmission
********************************/

#include "headsock.h" // include the header file for UDP transmission

float str_cli4(FILE *fp, int sockfd, long *len); //transmission function

void tv_sub(struct timeval *out, struct timeval *in); //calculate the time interval between out and in

int main(int argc, char **argv)
{
    int sockfd;
    float ti, rt;
    long len;
    struct sockaddr_in ser_addr;
    char **pptr;
    struct hostent *sh;
    struct in_addr **addrs;
    FILE *fp;

    /*
			struct hostent {
			    char  *h_name;            // hostname
			    char **h_aliases;         // alias list
			    int    h_addrtype;        // host address type
			    int    h_length;          // address length
			    char **h_addr_list;       // list of addresses
			}
	*/

    if (argc != 2)
    {
        printf("parameters not match");
    }

    // returns struct of type hostent for given hostname or IPv4
    sh = gethostbyname(argv[1]);
    if (sh == NULL)
    {
        printf("error when gethostby name");
        exit(0);
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0); //create socket
    if (sockfd == -1)
    {
        printf("error in socket");
        exit(1);
    }

    addrs = (struct in_addr **)sh->h_addr_list;
    printf("canonical name: %s\n", sh->h_name);
    for (pptr = sh->h_aliases; *pptr != NULL; pptr++)
        printf("the aliases name is: %s\n", *pptr); // print the remote host's information
    switch (sh->h_addrtype)
    {
    case AF_INET:
        printf("AF_INET\n");
        break;
    default:
        printf("unknown addrtype\n");
        break;
    }

    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(MYUDP_PORT);
    memcpy(&(ser_addr.sin_addr.s_addr), *addrs, sizeof(struct in_addr));
    bzero(&(ser_addr.sin_zero), 8);

    if ((fp = fopen("myfile.txt", "r+t")) == NULL)
    {
        printf("File doesn't exit\n");
        exit(0);
    }

    ti = str_cli4(fp, sockfd, &len); //perform the transmission and receiving
    rt = (len / (float)ti);          //caculate the average transmission rate
    printf("Time(ms) : %.3f, Data sent(byte): %d\nData rate: %f (Kbytes/s)\n", ti, (int)len, rt);

    close(sockfd);
    fclose(fp);
    exit(0);
}

float str_cli4(FILE *fp, int sockfd, long *len)
{
    char *buf;               // buffer char array
    long lsize, ci;          // lsize -> entire size, ci -> current index
    char sends[2 * DATALEN]; // packet to be sent
    struct ack_so ack;       //
    int n, slen;             // 1DU or 2DU
    float time_inv = 0.0;
    struct timeval sendt, recvt;
    ci = 0;
    int stage = 1;

    fseek(fp, 0, SEEK_END);
    // returns current file position of given stream
    // SEEK_SET / SEEK_CUR / SEEK_END
    // int fseek(FILE *stream, long int offset, int whence)

    lsize = ftell(fp); // lsize get's the last position of the file
    rewind(fp);        // sets back the file position to the start
    printf("The file length is %d bytes\n", (int)lsize);
    printf("the packet length is %d bytes\n", DATALEN);

    // allocate memory to contain the whole file.
    buf = (char *)malloc(lsize);
    if (buf == NULL)
        exit(2);

    // copy the file into the buffer.
    fread(buf, 1, lsize, fp);
    // size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)

    /*** the whole file is loaded in the buffer. ***/
    buf[lsize] = '\0';          //append the end byte (extra byte sent to server)
    gettimeofday(&sendt, NULL); //get the current time

    while (ci <= lsize)
    {
        // to track whether to send 1DU or 2*1DU
        stage++;
        if (stage % 2 == 0)
        {
            for (int i = 0; i < 2; i++)
            {
                if ((lsize + 1 - ci) <= DATALEN) // the last part of file that is < 1 or 2 DU
                    slen = lsize + 1 - ci;
                else
                    slen = DATALEN;
            }
        }
        else
        {
            if ((lsize + 1 - ci) <= DATALEN) // the last part of file that is < 1 or 2 DU
                slen = lsize + 1 - ci;
            else
                slen = DATALEN;
        }

        memcpy(sends, (buf + ci), slen);

        n = send(sockfd, &sends, slen, 0);
        if (n == -1)
        {
            printf("send error!"); //send the data
            exit(1);
        }

        if ((n = recv(sockfd, &ack, 2, 0)) == -1) //receive the ack
        {
            printf("error when receiving ack\n");
            exit(1);
        }
        if (ack.num != 1 || ack.len != 0)
        {
            printf("error in transmission\n");
        }
    }
    
    // calculating time taken for transfer
    gettimeofday(&recvt, NULL); //get current time
    *len = ci;
    tv_sub(&recvt, &sendt); // get the whole trans time
    time_inv += (recvt.tv_sec) * 1000.0 + (recvt.tv_usec) / 1000.0;
    free(buf);
    return (time_inv);
}

void tv_sub(struct timeval *out, struct timeval *in)
{
    if ((out->tv_usec -= in->tv_usec) < 0)
    {
        --out->tv_sec;
        out->tv_usec += 1000000;
    }
    out->tv_sec -= in->tv_sec;
}

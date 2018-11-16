// headfile for UDP program
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>

#define NEWFILE (O_WRONLY|O_CREAT|O_TRUNC)
#define MYTCP_PORT 4950
#define MYUDP_PORT 5350          // UDP port #
#define DATALEN 400             // packet data length
#define BUFSIZE 60000            // buffer size
#define PACKLEN 508              // data unit length
#define HEADLEN 8                // header length    

struct pack_so			//data packet structure
{
uint32_t num;		    // the sequence number
uint32_t len;		    // the packet length
char data[DATALEN];	    // the packet data
};

struct ack_so
{
uint8_t num;
uint8_t len;
};
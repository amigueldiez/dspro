/*
 * Alberto Miguel Diez - amigud00[at]estudiantes.unileon.es 
 *
 * Parts of this source code were taken from the
 * W. Richard Stevens' book on Unix Network Programming
 * (Prentice-Hall 1998)
 *
 * Refactorization, additional comments and adaptation
 * for the purposes of the CN Lab by José María Foces Moran 2014
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/param.h>

#include <sys/time.h>
#include <sys/file.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include <netinet/in_systm.h>
#include <netinet/in.h>

#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#include <netdb.h>
#include <unistd.h>

#include <errno.h>

#include <time.h>

#define TRUE 1

/*
 * #Bytes of data, following ICMP header (Stevens' UNIX Net Programming book)
 * Data that goes with ICMP echo request
 */
int datalen = 12;

struct sockaddr targetHost; //generic sock address for the target host
int rawSocket;

int nsent = 0;
int pid;
time_t minRttN;
int first = 1;
int numberRequest=1;
time_t sumOfDelta = 0;


#define MAXIPHEADERLENGTH 60
#define MAXICMPPAYLOADLENGTH 76
#define MAXIPPACKETSIZE (65536 - 60 - 8)
#define MAGICSEQN 512

struct timeval originateTimeval, receiveTimeval;
long tsorig, tsrecv, tstrans; //correct declaration

long tsdiff; //correct declaration


int initSocket(char *targetHostIpDDN) {
    
    int rawSock;
    struct sockaddr_in *inetTargetHost; //inet socket address for target host
    struct protoent *protocol;
    
    bzero((char *) &targetHost, sizeof (struct sockaddr));
    inetTargetHost = (struct sockaddr_in *) &targetHost;
    inetTargetHost->sin_family = AF_INET;
    inet_aton(targetHostIpDDN, &(inetTargetHost->sin_addr));
    
    protocol = getprotobyname("icmp");
    
    rawSock = socket(AF_INET, SOCK_RAW, protocol->p_proto);
    
    return rawSock;
    
} //end of initSock()

unsigned char *createPacket(int *packlen) {
    
    unsigned char *packet;
    
    *packlen = datalen + MAXIPHEADERLENGTH + MAXICMPPAYLOADLENGTH;
    
    packet = (unsigned char *) malloc((unsigned int) *packlen);
    
    return packet;
    
}//end of createPacket()

/*
 * The source code of function internetChecksum are original from
 * W. Richard Stevens' book on Unix Network Programming
 * (Prentice-Hall 1998) in its entirety, no modification has been
 * carried out by JMFoces whatsoever except for the function name
 */
unsigned short internetChecksum(u_short *addr, int len) {
    int nleft = len;
    int sum = 0;
    unsigned short *w = addr;
    unsigned short answer = 0;
    
    while (nleft > 1) {
        sum += *w++;
        nleft -= 2;
    }
    
    if (nleft == 1) {
        *(unsigned char *) (&answer) = *(unsigned char *) w;
        sum += answer;
    }
    
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;
    return (answer);
    
}

void sendRequest() {
    int len;
    struct icmp *icmp;
    unsigned char requestPacket[MAXIPPACKETSIZE];
    
    icmp = (struct icmp *) requestPacket;
    bzero((void *)icmp, MAXIPPACKETSIZE);
    icmp->icmp_type = ICMP_TSTAMP;
    icmp->icmp_code = 0;
    icmp->icmp_seq = MAGICSEQN;
    icmp->icmp_id = pid;
    
    gettimeofday(&originateTimeval, NULL);
    tsorig = (originateTimeval.tv_sec % (24 * 3600)) * 1000 + originateTimeval.tv_usec / 1000;
    icmp->icmp_otime = htonl(tsorig);
    icmp->icmp_rtime = 0;
    icmp->icmp_ttime = 0;
    
    len = datalen + 8;
    
    icmp->icmp_cksum = internetChecksum((u_short *) icmp, len);
    
    sendto(rawSocket, (char *) requestPacket, len, 0, &targetHost, sizeof (struct sockaddr));
}

// Prints the timestamp of the computer
void print_timestamp() {
    struct timeval tv;
    time_t t;
    struct tm *info;
    char buffer[64];
 
    gettimeofday(&tv, NULL);
     t = tv.tv_sec;

     info = localtime(&t);
    printf("%s",asctime (info));
}

int processPacket(char *buf, int n, struct sockaddr_in *from) {
    int headerLength;
    struct icmp *icmp;
    struct ip *ip;
    struct timeval delta;
    
    //Record real time for accurate Rtt measurement:
    // ---> BEWARE receiveTimeval.tv_usec / 1000. Shouldn't it read % instead of / ???
    gettimeofday(&receiveTimeval, NULL);
    
    time_t timeRecReply  = (receiveTimeval.tv_sec % (24 * 3600)) * 1000 + receiveTimeval.tv_usec / 1000;
    
    //Compute IP header length
    ip = (struct ip *) buf;
    headerLength = ip->ip_hl << 2;
    
    //Subtract header length from n
    n -= headerLength;
    icmp = (struct icmp *) (buf + headerLength);
    
    /*
     * Discard all ICMP packets which ICMP type is not
     * RFC 792 type value 14 for timestamp reply message represented by
     * ICMP_TSTAMPREPLY constant
     */
    if (icmp->icmp_type == ICMP_TSTAMPREPLY) {
        
        if (icmp->icmp_seq != MAGICSEQN)
            printf("Spurious sequence  received %d\n", icmp->icmp_seq);
        if (icmp->icmp_id != getpid())
            printf("Spurious id received %d\n", icmp->icmp_id);
        
        //Receive timestamp
        tsrecv  = ntohl(icmp->icmp_rtime);
        
        //Transmit timestamp
        tstrans = ntohl(icmp->icmp_ttime);
        
        // Rtt/2: backDelay
        time_t Rtt = timeRecReply - tsorig;
        time_t irqTime = tstrans - tsrecv;
        time_t backDelay = (Rtt - irqTime)/2;
        time_t diff = (tstrans + backDelay) - timeRecReply;

        // Compute RttN
        time_t RttN = Rtt - irqTime;
        
        //Difference between Receive timestamp and originate timestamp:
        //tsdiff = tsrecv - tsorig; // ms
        
        printf("\t· delta = %ld\n", diff);
        
        // Check / vs. %, see above within this function:
        delta.tv_sec = diff / 1000;
        delta.tv_usec = ((diff) % 1000) * 1000;
        
        //time_t is a signed long integer !
        //beware next printf(): %u ??? Should be %d?
        //printf("Correction = %d sec, %d usec\n", delta.tv_sec, delta.tv_usec);
        fflush(stdout);
        
        
        // Computation of mean delta and std deviation
        sumOfDelta = sumOfDelta + diff;
        printf("Mean delta: %f\n", sumOfDelta/(float)numberRequest);
        printf("RttN in this request: %d\n", (int)RttN);

        if (first == 1){
            first = 0;
            minRttN = RttN;
        }

        if(RttN <= minRttN){
            printf("The minimun Rtt is: %d\n", (int)minRttN);
            printf("Updating time using adjtime()\n");
            minRttN = RttN;
            printf("Local time before invoking adjtime: ");
            print_timestamp();
            adjtime(&delta, (struct timeval *) 0);
            printf("Local time after invoking adjtime: ");
            print_timestamp();
        }
        
        return (0); //Timestamp reply
    } else
        return (-1); //Not timestamp reply
}

void receiveResponse(int packetLength, unsigned char *packet) {
    struct sockaddr_in from;
    int nbytes;
    int fromlen;
    
    while (TRUE) {
        fromlen = sizeof (from);
        nbytes = recvfrom(rawSocket, (char *) packet, packetLength, 0,(struct sockaddr *) &from, &fromlen);


        if (nbytes < 0) {
            printf("Bytes received < 0");
            fflush(stdout);
            
            if (errno == EINTR)
                continue;
            else
                perror("recvfrom error");
        }
        
        if (processPacket((char *) packet, nbytes, &from) == 0)
            break;
    }
    
    
}

int main() {
    
    unsigned char *packet;
    int packetLength;
    
    pid = getpid();
    
    rawSocket = initSocket("193.146.101.46");
    
    packet = createPacket(&packetLength);
    
    while(1){
        printf("Request number %d...\n", numberRequest);
        sendRequest();
        receiveResponse(packetLength, packet);
        sleep(1);
        numberRequest = numberRequest + 1;
        printf("\n");
    }
    
    
} //end of main()



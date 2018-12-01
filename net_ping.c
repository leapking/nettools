#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/ip_icmp.h>

#define ICMP_PACKET_SIZE 16
#define TIMEOUT_SECONDS 2
uint16_t ident=0;

const char *ping_desc(uint8_t type, uint8_t code)
{
    switch(type)
    {
        case ICMP_ECHOREPLY:
            return "Echo Reply";
        case ICMP_ECHO:
            return "Echo Request";
        case ICMP_PARAMPROB:
            return "Bad Parameter";
        case ICMP_SOURCEQUENCH:
            return "Packet lost, slow down";
        case ICMP_TSTAMP:
            return "Timestamp Request";
        case ICMP_TSTAMPREPLY:
            return "Timestamp Reply";
        case ICMP_IREQ:
            return "Information Request";
        case ICMP_IREQREPLY:
            return "Information Reply";
        case ICMP_UNREACH:
            switch(code)
            {
                case ICMP_UNREACH_NET:
                    return "Unreachable Network";
                case ICMP_UNREACH_HOST:
                    return "Unreachable Host";
                case ICMP_UNREACH_PROTOCOL:
                    return "Unreachable Protocol";
                case ICMP_UNREACH_PORT:
                    return "Unreachable Port";
                case ICMP_UNREACH_NEEDFRAG:
                    return "Unreachable: Fragmentation needed";
                case ICMP_UNREACH_SRCFAIL:
                    return "Unreachable Source Route";
                case ICMP_UNREACH_NET_UNKNOWN:
                    return "Unknown Network";
                case ICMP_UNREACH_HOST_UNKNOWN:
                    return "Unknown Host";
                case ICMP_UNREACH_ISOLATED:
                    return "Unreachable: Isolated";
                case ICMP_UNREACH_NET_PROHIB:
                    return "Prohibited network";
                case ICMP_UNREACH_HOST_PROHIB:
                    return "Prohibited host";
                case ICMP_UNREACH_FILTER_PROHIB:
                    return "Unreachable: Prohibited filter";
                case ICMP_UNREACH_TOSNET:
                    return "Unreachable: Type of Service and Network";
                case ICMP_UNREACH_TOSHOST:
                    return "Unreachable: Type of Service and Host";
                case ICMP_UNREACH_HOST_PRECEDENCE:
                    return "Unreachable: Prec vio";
                case ICMP_UNREACH_PRECEDENCE_CUTOFF:
                    return "Unreachable: Prec cutoff";
                default:
                    return "Unreachable: Unknown Subtype";
            }
            break;
        case ICMP_REDIRECT:
            switch(code)
            {
                case ICMP_REDIRECT_NET:
                    return "Redirect: Network";
                case ICMP_REDIRECT_HOST:
                    return "Redirect: Host";
                case ICMP_REDIRECT_TOSNET:
                    return "Redirect: Type of Service and Network";
                case ICMP_REDIRECT_TOSHOST:
                    return "Redirect: Type of Service and Host";
                default:
                    return "Redirect: Unknown Subtype";
            }
        case ICMP_TIMXCEED:
            switch(code)
            {
                case ICMP_TIMXCEED_INTRANS:
                    return "Timeout: TTL";
                case ICMP_TIMXCEED_REASS:
                    return "Timeout: Fragmentation reassembly";
                default:
                    return "Timeout: Unknown Subtype";
            }
            break;
        default:
            return "Unknown type";
    }
}

uint16_t chksum(unsigned short *buf, int len)
{
    uint32_t sum = 0;

    while(len > 1) {
        sum += *buf;
        buf++;
        len -= 2;
    }
    if(len)
        sum += (*(uint8_t *)buf);

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum = (sum >> 16) + sum;

    return ~sum;
}

int ping_write(char *buf, int seq)
{
    struct timeval tv;
    struct icmp *icmp = (struct icmp *)buf;

    icmp->icmp_type = ICMP_ECHO;
    icmp->icmp_code = 0;
    icmp->icmp_id = ident;
    icmp->icmp_seq = seq;
    icmp->icmp_cksum = 0;

    gettimeofday(&tv, NULL);
    memcpy(buf + 8, &tv, sizeof(tv));

    icmp->icmp_cksum = chksum((unsigned short *)icmp, ICMP_PACKET_SIZE);

//    printf("send ICMP packet(type:%d, code:%d, id:%d, seq:%d, checksum:%d\n",
//           icmp->icmp_type, icmp->icmp_code, icmp->icmp_id, icmp->icmp_seq, icmp->icmp_cksum);
}

int ping_read(void *buf, struct sockaddr_in answer)
{
    uint16_t chk_sum;
    struct timeval tv_send, tv_now;
    unsigned int mini_sec;
    struct ip *ip = (struct ip *)buf;
    struct icmp *icmp = (struct icmp *)(buf + (ip->ip_hl << 2));

//    printf("recv ICMP packet(type:%d, code:%d, id:%d, seq:%d, checksum:%d\n",
//           icmp->icmp_type, icmp->icmp_code, icmp->icmp_id, icmp->icmp_seq, icmp->icmp_cksum);

    if(icmp->icmp_id != ident) {
//        printf("ERROR: bad icmp_id %d\n", icmp->icmp_id);
        return -1;
    }

    if(icmp->icmp_type != ICMP_ECHOREPLY) {
        printf("ERROR: %s\n", ping_desc(icmp->icmp_type, icmp->icmp_code));
        return -1;
    }

    chk_sum = icmp->icmp_cksum;
    icmp->icmp_cksum = 0;
    if(chk_sum != chksum((unsigned short *)icmp, ICMP_PACKET_SIZE)) {
        printf("ERROR: bad chksum\n");
        return -1;
    }

    gettimeofday(&tv_now, NULL);
    memcpy(&tv_send, ((char *)icmp + 8), sizeof(tv_send));
    mini_sec = (tv_now.tv_sec - tv_send.tv_sec) * 1000000 + (tv_now.tv_usec - tv_send.tv_usec);

    printf("%d bytes from %s: icmp_seq=%d, ttl=%d, time=%.3f ms\n",
           (ip->ip_hl << 2) + 16,
           inet_ntoa(answer.sin_addr),
           icmp->icmp_seq, ip->ip_ttl, mini_sec/1000.0);
    return 0;
}

int main(int argc, char *argv[])
{
    struct hostent *host;
    struct sockaddr_in dest;
    struct sockaddr_in answer;
    int  answer_len = sizeof(answer);
    int  sock_raw_fd, seq=0;
    char send_buf[ICMP_PACKET_SIZE];
    char recv_buf[50];

    if(argc != 2) {
        printf("Usage: %s ipaddr/hostname\n", argv[0]);
        exit(1);    
    }

    if ((host = gethostbyname(argv[1])) == NULL) {
        printf("ping: unknow host %s \n", argv[1]);
        exit(1);
    }

    ident = getpid() & 0xFFFF;
    bzero(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
    memcpy(&dest.sin_addr, host->h_addr, sizeof(int));
    dest.sin_port = ntohs(0);

    if ((sock_raw_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1) {
        perror("socket");
        exit(1);
    }    

    while(seq < 5) {
        seq++;
//        printf("\nid:%d\n", seq);
        ping_write(send_buf, seq);
        if (sendto(sock_raw_fd, (const char*)send_buf, ICMP_PACKET_SIZE, 0, (struct sockaddr *)&dest, sizeof(dest)) == -1) {
            perror("sendto");
            exit(1);
        }

        while(1) {
            int ret = 0;
            struct timeval tv;
            fd_set readset;
            FD_ZERO(&readset);
            FD_SET(sock_raw_fd, &readset);

            tv.tv_sec = TIMEOUT_SECONDS;
            tv.tv_usec = 0;

            if ((ret = select(sock_raw_fd+1, &readset, NULL, NULL, &tv)) == -1) {
                perror("select");
                exit(1);
            } 
            if(ret == 0) {
                printf("time out.\n");
                break;
            } else {
                if(recvfrom(sock_raw_fd, recv_buf, 36, 0, (struct sockaddr *)&answer, &answer_len) == -1) {
                    perror("select");
                    exit(1);
                } else { 
                    if (ping_read(recv_buf, answer) == 0)
                        break;
                }
            }
        }
        sleep(1);
    }
    return 0;
}

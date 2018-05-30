#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

void tst_gethostbyaddr(struct sockaddr_in *who)
{
    struct hostent *hp;
    const char *host;

    hp = gethostbyaddr((char *)&who->sin_addr, sizeof (struct in_addr), who->sin_family);
    if(hp)
        host = hp->h_name;
    else
        host = inet_ntoa(who->sin_addr);

    printf("gethostbyaddr host(%s)\n", host);
}

void tst_getnameinfo(struct sockaddr_in *ip)
{
    struct sockaddr *who=(struct sockaddr *)ip;
    int error=-1;
    char namebuf[255];

    error = getnameinfo(who, sizeof(*ip), namebuf, sizeof(namebuf), NULL, 0, 0);
    if(error)
    {
        perror("getnameinfo: localhost");
        perror(gai_strerror(error));
        printf("error=%d\n", error);
        exit(1);
    }
    printf("getnameinfo namebuf(%s)\n", namebuf);
}

int main(int argc,char **argv)
{
    /*get name of connected client*/
    struct sockaddr_in anip;

    if(argc!=2)
    {
        fprintf(stderr,"usage:%s <ip_address>\n",argv[0]);
        exit(1);
    }
    anip.sin_addr.s_addr = inet_addr(argv[1]);
    anip.sin_family = AF_INET;

    tst_gethostbyaddr(&anip);
    tst_getnameinfo(&anip);
}

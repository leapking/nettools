#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define DEFAULT_IP   "127.0.0.1"
#define DEFAULT_PORT 8000

int main(int argc, char *argv[])
{
	int sockfd = 0;
	unsigned short server_port = DEFAULT_PORT;
	char *server_ip = DEFAULT_IP;
	struct sockaddr_in server_addr;
	pid_t pid = getpid();

	if(argc > 1)
	{
		server_ip = argv[1];
	}

	if(argc > 2)
	{
		server_port = atoi(argv[2]);
	}

	/* 1. create socket */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket ");
		exit(-1);
	}

	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;			//set IPV4
	server_addr.sin_port = htons(server_port);		//set port
	inet_pton(AF_INET, server_ip, &server_addr.sin_addr);	//set ip

	/* 2. connect to server */
	if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0)
	{
		perror("connect");
		close(sockfd);
		exit(-1);
	}
	printf("Client[%d]: connect to server:%s, port:%d success\n", pid, server_ip, server_port);

	while(0)
	{
		char send_buf[512] = " ";
		char recv_buf[512] = " ";

		/* get data */
		printf("\nsend data to %s:%d\n", server_ip, server_port);
		//fgets(send_buf, sizeof(send_buf), stdin);
		//send_buf[strlen(send_buf)-1] = 0;
		strcpy(send_buf, "hello");

		/* send data */
		printf("Client[%d]: send:%s\n", pid, send_buf);
		if (send(sockfd, send_buf, strlen(send_buf), 0) <= 0)
		{
			printf("Client[%d]: send failed\n", pid);
			goto Ret;
		}

		/* recv data */
		if (recv(sockfd, recv_buf, sizeof(recv_buf), 0) <= 0)
		{
			printf("Client[%d]: recv failed\n", pid);
			goto Ret;
		}
		printf("Client[%d]: recv:%s\n", pid, recv_buf);
	}

	sleep(1000);

Ret:
	close(sockfd);
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define DEFAULT_IP   "127.0.0.1"
#define DEFAULT_PORT 8000
#define BACKLOG      10
#define MAX_CONN     3000

typedef struct _Connection
{
	pthread_t          thread;
	struct sockaddr_in addr;
	socklen_t          len;
	char               ip[INET_ADDRSTRLEN];
	int                fd;
	long               id;
} Connection;

pthread_mutex_t Mutex;

pid_t gettid(void)
{
    return syscall(__NR_gettid);
}

void *
ThreadMain(void *arg)
{
	Connection *client = (Connection *)arg;
	char recv_buf[2048] = " ";
	pid_t tid = gettid();

	inet_ntop(AF_INET, &client->addr.sin_addr, client->ip, INET_ADDRSTRLEN);
	printf("Server[%ld]: receive connection from %s\n", client->id, client->ip);

	if (pthread_mutex_lock(&Mutex) != 0)
	{
		printf("mutex lock failed\n");
		return NULL;
	}

	printf("Server[%ld]: locking\n", client->id);
	sleep(3);

/*
	while(1)
	{
		recv(client->fd, recv_buf, sizeof(recv_buf), 0);
		//printf("Server[%d]: recv:%s\n", tid, recv_buf);

		//printf("Server[%d]: send:ok\n", tid);
		send(client->fd, "ok", 2, 0);
	}
*/

	pthread_mutex_unlock(&Mutex);
	printf("Server[%ld]: unlock\n", client->id);
}

int main(int argc, char *argv[])
{
	int sockfd, i = 0;
	unsigned short port = DEFAULT_PORT;
	struct sockaddr_in my_addr;
	Connection *client = NULL, *conn[MAX_CONN] = {0};

	printf("Max connection: %ld\n", MAX_CONN);
	printf("BACKLOG: %d\n\n", BACKLOG);

	if (pthread_mutex_init(&Mutex, NULL) != 0)
	{
		printf("mutex init failed\n");
		return -1;
	}

	if(argc > 1)
	{
		port = atoi(argv[1]);
	}

	/* 1. create socket */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("create socket");
		exit(-1);
	}
	printf("Server: create socket OK!\n");

	bzero(&my_addr, sizeof(my_addr));
	my_addr.sin_family = AF_INET;			//use IPv4
	my_addr.sin_port = htons(port);			//set port
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);	//accept msg from any address

	/* 2. bind address to socket */
	if (bind(sockfd, (struct sockaddr*)&my_addr, sizeof(my_addr)) != 0)
	{
		perror("bind address to socket");
		close(sockfd);
		exit(-1);
	}
	printf("Server: bind OK!\n");

	/* 3. listen on socket */
	if (listen(sockfd, BACKLOG) != 0)
	{
		perror("listen on socket");
		close(sockfd);
		exit(-1);
	}
	printf("Server: listen on port: %d OK!\n\n", port);

	/* 4. wait for connection */
	while(1)
	{
		client = (Connection *)malloc(sizeof(Connection));
		memset(client, 0, sizeof(Connection));
		client->len = sizeof(struct sockaddr_in);
		client->id = i;

		if ((client->fd = accept(sockfd, (struct sockaddr*)&client->addr, &client->len)) < 0)
		{
			perror("accept");
			free(client);
			return -1;
		}
		if (i >= MAX_CONN)
		{
			printf("too many connection, max conn is %d\n", MAX_CONN);
			close(client->fd);
			free(client);
			continue;
		}
		conn[i++] = client;

		if (pthread_create(&client->thread, NULL, ThreadMain, (void *)client) != 0)
		{
			printf("pthread_create error \n");
			return -1;
		}
	}

	/* 5. wait child thread to exit */
	for(i = 0; i < MAX_CONN; i++)
	{
		client = conn[i];
		if (client == NULL)
		{
			continue;
		}

		pthread_join(client->thread, NULL);
		close(client->fd);
		free(client);
	}

	/* 6. close socket */
	close(sockfd);

	pthread_mutex_destroy(&Mutex);
	return 0;
}

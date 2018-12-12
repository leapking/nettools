#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/syscall.h>
#include <sys/wait.h>

static inline pid_t gettid()
{
	return syscall(SYS_gettid);
}

/* guess the offset of tid in pthread_t */
static int get_tid_offset()
{
	size_t i;
	pid_t *thaddr, tid = gettid();
	pthread_t self = pthread_self();

	if (self == 0)
		return -1;

	/* guess which offset value is equal tid */
	thaddr = (pid_t*)self;
	for (i = 0; i < 64; ++i)
	{
		if (thaddr[i] == tid)
			return i * sizeof(pid_t);
	}
	return -1;
}

/* convert pthread_t to tid */
static int pthread_getunique_np(pthread_t thread, pid_t* id)
{
	int tid_offset = get_tid_offset();
	if (tid_offset < 0)
		return ESRCH;

	*id = *(pid_t*)((char*)(thread) + tid_offset);
	return 0;
}

int wykill(pthread_t thread, int sig)
{
	pid_t tid;

	if (pthread_getunique_np(thread, &tid) != ESRCH)
	{
		int status = 0;
		char cmd[100]={0};
		pid_t pid = getpid();
		printf("thread:%lu ->tid: %d\n", thread, tid);
	
		sprintf(cmd, "pstree -p %d | grep -c %u &>/dev/null", pid, tid);
		status = system(cmd);
		if(WIFEXITED(status))  //system normal termination
		{
			printf("normal termination, exit status = %d\n", WEXITSTATUS(status));
			if (WEXITSTATUS(status) == 1)
				return ESRCH;
		}
	}
	return pthread_kill(thread, sig);
}

void*
ThreadMain()
{
	printf("sub process id:%d\n", gettid());
	sleep(1000000);
}

int
main()
{
	int ret;
	pthread_t  thread;

	if (pthread_create(&thread, NULL, ThreadMain, NULL) != 0)
	{
		printf("pthread_create error \n");
		return -1;
	}

	ret = wykill(thread, 0);
	if(ret == ESRCH)
		printf("线程(%lu)不存在或者已经退出。\n", (unsigned int)thread);
	else if(ret == EINVAL)
		printf("发送信号非法。/n");
	else
		printf("线程(%lu)目前仍然存活。\n", (unsigned int)thread);

	sleep(1000000);
}

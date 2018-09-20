#include <stdio.h>
#include <malloc.h>
#include <unistd.h>

#define NUM_CHUNKS 1001
#define CHUNCK_SIZE 8192

typedef struct {
    unsigned long size, resident, shared, text, lib, data, dt;
}statm_t;

void ShowMemoryStat(pid_t pid)
{
	FILE *fp = NULL;
	char fname[128] = {0};
	statm_t statm = {0,0,0,0,0,0,0};
	int psize = getpagesize()/1000;

	sprintf(fname, "/proc/%d/statm", pid);
	fp = fopen(fname, "r");
	fscanf(fp, "%lu %lu %lu %lu %lu %lu %lu",
		&statm.size, &statm.resident, &statm.shared, &statm.text, &statm.lib, &statm.data, &statm.dt);
	fclose(fp);

	printf("Process %d Memory Use:\n", pid);
	printf("Size\tResident\tShared\tText\tLib\tData\tData+Stack\n");
	printf("------------------------------------------------------------------\n");
	printf("%lu\t%lu\t\t%lu\t%lu\t%lu\t%lu\t%lu\n",
		statm.size*psize, statm.resident*psize, statm.shared*psize, statm.text*psize, statm.lib*psize, statm.data*psize, statm.dt*psize);
}

int main()
{
	int i;
	pid_t pid = getpid();
	void *array[NUM_CHUNKS];

	/* disable fast bins */
	mallopt(M_MXFAST, 0);

        /* allocating memory */
        for (i = 0; i < NUM_CHUNKS; i++)
        {
                array[i] = malloc(CHUNCK_SIZE);
        }

        /* releasing memory ALMOST all memory */
        for (i = 0; i < NUM_CHUNKS - 1; i++)
        {
                free(array[i]);
        }

	malloc_stats();
	printf("-----------------------------\n");
	ShowMemoryStat(pid);

	/* when enabled memory consumption reduces */
	printf("\n*****>>> call malloc_trim <<<*****\n\n");
	malloc_trim(100);

	malloc_stats();
	printf("-----------------------------\n");
	ShowMemoryStat(pid);

	printf("\nSee memory size use: top -p %d\n", pid);
	sleep(1000);
}

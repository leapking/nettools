/*
 * Purpose: this file is use to test: if 'malloc_trim(0)' can give back the released memory to system.
 * Author:  leapking
 */

#include <stdio.h>
#include <malloc.h>
#include <unistd.h>

#define NUM_CHUNKS 1000
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

	malloc_stats();
	printf("-----------------------------\n");
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

        /* allocating memory */
        for (i = 0; i < NUM_CHUNKS; i++)
                array[i] = malloc(CHUNCK_SIZE);

        /* releasing all memory except the last one */
        for (i = 0; i < NUM_CHUNKS - 1; i++)
                free(array[i]);

	ShowMemoryStat(pid);

	/* release memory to the system */
	printf("\n*****>>> call malloc_trim <<<*****\n");
	if (malloc_trim(0))
		printf("release memory to the system success\n\n");
	else
		printf("release memory to the system failed\n\n");

	ShowMemoryStat(pid);

	printf("\nsee: Resident\n");
}

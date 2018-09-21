/* gcc -g -Wl,-wrap,malloc -Wl,-wrap,calloc -Wl,-wrap,realloc -Wl,-wrap,free mem_trace.c */

#include "stdio.h"
#include "stdlib.h"

#define ARRYSIZE 50
#define GetMemRealSize(ptr) *(size_t *)((char *)(ptr)-sizeof(size_t))
long MemoryCount = 0;

void * __real_malloc(size_t size);
void * __real_calloc(size_t n, size_t size);
void * __real_realloc(void *ptr, size_t size);

void * __wrap_malloc(size_t size)
{
	void *ptr = __real_malloc(size);
	size_t realsize = GetMemRealSize(ptr);
	MemoryCount += realsize;
	printf("malloc %d/%d, count %d\n", size, realsize, MemoryCount);
	return ptr;
}

void * __wrap_calloc(size_t n, size_t size)
{
	void *ptr = __real_calloc(n, size);
	size_t realsize = GetMemRealSize(ptr);
	MemoryCount += realsize;
	printf("calloc %d/%d, count %d\n", n*size, realsize, MemoryCount);
	return ptr;
}

void * __wrap_realloc(void *ptr, size_t size)
{
	size_t oldsize, newsize;

	oldsize = GetMemRealSize(ptr);
	MemoryCount -= oldsize;

	ptr = __real_realloc(ptr, size);

	newsize = GetMemRealSize(ptr);
	MemoryCount += newsize;

	printf("realloc %d->%d, count %d\n", oldsize, newsize, MemoryCount);
	return ptr;
}

void __wrap_free(void *ptr)
{
	size_t size = GetMemRealSize(ptr);

	MemoryCount -= size;
	printf("free %d, count %d\n", size, MemoryCount);
	__real_free(ptr);
}

int
main()
{
	int i;
	char *arry[ARRYSIZE] = {0};

	printf("\nBegin malloc-------------------------------\n");
	for (i = 0; i < ARRYSIZE; i++)
	{
		arry[i] = malloc(i+1);
	}

	printf("\nBegin free-------------------------------\n");
	for (i = 0; i < ARRYSIZE; i++)
	{
		free(arry[i]);
	}

	printf("\nBegin calloc-------------------------------\n");
	for (i = 0; i < ARRYSIZE; i++)
	{
		arry[i] = calloc(1, i+1);
	}

	printf("\nBegin realloc-------------------------------\n");
	for (i = 0; i < ARRYSIZE; i++)
	{
		arry[i] = realloc(arry[i], (i+1)*2);
	}

	printf("\nBegin free-------------------------------\n");
	for (i = 0; i < ARRYSIZE; i++)
	{
		free(arry[i]);
	}

	if (MemoryCount)
		fprintf(stderr, "WARING: memory leak size %lu\n", MemoryCount);
	else
		fprintf(stdout, "no memory leak occured.\n");
}

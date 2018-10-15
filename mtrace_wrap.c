/*
 * Purpose: this file is use to wrap methord for tracing malloc,calloc,realloc and free. NOTE: wrap is thread safety.
 * Author:  leapking
 * Build:   gcc -g -Wl,-wrap,malloc -Wl,-wrap,calloc -Wl,-wrap,realloc -Wl,-wrap,free mem_trace_wrap.c
 * Reference:
 *      https://blog.csdn.net/wz1226864411/article/details/77934941
 *      https://blog.csdn.net/a10615/article/details/41063063
 */

#include "stdio.h"
#include "stdlib.h"

typedef struct malloc_chunk* mchunkptr;
#define SIZE_SZ sizeof(size_t)
#define mem2chunk(mem) ((mchunkptr)((char*)(mem) - 2*SIZE_SZ))
#define PREV_INUSE 0x1
#define IS_MMAPPED 0x2
#define NON_MAIN_ARENA 0x4
#define SIZE_BITS (PREV_INUSE | IS_MMAPPED | NON_MAIN_ARENA)
#define chunk_is_mmapped(p) ((p)->mchunk_size & IS_MMAPPED)
#define chunksize_nomask(p)         ((p)->mchunk_size)
#define chunksize(p) (chunksize_nomask (p) & ~(SIZE_BITS))
#define inuse(p) ((((mchunkptr) (((char *) (p)) + chunksize (p)))->mchunk_size) & PREV_INUSE)

struct malloc_chunk {

  size_t      mchunk_prev_size;  /* Size of previous chunk (if free).  */
  size_t      mchunk_size;       /* Size in bytes, including overhead. */

  struct malloc_chunk* fd;       /* double links -- used only if free. */
  struct malloc_chunk* bk;

  /* Only used for large blocks: pointer to next larger size.  */
  struct malloc_chunk* fd_nextsize; /* double links -- used only if free. */
  struct malloc_chunk* bk_nextsize;
};

static size_t
musable (void *mem)
{
        mchunkptr p;
        if (mem == 0)
                return 0;

        p = mem2chunk (mem);

        if (chunk_is_mmapped (p))
                return chunksize (p) - 2 * SIZE_SZ;
        else if (inuse (p))
                return chunksize (p) - SIZE_SZ;
}

#define ARRYSIZE 150
//#define GetMemRealSize(ptr) *(size_t *)((char *)(ptr)-sizeof(size_t))
#define GetMemRealSize(ptr) musable(ptr)
long MemoryCount = 0;

void * __real_malloc(size_t size);
void * __real_calloc(size_t n, size_t size);
void * __real_realloc(void *ptr, size_t size);

void * __wrap_malloc(size_t size)
{
        void *ptr = __real_malloc(size);
        size_t realsize = GetMemRealSize(ptr);

        printf("malloc:%d, real:%d, more:%d, count:%d->%d\n", size, realsize, realsize-size, MemoryCount, MemoryCount+realsize);
        MemoryCount += realsize;
        return ptr;
}

void * __wrap_calloc(size_t n, size_t size)
{
        void *ptr = __real_calloc(n, size);
        size_t realsize = GetMemRealSize(ptr);

        printf("calloc:%d, real:%d, more:%d, count:%d->%d\n", n*size, realsize, realsize-size, MemoryCount, MemoryCount+realsize);
        MemoryCount += realsize;
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

        printf("realloc:%d->%d, more:%d, count:%d->%d\n", oldsize, newsize, newsize-size, MemoryCount-newsize+oldsize, MemoryCount);
        return ptr;
}

void __wrap_free(void *ptr)
{
        size_t size = GetMemRealSize(ptr);
        __real_free(ptr);

        MemoryCount -= size;
        printf("free:%d, count:%d->%d\n", size, MemoryCount+size, MemoryCount);
}

int
main()
{
        int i;
        char *arry[ARRYSIZE+1] = {0};

        printf("\n>>>Begin malloc-------------------------------\n");
        for (i = 1; i <= ARRYSIZE; i++)
        {
                arry[i] = malloc(i * 1024);
        }

        printf("\n>>>Begin free-------------------------------\n");
        for (i = 1; i <= ARRYSIZE; i++)
        {
                free(arry[i]);
        }

        printf("\n>>>Begin calloc-------------------------------\n");
        for (i = 1; i <= ARRYSIZE; i++)
        {
                arry[i] = calloc(1, i * 1024);
        }

        printf("\n>>>Begin realloc-------------------------------\n");
        for (i = 1; i <= ARRYSIZE; i++)
        {
                arry[i] = realloc(arry[i], (i+1) * 1024);
        }

        printf("\n>>>Begin free-------------------------------\n");
        for (i = 1; i <= ARRYSIZE; i++)
        {
                free(arry[i]);
        }

        if (MemoryCount)
                fprintf(stderr, "\nWARING: memory leak size %lu\n", MemoryCount);
        else
                fprintf(stdout, "\nno memory leak occured.\n");
}


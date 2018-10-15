/*
 * Purpose: this file is use to hook methord for tracing malloc and free.
 * Author:  leapking
 * Description:
 * __malloc_hook is support by glibc for debugging malloc. Include:
 *      void *(*__malloc_hook)(size_t size, const void *caller);
 *      void *(*__realloc_hook)(void *ptr, size_t size, const void *caller);
 *      void *(*__memalign_hook)(size_t alignment, size_t size, const void *caller);
 *      void (*__free_hook)(void *ptr, const void *caller);
 *      void (*__malloc_initialize_hook)(void);
 *      void (*__after_morecore_hook)(void);
 *
 * Notice: hook method is not thread safety
 */

#include <stdio.h>
#include <malloc.h>

/* prototype define for us */
static void my_init_hook(void);
static void *my_malloc_hook(size_t, const void *);
static void my_free_hook(void *, const void *);

/* some temp variable */
static void *(*old_malloc_hook)(size_t, const void *);
static void (*old_free_hook)(void *, const void *);

/* init function */
static void
my_init_hook(void){
        old_malloc_hook = __malloc_hook;
        old_free_hook = __free_hook;
        __malloc_hook = my_malloc_hook;
        __free_hook = my_free_hook;
}

/*my alloc hook*/
static void *
my_malloc_hook(size_t size, const void * caller)
{
        void *result;

        /* restore all old hooks */
        __malloc_hook = old_malloc_hook;
        __free_hook = old_free_hook;

        /*call recursively*/
        result = malloc(size);

        /* save underlying hooks */
        old_malloc_hook = __malloc_hook;
        old_free_hook = __free_hook;

        /* printf might call malloc, so protect it too */
        printf("malloc(%u) call from %p, return %p\n",(unsigned int)size, caller, result);

        __malloc_hook = my_malloc_hook;
        __free_hook = my_free_hook;

        return result;
}

static void
my_free_hook(void *ptr, const void *caller)
{
        __malloc_hook = old_malloc_hook;
        __free_hook = old_free_hook;

        free(ptr);

        old_malloc_hook = __malloc_hook;
        old_free_hook = __free_hook;

        printf("%s free %p\n", caller, ptr);

        __malloc_hook = my_malloc_hook;
        __free_hook = my_free_hook;
}

/* initialize hook */
void (*__malloc_initialize_hook) (void) = my_init_hook;

int main(void)
{
    char *p;

    p = malloc(10);
    free(p);

    return 0;
}

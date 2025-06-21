#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int parent_pid = getpid();
    int pid = fork();
    uint64 result_p = 0;
    if (pid == 0)
    {
        // child
        char *test = "test";
        printf("child sharing\n");
        result_p = map_shared_pages(test, strlen(test), parent_pid);
        printf("result_p str in child = %s\n", result_p);
        sleep(10);
        printf("result_p in child = %d\n", result_p);
        exit(0);
    }
    if (pid < 0)
    {
        // error
    }
    else
    {
        // parent
        // printf("parent waiting\n");
        sleep(1);
        printf("parent finished waiting\n");
    }
    printf("result_p in parent = %d\n", result_p);
    int PGSIZE = 4096; // bytes per page
    uint64 pointer = (((getsz())) & ~(PGSIZE - 1));
    printf("pointer in parent = %d\n", pointer);
    if (pointer > 0)
    {
        char *str = (char *)pointer;
        printf("result = %s\n", *str);
        unmap_shared_pages((void *)pointer, (int)strlen(str));
    }
    else
    {
        printf("error sharing mapped pages\n");
    }

    exit(0);
}

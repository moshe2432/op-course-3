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
        char *test = "test\n";
        printf("child sharing\n");
        result_p = map_shared_pages(test, sizeof(test), parent_pid);
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
    printf("result_p in parent = %s\n", result_p);

    if (result_p > 0)
    {
        char *str = (char *)result_p;
        printf(str);
        unmap_shared_pages((void *)result_p, (int)strlen(str));
    }
    else
    {
        printf("error sharing mapped pages\n");
    }
    
    exit(0);
}

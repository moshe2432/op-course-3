#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    char buf[32] = "parent original";
    int parent_pid = getpid();
    int pid = fork();
    uint64 shared_va = 0;

    if (pid == 0)
    {
        // child
        sleep(1); // wait for parent to map
        printf("child: mapping shared\n");
        shared_va = map_shared_pages(buf, sizeof(buf), parent_pid);
        if (shared_va == 0)
        {
            printf("child: map_shared_pages failed\n");
            exit(1);
        }
        printf("child: writing to shared memory\n");
        printf("child: shared_va = %d\n", shared_va);
        printf("child: buf = %d\n", buf);
        strcpy((char *)shared_va, "Hello daddys");
        printf("child: wrote '%s'\n", (char *)shared_va);
        printf("child: buf is: %s\n", buf);
        unmap_shared_pages((void *)shared_va, sizeof(buf));
        printf("child: unmap_shared_pages done\n");
        exit(0);
    }
    else if (pid > 0)
    {
        // parent
        printf("parent: waiting for child\n");

        wait(0);
        printf("parent: child done, buf = '%s'\n", buf);
        // Optionally unmap
    }
    else
    {
        printf("fork failed\n");
        exit(1);
    }
    exit(0);
}

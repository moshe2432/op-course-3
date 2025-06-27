#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    char buf[32] = "parent original";
    int parent_pid = getpid();

    uint64 shared_va = 0;
    int p[2];
    pipe(p);
    int pid = fork();
    if (pid == 0)
    {
        // child
        sleep(3); // Ensure parent runs first
        close(p[1]);
        read(p[0], &shared_va, sizeof(shared_va));
        printf("child: read shared_va = %p\n", (void *)shared_va);
        close(p[0]);
        if (shared_va == 0)
        {
            printf("child: map_shared_pages failed\n");
            exit(1);
        }
        printf("child: writing to shared memory\n");
        strcpy((char *)shared_va, "Hello daddys");

        unmap_shared_pages((void *)shared_va, sizeof(buf));
        printf("child: unmap_shared_pages done\n");
        exit(0);
    }
    else if (pid > 0)
    {
        // parent
        close(p[0]);
        printf("parent: mapping shared\n");
        shared_va = map_shared_pages(buf, sizeof(buf), parent_pid, pid);
        printf("parent: shared_va = %p\n", (void *)shared_va);
        write(p[1], &shared_va, sizeof(shared_va));
        close(p[1]);
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

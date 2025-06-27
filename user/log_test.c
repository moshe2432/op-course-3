#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// #define PGSIZE 4096 // bytes per page

int main(int argc, char *argv[])
{
    printf("111\n");
    int num_child = 1;
    int pipefd[2];

    printf("shared buffer\n");
    // shared buffer
    char buffer[4096];
    uint64 shared_va = 0;
    printf("piping\n");

    if (pipe(pipefd) == -1)
    {
        exit(1);
    }

    int parent_pid = getpid();

    printf("loop");

    int i;
    for (i = 0; i < num_child; i++)
    {
        int pid = fork();
        printf("fork pid = %d\n", pid);
        if (pid == 0)
        {
            // child
            sleep(3); // Ensure parent runs first
            close(pipefd[1]);
            read(pipefd[0], &shared_va, sizeof(shared_va));
            printf("child: read shared_va = %p\n", (void *)shared_va);
            close(pipefd[0]);
            if (shared_va == 0)
            {
                printf("child: map_shared_pages failed\n");
                exit(1);
            }
            printf("child: writing to shared memory\n");
            strcpy((char *)shared_va, "Hello daddys");

            printf("child: unmap_shared_pages done\n");
            exit(0);
        }
        else if (pid > 0)
        {
            // parent
            close(pipefd[0]);
            printf("parent: mapping shared\n");
            shared_va = map_shared_pages(buffer, sizeof(buffer), parent_pid, pid);
            printf("parent: shared_va = %p\n", (void *)shared_va);
            write(pipefd[1], &shared_va, sizeof(shared_va));
            close(pipefd[1]);
            printf("parent: waiting for child\n");
            wait(0);
            printf("parent: child done, buffer = '%s'\n", buffer);
            // Optionally unmap
        }
        else
        {
            printf("fork failed\n");
            exit(1);
        }
    }
    // fork the child processes
    // set up the shared buffer
    // implement the message-writing protocol.
    exit(0);
}

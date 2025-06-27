#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// #define PGSIZE 4096 // bytes per page

int main(int argc, char *argv[])
{
    // shared buffer
    char buffer[4096];
    uint64 shared_va = 0;
    int parent_pid = getpid();

    printf("piping\n");
    int num_child = 4;
    int (*pipes)[2] = malloc(num_child * sizeof(int[2]));
    for (int i = 0; i < num_child; i++)
    {
        if (pipe(pipes[i]) == -1)
        {
            exit(1);
        }
    }

    // int pipes[4][2];

    // if (pipe(pipes[0]) == -1 || pipe(pipes[1]) == -1 || pipe(pipes[2]) == -1 || pipe(pipes[3]) == -1)
    // {
    //     printf("pipe failed\n");
    //     exit(1);
    // }

    printf("loop");

    int i;
    for (i = 0; i < num_child; i++)
    {
        int pid = fork();
        if (pid == 0)
        {
            // child
            sleep(3);           // Ensure parent runs first
            close(pipes[i][1]); // close write end
            read(pipes[i][0], &shared_va, sizeof(shared_va));
            printf("child: read shared_va = %p\n", (void *)shared_va);
            close(pipes[i][0]);
            if (shared_va == 0)
            {
                printf("child: map_shared_pages failed\n");
                exit(1);
            }
            // todo : write to shared memory

            // todo unmap_shared_pages
            exit(0);
        }
        else if (pid > 0)
        {
            printf("fork pid = %d\n", pid);
            // parent
            close(pipes[i][0]); // close read end
            printf("parent: mapping shared\n");
            shared_va = map_shared_pages(buffer, sizeof(buffer), parent_pid, pid);
            printf("parent: shared_va = %p\n", (void *)shared_va);
            write(pipes[i][1], &shared_va, sizeof(shared_va));
            close(pipes[i][1]);
        }
        else
        {
            printf("fork failed\n");
            exit(1);
        }
    }

    // wait for all children to finish
    for (i = 0; i < num_child; i++)
    {
        int pid = wait(0);
    }

    // fork the child processes
    // set up the shared buffer
    // implement the message-writing protocol.
    exit(0);
}

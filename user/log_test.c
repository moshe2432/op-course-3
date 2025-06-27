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
    int pipes[4][2];

    if (pipe(pipes[0]) == -1 || pipe(pipes[1]) == -1 || pipe(pipes[2]) == -1 || pipe(pipes[3]) == -1)
    {
        printf("pipe failed\n");
        exit(1);
    }

    // int pid1 = fork();
    // if (pid1 < 0)
    // {
    //     printf("fork failed\n");
    //     exit(1);
    // }
    // if (pid1 == 0)
    // {
    //     // child process
    //     close(pipe1[1]); // close write end
    //     read(pipe1[0], &shared_va, sizeof(shared_va));
    //     printf("child: read shared_va = %p\n", (void *)shared_va);
    //     close(pipe1[0]);
    //     if (shared_va == 0)
    //     {
    //         printf("child: map_shared_pages failed\n");
    //         exit(1);
    //     }
    //     // todo: write to shared memory

    //     exit(0);
    // }

    // // parent process
    // close(pipe1[0]); // close read end
    // printf("parent: mapping shared to child 1\n");
    // shared_va = map_shared_pages(buffer, sizeof(buffer), parent_pid, pid1);
    // printf("parent: shared_va = %p\n", (void *)shared_va);
    // write(pipe1[1], &shared_va, sizeof(shared_va));
    // close(pipe1[1]);

    // int pid2 = fork();
    // if (pid2 < 0)
    // {
    //     printf("fork failed\n");
    //     exit(1);
    // }
    // if (pid2 == 0)
    // {
    //     // child process
    //     close(pipe2[1]); // close write end
    //     read(pipe2[0], &shared_va, sizeof(shared_va));
    //     printf("child: read shared_va = %p\n", (void *)shared_va);
    //     close(pipe2[0]);
    //     if (shared_va == 0)
    //     {
    //         printf("child: map_shared_pages failed\n");
    //         exit(1);
    //     }
    //     // todo: write to shared memory
    //     exit(0);
    // }

    // // parent process
    // close(pipe2[0]); // close read end
    // printf("parent: mapping shared to child 2\n");
    // shared_va = map_shared_pages(buffer, sizeof(buffer), parent_pid, pid2);
    // printf("parent: shared_va = %p\n", (void *)shared_va);
    // write(pipe2[1], &shared_va, sizeof(shared_va));
    // close(pipe2[1]);

    // int pid3 = fork();
    // if (pid3 < 0)
    // {
    //     printf("fork failed\n");
    //     exit(1);
    // }
    // if (pid3 == 0)
    // {
    //     // child process
    //     close(pipe3[1]); // close write end
    //     read(pipe3[0], &shared_va, sizeof(shared_va));
    //     printf("child: read shared_va = %p\n", (void *)shared_va);
    //     close(pipe3[0]);
    //     if (shared_va == 0)
    //     {
    //         printf("child: map_shared_pages failed\n");
    //         exit(1);
    //     }
    //     // todo: write to shared memory
    //     exit(0);
    // }
    // // parent process
    // close(pipe3[0]); // close read end
    // printf("parent: mapping shared to child 3\n");
    // shared_va = map_shared_pages(buffer, sizeof(buffer), parent_pid, pid3);
    // printf("parent: shared_va = %p\n", (void *)shared_va);
    // write(pipe3[1], &shared_va, sizeof(shared_va));
    // close(pipe3[1]);

    // int pid4 = fork();
    // if (pid4 < 0)
    // {
    //     printf("fork failed\n");
    //     exit(1);
    // }
    // if (pid4 == 0)
    // {
    //     // child process
    //     close(pipe4[1]); // close write end
    //     read(pipe4[0], &shared_va, sizeof(shared_va));
    //     printf("child: read shared_va = %p\n", (void *)shared_va);
    //     close(pipe4[0]);
    //     if (shared_va == 0)
    //     {
    //         printf("child: map_shared_pages failed\n");
    //         exit(1);
    //     }
    //     // todo: write to shared memory
    //     exit(0);
    // }
    // // parent process
    // close(pipe4[0]); // close read end
    // printf("parent: mapping shared to child 4\n");
    // shared_va = map_shared_pages(buffer, sizeof(buffer), parent_pid, pid4);
    // printf("parent: shared_va = %p\n", (void *)shared_va);
    // write(pipe4[1], &shared_va, sizeof(shared_va));
    // close(pipe4[1]);

    // printf("parent: waiting for children\n");
    // wait(pid1);
    // wait(pid2);
    // wait(pid3);
    // wait(pid4);

    // printf("parent: child done, buffer = '%s'\n", buffer);
    ////////////////////////////////////////////////////////////

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
            close(pipes[i][0]);
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

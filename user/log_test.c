#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int num_child = 1;
    int i;
    // shared memory buffer
    // buf    
    for (i = 0; i < num_child; i++)
    {
        int pid = fork();
        if (pid == -1)
        {
            exit(1);
        }
    }
    // fork the child processes
    // set up the shared buffer
    // implement the message-writing protocol.
    exit(0);
}

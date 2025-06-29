#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define PGSIZE 4096 // bytes per page

char *strcat(char *dest, const char *src)
{
    char *p = dest;
    // Move p to the end of dest string
    while (*p)
        p++;
    // Copy src to the end of dest
    while ((*p++ = *src++) != '\0')
        ;
    return dest;
}

void itoa(int num, char *buf)
{
    int i = 0;
    char tmp[16];
    if (num == 0)
    {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }
    while (num > 0)
    {
        tmp[i++] = '0' + (num % 10);
        num /= 10;
    }
    // reverse the digits
    for (int j = 0; j < i; j++)
    {
        buf[j] = tmp[i - j - 1];
    }
    buf[i] = '\0';
}

int main(int argc, char *argv[])
{
    char *buffer = malloc(PGSIZE); // get heap memory

    int parent_pid = getpid();

    printf("piping, parent pid = %d, original pointer = %p\n", parent_pid, &buffer);
    int num_child = 16;
    int (*pipes)[2] = malloc(num_child * sizeof(int[2]));
    for (int i = 0; i < num_child; i++)
    {
        if (pipe(pipes[i]) == -1)
        {
            printf("error with pipe number %d\n", i);
            exit(1);
        }
    }

    int i;
    for (i = 0; i < num_child; i++)
    {
        int pid = fork();
        if (pid == 0)
        {
            // child
            // sleep(3);           // Ensure parent runs first
            close(pipes[i][1]); // close write end
            void *shared_va;
            read(pipes[i][0], &shared_va, sizeof(uint64));
            printf("child: read shared_va = %p\n", shared_va);
            close(pipes[i][0]);
            if (shared_va == 0)
            {
                printf("child: map_shared_pages failed\n");
                exit(1);
            }
            // todo : write to shared memory
            char to_write[18];
            strcpy(to_write, "this is the child");
            char index_str[16];
            itoa(i, index_str);
            strcat(to_write, index_str);
            uint64 offset = 0;

            uint32 new_header = ((uint32)i << 16 | (uint32)(strlen(to_write) + 1));
            printf("size of new text to write = %d\n", strlen(to_write));
            uint32 *header_ptr = (uint32 *)((char *)shared_va + offset);
            uint32 value = __sync_val_compare_and_swap(header_ptr, 0, new_header);

            while (offset < PGSIZE)
            {
                if (value == 0)
                {

                    strcpy((char *)(shared_va + offset + sizeof(uint32)), to_write);
                    break;
                }
                uint64 msg_length = (uint64)(value & 0xFFFF);
                uint64 header_length = (uint64)sizeof(uint32);
                offset += header_length;
                offset += msg_length;

                offset = (offset + 3) & ~3;
                uint64 *addr = (uint64 *)(shared_va + offset);

                value = __sync_val_compare_and_swap(addr, 0, new_header);
            }

            // todo unmap_shared_pages
            exit(0);
        }
        else if (pid > 0)
        {
            // printf("fork pid = %d\n", pid);
            // parent
            close(pipes[i][0]); // close read end
            // printf("parent: mapping shared\n %d, %d, \n", sizeof(buffer), parent_pid);
            uint64 shared_va = 0;
            shared_va = map_shared_pages((void *)buffer, sizeof(buffer), parent_pid, pid);
            // printf("parent: shared_va = %p\n", (void *)shared_va);
            write(pipes[i][1], (void *)&shared_va, sizeof(uint64));
            close(pipes[i][1]);
            // wait(0);
        }
        else
        {
            printf("fork failed\n");
            exit(1);
        }
    }

    // wait for all children to finish
    for (int i = 0; i < num_child; i++)
    {
        wait(0);
    }

    printf("what is in the buffer? in pointer %p\n", &buffer);
    int offset = 0;
    char header[32];
    char zero_header[32] = {0};
    while (offset < PGSIZE)
    {
        for (int i = 0; i < 32; i++)
        {
            header[i] = buffer[i + offset];
        }

        if (memcmp(header, zero_header, 32) == 0)
        {
            continue;
        }

        uint32 header_value = *(uint32 *)header;
        printf("(%d) ", (uint16)(header_value >> 16));
        uint64 msg_length = (uint64)(header_value & 0xFFFF);
        // printf("msg_length in this part = %d\n", msg_length);

        for (int i = 0; i < msg_length; i++)
        {
            printf("%c", buffer[i + offset + sizeof(uint32)]);
        }
        printf("\n");

        offset += sizeof(uint32) + msg_length;
        offset = (offset + 3) & ~3;
    }

    // int header_size = 0;
    // for (int i = 0; i < PGSIZE; i++)
    // {
    //     if (buffer[i] == 0)
    //     {
    //         header_size++;
    //         continue;
    //     }

    //     if (header_size > 0)
    //     {
    //         printf("(%d)", header_size);
    //         header_size = 0;
    //     }
    //     printf("%c", buffer[i]);
    // }
    printf("\n");

    // fork the child processes
    // set up the shared buffer
    // implement the message-writing protocol.
    exit(0);
}

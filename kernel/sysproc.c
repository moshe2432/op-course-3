#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0; // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_getsz(void)
{
  return myproc()->sz;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if (growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n)
  {
    if (killed(myproc()))
    {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// System call to map shared pages
uint64
sys_map_shared_pages(void)
{
  struct proc *src_proc = myproc();
  struct proc *dst_proc;
  int dst_pid;
  uint64 src_va;
  int size;
  argaddr(0, &src_va);
  argint(1, &size);
  argint(2, &dst_pid);

  dst_proc = procbyid(dst_pid);

  if (src_va >= MAXVA)
  {
    printf("sys_map_shated_pages src_va >= MAXVA");
    return -1;
  }
  if (size <= 0)
  {
    printf("sys_map_shated_pages size <= 0");
    return -1;
  }
  if (dst_proc == 0)
  {
    printf("sys_map_shated_pages dst_proc == 0");
    return -1;
  }
  if (dst_proc->pid == src_proc->pid)
  {
    printf("sys_map_shated_pages dst_proc->pid == src_proc->pid");
    return -1;
  }

  return map_shared_pages(src_proc, dst_proc, src_va, size);
}

uint64
sys_unmap_shared_pages(void)
{
  struct proc *p = myproc();
  uint64 addr;
  int size;
  argaddr(0, &addr);
  argint(1, &size);
  return unmap_shared_pages(p, addr, size);
}
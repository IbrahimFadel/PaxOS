#include "syscall/exit.h"
#include "proc.h"
#include "scheduler.h"

void sys_exit(int code) {
  current_proc->state = PROC_ZOMBIE;
  scheduler_remove(current_proc);
  schedule();
  __builtin_unreachable();
}

#include "syscall/syscall.h"
#include "logging/logging.h"
#include "syscall/write.h"
#include "syscall/exit.h"
#include "syscall/brk.h"

void syscall_handler(trap_frame_t *tf) {
  LOGI("syscall_handler: eax = %d\n", tf->eax);
  switch (tf->eax) {
  case SYS_WRITE: tf->eax = write(tf->ebx, (void *)tf->ecx, tf->edx); break;
  case SYS_EXIT:  exit(tf->ebx); break;
  case SYS_BRK:   tf->eax = (uint32_t)sbrk(tf->ebx); break;
  default:        tf->eax = -1; break;
  }
}

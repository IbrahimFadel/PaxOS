#include "syscall/syscall.h"
#include "i386/mmap_config.h"
#include "logging/logging.h"
#include "syscall/write.h"
#include "syscall/exit.h"
#include "syscall/brk.h"

void syscall_handler(trap_frame_t *tf) {
  LOGI("syscall_handler: eax = %d\n", tf->eax);
  switch (tf->eax) {
  case SYS_WRITE: tf->eax = sys_write(tf->ebx, (void *)tf->ecx, tf->edx); break;
  case SYS_EXIT:  sys_exit(tf->ebx); break;
  case SYS_BRK:   tf->eax = (uint32_t)sys_sbrk(tf->ebx); break;
  default:        tf->eax = -1; break;
  }
}

bool uptr_valid(const void *ptr, size_t len) {
  uint32_t start = (uint32_t)ptr;
  uint32_t end = start + len;

  if (end < start || end > KERNEL_VA) {
    LOGD("uptr_valid: invalid");
    return false;
  }

  return true;
}

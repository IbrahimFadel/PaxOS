#ifndef KERNEL_PROC_PROC_H
#define KERNEL_PROC_PROC_H

#include "fs/fs.h"
#include "gdt.h"
#include "interrupts/idt.h"
#include "mem/page_table.h"
#include "stdbool.h"
#include <stdint.h>
#include <stddef.h>

typedef uint32_t cpu_id_t;

#define CPU_ID_SENTINEL -1

typedef struct {
  cpu_id_t id;
  int num_interrupt_offs;
  bool interrupts_enabled;
} cpu_t;

cpu_t *cpu_current(void);

typedef uint32_t pid_t;

typedef enum { PROC_RUNNING, PROC_READY, PROC_BLOCKED, PROC_ZOMBIE } proc_state_t;

typedef struct __attribute__((packed)) {
  // iret with privilege change pops these in order:
  uint32_t eip;
  uint32_t cs;
  uint32_t eflags;
  uint32_t esp; // user stack pointer
  uint32_t ss;  // user stack segment
} user_entry_frame_t;

typedef struct __attribute__((packed)) {
  uint32_t ebp;
  uint32_t edi;
  uint32_t esi;
  uint32_t ebx;
  uint32_t eip;
} proc_context_t;

typedef struct proc {
  uint32_t pid;
  page_table_t page_dir;
  trap_frame_t *tf;
  proc_context_t *ctx;
  void *kernel_stack;
  proc_state_t state;
  struct proc *next;
  fd_entry_t fds[KCONFIG_MAX_FDS];
} proc_t;

proc_t *process_create(void (*entry)(void), priv_t priv);
proc_t *process_create_user(void (*entry)(void));
proc_t *process_create_kernel(void (*entry)(void));
void process_destroy(proc_t *proc);

#define PROCESS_PAGEDIR_OFFSET 4
#define PROCESS_TF_OFFSET      8

_Static_assert(offsetof(proc_t, page_dir) == PROCESS_PAGEDIR_OFFSET,
               "PROCESS_PAGEDIR_OFFSET wrong");
_Static_assert(offsetof(proc_t, tf) == PROCESS_TF_OFFSET, "PROCESS_TF_OFFSET wrong");

#endif

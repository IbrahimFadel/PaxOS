__attribute__((section(".user_text"))) void user_function(void) {
  // Can't call kernel functions from here — wrong ring, wrong mappings
  // Can only do things that don't need syscalls yet
  for (;;) {
    volatile int x = 0;
    for (int i = 0; i < 1000000; i++) x++;
    // Later: __asm__ volatile("int $0x80" :: "a"(SYS_WRITE) ...);
  }
}

__attribute__((section(".user_text"))) void user_function(void) {
  for (;;) {
    volatile int x = 0;
    for (int i = 0; i < 1000000; i++) x++;
    // TODO: syscall
  }
}

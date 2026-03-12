.intel_syntax noprefix

.global _start
_start:
  call main
  mov ebx, eax // return value
  mov eax, 0
  int 0x80

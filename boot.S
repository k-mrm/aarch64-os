.section ".text.boot"

.global _start

_start:
loop:
  wfe
  b loop

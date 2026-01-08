    .section .text
    .global _start
    .extern ext1
    .extern ext2

_start:
    bl ext1
    bl ext2
    ldr r0, =ext1
    bx  lr

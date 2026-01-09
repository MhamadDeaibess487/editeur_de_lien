    .section .data
    .global myvar
myvar:
    .word 42

    .section .text
    .global _start
_start:
    ldr r0, =myvar
    bx  lr

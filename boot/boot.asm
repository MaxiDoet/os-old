section .multiboot
    dd 0x1badb002
    dd 7
    dd -(0x1badb002 + 7)
    dd 0
    dd 0
    dd 0
    dd 0
    dd 0
    dd 0
    dd 800
    dd 600
    dd 16
    ;space 4 * 13

section .bootstrap_stack nobits
stack_bottom:
    resb 16384 ; 16 KiB
stack_top:

section .text

extern kernel_main

global _start
_start:
    cli

    ; Pass the multiboot info to the kernel
    push ebx
    call kernel_main

    cli
    hlt
.lhang:
    jmp .lhang

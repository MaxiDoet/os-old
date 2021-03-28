section .multiboot
align 4
    dd 1BADB002h
    dd 0003h ; set align boot modules to page boundaries, supply memory map
    dd -(1BADB002h + 0003h)

section .bootstrap_stack nobits
stack_bottom:
    resb 16384 ; 16 KiB
stack_top:

section .text

extern kernel_main

global _start
_start:
    cli
    call kernel_main

    cli
    hlt
.lhang:
    jmp .lhang

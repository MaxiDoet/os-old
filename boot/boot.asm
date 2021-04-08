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

extern kmain

global _start
_start:
    cli

    ; Enable sse
    mov eax, cr0
    and ax, 0xFFFB		;clear coprocessor emulation CR0.EM
    or ax, 0x2			;set coprocessor monitoring  CR0.MP
    mov cr0, eax
    mov eax, cr4
    or ax, 3 << 9		;set CR4.OSFXSR and CR4.OSXMMEXCPT at the same time
    mov cr4, eax

    ; Pass the multiboot info to the kernel
    push ebx
    ; Pass the magic value
    push eax

    call kmain

    cli
    hlt
.lhang:
    jmp .lhang

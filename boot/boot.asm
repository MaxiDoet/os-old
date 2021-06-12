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

global enable_sse

enable_sse:
	mov eax, cr0
	and ax, 0xFFFB
	or ax, 0x2
	mov cr0, eax
	mov eax, cr4
	or ax, 3 << 9
	mov cr4, eax
	ret

_start:
    cli

    ; Pass the multiboot info to the kernel
    push ebx
    ; Pass the magic value
    push eax

    call enable_sse

    call kmain

    cli
    hlt
.lhang:
    jmp .lhang


bits 32

extern kernel_main

%include "mb.inc"

%define STACKSIZE 1024 * 16
%define VIRTUAL_OFFSET 0xC0000000
%define GDT_BASE (0xFFFFFFFF - (VIRTUAL_OFFSET - 1))

; Stack
section .bss
align 4
stack_bottom:
    resb STACKSIZE
stack_top:

; Multiboot definition
section .setup
align 4
    
    ; Required
    dd MULTIBOOT_HEADER_MAGIC
    dd FLAGS
    dd CHECKSUM 

    dd 0, 0, 0, 0, 0    
    ; Graphics

    dd 0    ; Mode, 0 - linear, 1 - text mode

    dd 1024, 768, 32  ; Width, Height, BPP

align 4
tmp_gdtr:
    dw ((3 * 8) - 1)
    dd tmp_gdt

align 4
tmp_gdt:
	; NULL DESCRIPTOR 
	dw	0x0000
	dw	0x0000
	dw	0x0000
	dw	0x0000

	; KERNEL CODE 
	dw	0xFFFF		; segment limit 15-00 
	dw	0x0000		; base address 15-00 
	db	0x00		; base address 23-16 
	db	0x9A		; P=1 DPL=00 S=1 TYPE=1010 (exec;read) 
	db	0xCF		; G=1 DB=1 0=0 AVL=0 SEGLIM=1111 
	db	GDT_BASE >> 24	; base address 31-24 

	; KERNEL DATA
	dw	0xFFFF		; segment limit 15-00 
	dw	0x0000		; base address 15-00 
	db	0x00		; base address 23-16 
	db	0x92		; P=1 DPL=00 S=1 TYPE=0010 (read;write) 
	db	0xCF		; G=1 DB=1 0=0 AVL=0 SEGLIM=1111 
	db	GDT_BASE >> 24	; base address 31-24 
tmp_gdt_end:

global start
start:
    cli
    jmp gdt_flush

    lgdt [tmp_gdtr]

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    jmp 0x08:gdt_flush

section .text

gdt_flush:
    mov esp, stack_top

    push dword 0
    popf

    push ebx
    call kernel_main

    cli
halt:
    hlt
    jmp halt
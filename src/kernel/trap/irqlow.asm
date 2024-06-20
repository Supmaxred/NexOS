extern handle_irqint

global irq_table

%macro irqint 1
irqint_%+%1:
    push %1
    jmp irq_master
%endmacro

section .text
    irqint 0
    irqint 1
    irqint 2
    irqint 3
    irqint 4
    irqint 5
    irqint 6
    irqint 7
    irqint 8
    irqint 9
    irqint 10
    irqint 11
    irqint 12
    irqint 13
    irqint 14
    irqint 15

irq_table:
%assign i 0 
%rep    16
    dd irqint_%+i
%assign i i+1 
%endrep

extern kernelds

irq_master:

    ;mov ecx, 0
    ;loop:
    ;inc ecx
    ;cmp ecx, 0xffffffff
    ;jne loop

    pusha                    ; Push general registers

    mov ax, ds
    push eax                 ; Push data segment

    mov ax, [kernelds]  ; Load the kernel data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call handle_irqint

    pop eax        ; Reload the original data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa                     ; Pop general registers

    add esp, 4     ; Clean up the pushed error code and IRQ number
    iret           ; Pop 5 things at once: CS, EIP, EFLAGS, SS, and ESP
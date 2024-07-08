#ifndef ARCH_H
#define ARCH_H

#include <stdint.h>

#define IDT_ENTRIES 256

typedef struct {
	uint16_t    isr_low;		// The lower 16 bits of the ISR's address
	uint16_t    gdt_cs;			// The GDT segment selector that the CPU will load into CS before calling the ISR
	uint8_t     reserved;		// Set to zero
	uint8_t     flags;			// Type and attributes; see the IDT page
	uint16_t    isr_high;		// The higher 16 bits of the ISR's address
} __attribute__((packed)) idt_entry_t;

typedef struct {
	uint16_t	limit;
	uint32_t	base;
} __attribute__((packed)) idtr_t;

typedef struct isrctx {
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;//Pushed by pusha
    uint32_t ds;                                    //Pushed by handler, data segment
    uint32_t intnum, errcode;                       //Pushed by handler, if there is no error errcode is zero
    uint32_t eip, cs, eflags, useresp, ss;          //Pushed by CPU
} isrctx_t;

typedef struct irqctx {
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;//Pushed by pusha
    uint32_t ds;                                    //Pushed by handler, data segment
    uint32_t irq;                       			//Pushed by handler
    uint32_t eip, cs, eflags, useresp, ss;          //Pushed by CPU
} irqctx_t;

typedef void (*irq_handler)(irqctx_t);

void idt_setentry(uint8_t vector, void* isr, uint16_t cs, uint8_t flags);
void idt_setirqhandler(uint8_t line, irq_handler handler, uint8_t enable);
void idt_init(void);
void irq_sendeoi(uint8_t irq);
void irq_remap(void);
void irq_setmask(uint8_t IRQline);
void irq_clearmask(uint8_t IRQline);
void irq_clear(void);

static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile ( "outb %b0, %w1" :: "a"(val), "Nd"(port) : "memory");
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    __asm__ volatile ( "inb %w1, %b0"
                   : "=a"(ret)
                   : "Nd"(port)
                   : "memory"); 
    return ret;
}

static inline uint8_t halt()
{
    __asm__ volatile ("hlt");
}

static inline void io_wait(void)
{
    outb(0x80, 0);
}
#endif
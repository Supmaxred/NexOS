#ifndef ARCH_H
#define ARCH_H

#include <stdint.h>

#define DPL_KRNL 0x0
#define DPL_USER 0x3

#define IS_CS 0x1
#define IS_DS 0x0

#define NUMSEGS 5

typedef struct {
    uint16_t isr_low;           // The lower 16 bits of the ISR's address
    uint16_t gdt_cs;            // The GDT segment selector that the CPU will load into CS before calling the ISR
    uint8_t res;                //Reserved
    uint8_t flags;              // Type and attributes; see the IDT page
    uint16_t isr_high;          // The higher 16 bits of the ISR's address
} __attribute__((packed)) idt_entry_t;

typedef struct {
	uint16_t limit;
	uint32_t offset;
} __attribute__((packed)) idtr_t;

typedef struct {
    uint32_t present     : 1;       // Page present in memory
    uint32_t rw          : 1;       // Read/write flag
    uint32_t us          : 1;       // User/supervisor flag
    uint32_t pwt         : 1;       // Page-level write-through
    uint32_t pcd         : 1;       // Page-level cache disable
    uint32_t accessed    : 1;       // Accessed flag
    uint32_t dirty       : 1;       // Dirty flag (only for 4 MB pages)
    uint32_t page_size   : 1;       // Page size flag (0 = 4 KB, 1 = 4 MB)
    uint32_t global      : 1;       // Global flag (only for 4 MB pages)
    uint32_t avail       : 3;
    uint32_t page_table_base : 20;  // Base address of the page table (aligned to 4 KB)
} pde_t;

typedef struct {
    uint16_t limit_low : 16;        // Low bits of segment limit
    uint16_t offset_low : 16;       // Low bits of segment base address
    uint8_t offset_middle : 8;      // Middle bits of segment base address
    union
    {
        struct
        {
            uint8_t a : 1;
            uint8_t rw : 1;
            uint8_t dc : 1;
            uint8_t e : 1;
            uint8_t s : 1;          
            uint8_t dpl : 2;        // Descriptor Privilege Level
            uint8_t p : 1;          // Is Present
        };
        struct
        {
            uint8_t type : 4;
        };
        struct
        {
            uint8_t accbyte;
        };
    };
    uint8_t limit_high : 4;         // High bits of segment limit
    uint8_t res1 : 1;               // Reserved
    uint8_t res2 : 1;               // Reserved (In AMD64 its long mode flag)
    uint8_t db : 1;                 // 0 = 16-bit segment, 1 = 32-bit segment
    uint8_t g : 1;                  // Granularity: limit scaled by 4K when set
    uint8_t offset_high : 8;        // High bits of segment base address
} gdt_entry_t;

typedef struct {
    uint16_t limit;
    uint32_t offset;
} __attribute__((packed)) gdtr_t;

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

typedef void (*irq_handler)(irqctx_t*);

//int.c
void idt_setentry(uint8_t vector, void* isr, uint16_t cs, uint8_t flags);
void idt_setirqhandler(uint8_t line, irq_handler handler, uint8_t enable);
void idt_init(void);
//irq.c
void irq_sendeoi(uint8_t irqline);
void irq_remap(uint8_t start_int);
void irq_setmask(uint8_t irqline);
void irq_clearmask(uint8_t irqline);
void irq_clear(void);
//seg.c
void gdt_init(void);
//gdt_flush.asm
void gdt_flush(gdtr_t* gtdr);

static inline void gdt_setgate(gdt_entry_t* entry, uint32_t offset, uint32_t limit, uint8_t dpl, uint8_t iscsords, uint8_t iscs)
{
    entry->offset_low = offset & 0xFFFF;
    entry->offset_middle = (offset >> 16) & 0xFF;
    entry->offset_high = offset >> 24;
    entry->limit_low = limit & 0xFFFF;
    entry->limit_high = (limit >> 16) & 0xFF;
    entry->a = 0;
    entry->p = 1;
    entry->dpl = dpl;
    entry->s = !iscsords;
    entry->e = iscs;
    entry->db = 1;
    entry->rw = 1;
    entry->dc = 0;
    entry->g = 1;
    entry->res1 = 0;
    entry->res2 = 0;
}

static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile ( "outb %b0, %w1" :: "a"(val), "Nd"(port) : "memory");
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    __asm__ volatile ( "inb %w1, %b0" : "=a"(ret) : "Nd"(port) : "memory"); 
    return ret;
}

static inline void outw(uint16_t port, uint16_t val)
{
    __asm__ volatile ("outw %0, %1" :: "a"(val), "Nd"(port));
}

static inline uint16_t inw(uint16_t port)
{
    uint16_t ret;
    __asm__ volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void lidt(idtr_t* idtr)
{
    __asm__ volatile ("lidt %0" :: "m"(*idtr));
}

static inline void lgdt(gdtr_t* gdtr)
{
    __asm__ volatile ("lgdt %0" :: "m"(*gdtr));
}

static inline uint32_t gcr0()
{
    uint32_t ret;
    __asm__ volatile("mov %%cr0, %0" : "=r"(ret));
    return ret;
}

static inline void lcr0(uint32_t cr0)
{
    __asm__ volatile("mov %0, %%cr0" : : "r"(cr0));
}

static inline void lcr3(uint32_t* pagedir)
{
    __asm__ volatile("mov %0, %%cr3" : : "r"(pagedir));
}

static inline void cli()
{
    __asm__ volatile ("cli");
}

static inline void sti()
{
    __asm__ volatile ("sti");
}

static inline void hlt()
{
    __asm__ volatile ("hlt");
}

static inline void io_wait(void)
{
    __asm__ volatile ( "outb %%al, $0x80" : : "a"(0) );
}

static inline uint32_t xchg(volatile uint32_t* addr, uint32_t newval)
{
  uint32_t result;

  // The + in "+m" denotes a read-modify-write operand.
  asm volatile("lock; xchgl %0, %1" :
               "+m" (*addr), "=a" (result) :
               "1" (newval) :
               "cc");
  return result;
}

static inline void bochs_brkpt()
{
    __asm__ volatile ("xchg %bx, %bx");
}
#endif
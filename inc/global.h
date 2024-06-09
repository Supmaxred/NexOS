#include <multiboot.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

extern multiboot_info_t* multiboot;
extern uint32_t kernelcs;
extern uint32_t kernel_ticks;

void sleep(uint32_t ms);
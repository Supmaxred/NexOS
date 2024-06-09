//serialio.c
int init_serial();
int is_transmit_empty();
void write_serial(char a);

//serialstdio.c
void dbg_puts(const char* str);
void dbg_printf(const char* format, ...);
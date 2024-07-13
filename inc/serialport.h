//serialio.c
int init_serial();
int is_transmit_empty();
void write_serial(char a);

//serialstdio.c
void sp_puts(const char* str);
void sp_printf(const char* format, ...);
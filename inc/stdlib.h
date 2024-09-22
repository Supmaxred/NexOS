#define RAND_MAX 32767

//convert.c
void itox(unsigned int value, char* buf);
void itos(int num, char* str);
void itob(unsigned int value, char* buf);
void itou(unsigned int value, char* buf);
int stoi(char* str);

//rand.c
void srand(unsigned int seed);
int rand(void);
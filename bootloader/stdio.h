#ifndef INCLUDE_STDIO_H
#define INCLUDE_STDIO_H

int *printf_number(int *argp, int length, int sign, int radix);
void putchar(char c, unsigned int mode);
void aprintf(char *c,int channel);
void printf(const char* fmt, ...);

#endif
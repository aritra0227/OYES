#ifndef INCLUDE_UTILITIES_H
#define INCLUDE_UTILITIES_H

unsigned int divideUnsigned(unsigned int dividend, unsigned int divisor);
unsigned int div_ceil(unsigned int dividend, unsigned int divisor);
unsigned int div_absolute_ceil(unsigned int dividend, unsigned int divisor);
unsigned int modulo(unsigned int dividend, unsigned int divisor); 
void *memset(void *buffer, const unsigned char byte, const unsigned int len);
unsigned int  align_up(unsigned int n, unsigned int a);
unsigned int align_down(unsigned int n, unsigned int a);

#endif
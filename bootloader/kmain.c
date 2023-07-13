#include "stdio.h"
#include "idt.h"
#include "memory_seg.h"

int kmain(){  
    install_gdt();
    install_idt();
    // unsigned long int remainder;
    // unsigned long long number = 12345;
    // int radix =10;
    // x86_div64_32(number, radix, &number, &remainder);
    // aprintf(dummy_text, 0);
    printf("For %d %i %hd %hi %hhu %hhd \r\n", 1234, -5678, (short)27, (short)-42, (unsigned char)20, (signed char)-10);
    return 145;
}
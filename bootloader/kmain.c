#include "stdio.h"
#include "idt.h"
#include "pic.h"
#include "keyboard.h"
#include "memory_seg.h"
#include "system_interrupt_handler.h"

int kmain(){  
    install_gdt();
    install_idt();
    install_pic();
    enable_interrupts();
    install_keyboard();
    // unsigned long int remainder;
    // unsigned long long number = 12345;
    // int radix =10;
    // x86_div64_32(number, radix, &number, &remainder);
    // aprintf(dummy_text, 0);
    printf("Still Working %d %i %hd %hi %hhu %hhd \r\n", 1234, -5678, (short)27, (short)-42, (unsigned char)20, (signed char)-10);
    return 145;
}
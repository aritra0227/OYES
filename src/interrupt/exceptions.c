/**
 * Dedicate this file to software interrupts/exceptions
*/
#include "interrupt/interrupt_handler.h"
#include "stdio.h"

#define PAGEFAULT_INTERRUPT_NUMBER 0x0E

extern unsigned int retrieve_page_vaddr(void);
extern void cli_and_halt(void);

void pagefault_exception_handler(struct CPU_STATE cpu, struct INTERRUPT_INFO interrupt_info, struct STACK_STATE stack)
{
    (void)cpu, (void)interrupt_info, (void)stack;
    unsigned int bad_page_vaddr = retrieve_page_vaddr();
    printf("PAGE FAULT EXCEPTION OCCURED FOR VADDR: %u \r\n", bad_page_vaddr);
    cli_and_halt();
    return;
}

void enable_exceptions(void)
{
    /* Map all software exceptions to appropriate handlers*/
    assign_interrupt_handler(PAGEFAULT_INTERRUPT_NUMBER, pagefault_exception_handler);
    return;
}

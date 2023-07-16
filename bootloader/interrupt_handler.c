#include "interrupt_handler.h"
#include "stdio.h"
#include "constants.h"

interrupt_handler_t interrupt_handlers[IDT_INTERRUPTS_NUM];

void assign_interrupt_handler(unsigned int interrupt_number, interrupt_handler_t handler)
{
    if (interrupt_number < IDT_INTERRUPTS_NUM)
    {
        interrupt_handlers[interrupt_number] = handler;
    }
    else
    {
        (void)interrupt_number, (void)handler;
        printf("Invalid Interrupt Number: %d", (int)interrupt_number);
    }
}

void interrupt_handler(struct CPU_STATE cpu, struct INTERRUPT_INFO interrupt_info, struct STACK_STATE stack)
{
    if (interrupt_handlers[interrupt_info.interrupt_number] != NULL)
    {
        interrupt_handlers[interrupt_info.interrupt_number](cpu, interrupt_info, stack);
    }
    else
    {
        printf("Handler with interrupt number:%d is not implemented", (int)interrupt_info.interrupt_number);
    }
}
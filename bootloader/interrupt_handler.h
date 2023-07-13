#ifndef INCLUDE_INTERRUPT_HANDLER_H
#define INCLUDE_INTERRUPT_HANDLER_H

#define IDT_INTERRUPTS_NUM 256

struct CPU_STATE
{
    unsigned int edi;
    unsigned int esi;
    unsigned int ebp;
    unsigned int edx;
    unsigned int ecx;
    unsigned int ebx;
    unsigned int eax;
    unsigned int esp;
} __attribute__((packed));

struct STACK_STATE
{
    unsigned int eip;
    unsigned int cs;
    unsigned int eflags;
} __attribute__((packed));

struct INTERRUPT_INFO
{
    unsigned int interrupt_number;
    unsigned int error_code;
} __attribute__((packed));

typedef void (*interrupt_handler_t)(struct CPU_STATE cpu, struct INTERRUPT_INFO interrupt_info, struct STACK_STATE stack);

/**
 * This function would be responsible for assigning the correct interrupt handler for the appropriate interrupt number
*/
void assign_interrupt_handler(unsigned int interrupt_number, interrupt_handler_t handler);

/**
 * Handles interrupts through calling the appropriate interrupt handler
*/
void interrupt_handler(struct CPU_STATE cpu, struct INTERRUPT_INFO interrupt_info, struct STACK_STATE stack);

#endif
#ifndef INCLUDE_PROCESS_H
#define INCLUDE_PROCESS_H

struct REGISTERS
{
    unsigned int ss;
    unsigned int esp;
    unsigned int eflags;
    unsigned int cs;
    unsigned int eip;
} __attribute__((packed));
typedef struct REGISTERS REGISTERS_T;

struct PROCESS
{
    unsigned int id; //process id
    unsigned int parent_id;

    unsigned int *pdt; //process pdt
    unsigned int pdt_paddr;

    unsigned int kernel_stack_vaddr;
    unsigned int process_stack_vaddr;
    unsigned int process_stack_paddr;

    unsigned int process_code_vaddr;
    unsigned int process_code_paddr;


    struct REGISTERS current_registers;

} __attribute__((packed));

typedef struct PROCESS PROCESS_T; 

PROCESS_T *create_process(unsigned int *pointer_to_program_vaddr, unsigned int pid);

#endif
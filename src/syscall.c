#include "process.h"
// #include "scheduler.h"
#include "paging.h"
#include "constants.h"
#include "syscall.h"
#include "memory_seg.h"
#include "stdio.h"

static unsigned int NUM_PROCESSES = 0;

extern void enter_kernel_mode(unsigned int program_vaddr);

// temp dummy implementation
unsigned int sys_get_pid()
{
    return ++NUM_PROCESSES;
}

unsigned int sys_fork();

/**
 * Replaces current process with a new process running program from program_addr 
 * @param: program_paddr: program to run when replacing current process
*/
int sys_exec(unsigned int *program_vaddr)
{
    unsigned int pid = sys_get_pid();
    PROCESS_T *proc = create_process(program_vaddr, pid);
    if (proc == NULL)
    {
        return -1;
    }
    // program_t prog = (program_t)program_vaddr;
    // prog();
    // printf("hello this is vaddr %u", proc->process_code_vaddr);
    set_kernel_stack(0x10, proc->kernel_stack_vaddr);
    printf("pdt addr %u, %u", (unsigned int)proc->pdt, proc->pdt_paddr);
    load_process_pdt(proc->pdt, proc->pdt_paddr);
    // printf("hello this is vaddr %u", proc->process_code_vaddr);
    printf("hello this is vaddr yerr");

    typedef void (*program_t)(void);
    program_t prog = (program_t)proc->process_code_vaddr;
    prog();
    // enter_kernel_mode(proc->process_code_vaddr);
    // printf("hi im here now");

    // scheduler_override_current_process(proc);

    return 0;
}
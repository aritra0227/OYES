#include "kernel/process.h"
#include "utility/constants.h"
#include "utility/utilities.h"
#include "memory/heap/malloc.h"
#include "memory/virtual/paging.h"
#include "memory/physical/page_frame_alloc.h"
#include "stdlib/stdio.h"
#define USER_MODE_CODE_SEGMENT_SELECTOR 0x18
#define USER_MODE_DATA_SEGMENT_SELECTOR 0x20
#define PROCESS_INIT_STACK_VADDR (KERNEL_START_VADDR - FOUR_KB) //recall that this is the stack start for all processes in VM

static unsigned int process_set_pdt(PROCESS_T *process)
{
    unsigned int *pdt;
    unsigned int pdt_paddr;
    pdt = create_pdt(&pdt_paddr);
    if (pdt == NULL || pdt_paddr == 0)
    {
        printf("COULD NOT CREATE PDT FOR PROCESS");
        return 0;
    }
    process->pdt = pdt;
    process->pdt_paddr = pdt_paddr;
    printf("this is the pdt_paddr %u", process->pdt_paddr);
    printf("    this is the pdt_vaddr %u", (unsigned int)process->pdt);
    return 1;
}
static unsigned int process_set_code(PROCESS_T *process, unsigned int *code_vaddr)
{
    unsigned int new_code_vaddr = 0;
    unsigned int *code_paddr = pf_allocate_frames(1);
    printf("this is the code paddr %u", code_paddr[0]);
    unsigned int res;
    // (void)code_vaddr;
    if (code_paddr)
    {
        printf("    in this block   ");
        res = kernel_map_page((unsigned int *)code_paddr[0], code_vaddr, 1);
        if (!res)
        {
            printf("COULDN'T MAP CODE FOR PROCESS");
            return 0;
        }
        // unmap_page(code_vaddr);
    }
    // unsigned int tmp_entry = kernel_get_temporary_entry();
    // unsigned int *code_paddr = get_page((unsigned int)code_vaddr);
    // printf("this is paddr %u", (unsigned int)code_paddr);                                        //set code to execution to start at vaddr 0
    res = map_page(process->pdt, (unsigned int *)code_paddr[0], (unsigned int *)new_code_vaddr, 1); //note: gotta map for process-pdt instead
    if (!res)
    {
        printf("COULDN'T MAP CODE FOR PROCESS");
        return 0;
    }
    // typedef void (*program_t)(void);
    // program_t prog = (program_t)code_vaddr;
    // prog();
    // printf("uerrrr");
    process->process_code_vaddr = (unsigned int)new_code_vaddr;
    process->process_code_paddr = code_paddr[0];
    // kernel_set_temporary_entry(tmp_entry);
    return 1;
}

static unsigned int process_set_stack(PROCESS_T *process)
{
    unsigned int *page_frames = pf_allocate_frames(1); //for now allocate 1 page
    if (!page_frames)
    {
        printf("COULDN'T ALLOCATE ENOUGH MEMORY FOR PROCESS STACK");
        return 0;
    }
    unsigned int res = map_page(process->pdt, (unsigned int *)page_frames[0], (unsigned int *)PROCESS_INIT_STACK_VADDR, 1); // note: gotta map for process-pdt instead
    if (!res)
    {
        printf("COULDN'T MAP MEMORY FOR PROCESS STACK");
        return 0;
    }
    process->process_stack_paddr = page_frames[0];
    process->process_stack_vaddr = PROCESS_INIT_STACK_VADDR;
    return 1;
}

static unsigned int process_set_kernel_stack(PROCESS_T *process)
{
    unsigned int *page_frames = pf_allocate_frames(1); //for now allocate 1 page
    unsigned int stack_vaddr = pdt_kernel_find_next_vaddr(FOUR_KB);
    if (!page_frames)
    {
        printf("COULDN'T ALLOCATE ENOUGH MEMORY FOR KERNEL STACK");
        return 0;
    }
    unsigned int res = kernel_map_page((unsigned int *)page_frames[0], (unsigned int *)stack_vaddr, 1); // note: gotta map for process-pdt instead
    if (!res)
    {
        printf("COULDN'T MAP ENOUGH MEMORY FOR KERNEL STACK");
        return 0;
    }
    process->kernel_stack_vaddr = stack_vaddr;
    return 1;
}
static unsigned int process_map_process(PROCESS_T *process)
{
    unsigned int tmp_entry = kernel_get_temporary_entry();
    unsigned int *proc_paddr = get_page((unsigned int)process);
    // printf("this is the vaddr %u, paddr %u, also %u", vaddr, (unsigned int)proc_paddr, (unsigned int)process);
    if (process)
    {
        map_page(process->pdt, proc_paddr, (unsigned int *)process, 1);
        kernel_set_temporary_entry(tmp_entry);
        return 1;
    }
    printf("COULDN'T MAP PROCESS IN PROCESS PDT");
    kernel_set_temporary_entry(tmp_entry);
    return 0;
}

//populates all fields of process
void init_populate_process(PROCESS_T *process)
{
    process->id = 0;
    process->parent_id = 0;
    process->pdt = NULL;
    process->pdt_paddr = 0;
    process->kernel_stack_vaddr = 0;
    process->process_stack_vaddr = 0;
    process->process_code_vaddr = 0;
    process->process_stack_paddr = 0;
    process->process_code_paddr = 0;

    memset(&process->current_registers, 0, sizeof(struct REGISTERS));
}

PROCESS_T *create_process(unsigned int *program_vaddr, unsigned int pid)
{
    PROCESS_T *process = (PROCESS_T *)malloc(sizeof(PROCESS_T));
    init_populate_process(process);
    process->id = pid;
    unsigned int res = process_set_pdt(process);
    if (!res)
    {
        return NULL;
    }
    res = process_set_stack(process);
    if (!res)
    {
        return NULL;
    }
    res = process_set_kernel_stack(process);
    if (!res)
    {
        return NULL;
    }
    res = process_set_code(process, program_vaddr);
    if (!res)
    {
        return NULL;
    }
    res = process_map_process(process);
    if (!res)
    {
        return NULL;
    }
    return process;
}
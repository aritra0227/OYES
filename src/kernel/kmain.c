#include "interrupt/pic.h"
#include "interrupt/syscall.h"
#include "interrupt/system_interrupt_handler.h"
#include "interrupt/exceptions.h"
#include "interrupt/idt.h"
#include "hardware/serial.h"
#include "hardware/keyboard.h"
#include "kernel/process.h"
#include "kernel/multiboot.h"
#include "kernel/user_mode.h"
#include "memory/malloc.h"
#include "memory/memory_seg.h"
#include "memory/paging.h"
#include "memory/page_frame_alloc.h"
#include "stdlib/stdio.h"

#define CHECK_FLAG(flags, bit) ((flags) & (1 << (bit)))
typedef void (*call_module_t)(void);

/*wrapper to call several initialization*/
static void kernel_init()
{
    install_gdt();
    install_idt();
    install_pic();
    install_keyboard();
}

extern void user_entry(void);
typedef void (*program_t)(void);

// static void run_user_mode(void){
//     // unsigned int user_process = create_user_process();
//     // if(user_process == 0){
//     //     printf("COULD NOT CREATE USER_PROCESS");
//     // }
//     enter_user_mode();
// }

void kernel_run_module(multiboot_info_t *mbinfo)
{
    printf("INSIDE RUN MODULE\n");
    multiboot_module_t *modules = (multiboot_module_t *)mbinfo->mods_addr;
    unsigned int address_of_module = modules->mod_start;
    printf("flags: %u,     count :%u",mbinfo->flags,mbinfo->mods_count);
    if (CHECK_FLAG(mbinfo->flags, 3) && (mbinfo->mods_count == 1))
    {
        printf("RUNNING MODULE");
        call_module_t start_program = (call_module_t)address_of_module;
        start_program();
    }
    printf("EOP");
}

int kmain(unsigned int kernel_physical_start, unsigned int kernel_physical_end, unsigned int kernel_virtual_start, unsigned int kernel_virtual_end, unsigned int kernel_pt, unsigned int kernel_pd, unsigned int module_info)
{
    kernel_init();
    (void)kernel_virtual_start, (void)kernel_virtual_end;
    // Note: kernel_virtual_start = 3222274048, kernel_virtual_end= 3222447264
    printf("ADDY OF: kernel_physical_start: %u, kernel_physical_end: %u \r\n", kernel_physical_start, kernel_physical_end);
    // if (kernel_virtual_start == 0xC0100000) printf("kernel_virtual_start: %u", (unsigned int) 3222274048);
    // printf("kernel_physical > kernel_virtual: %d, kernel_phy_st > kernel_phy_end: %d, kernel_virt_st > kernel_virt_end: %d", (int)(kernel_physical_start> kernel_virtual_start), (int)(kernel_physical_start> kernel_physical_end), (int)(kernel_virtual_start > kernel_virtual_end));
    page_frame_allocator_init(kernel_physical_end);
    paging_init(kernel_pd, kernel_pt);
    enable_interrupts();
    enable_exceptions();
    //test malloc:
    unsigned int *malloc_first_test = (unsigned int *)malloc((unsigned int)(sizeof(unsigned int) * 1024));
    for (int i = 0; i < 1024; ++i)
    {
        malloc_first_test[i] = i;
    }
    for (int i = 0; i < 4; ++i)
    {
        printf("malloc %u\n,", malloc_first_test[i]);
    }
    for (int i = 1020; i < 1024; ++i)
    {
        printf("malloc %u\n,", malloc_first_test[i]);
    }
    free(malloc_first_test);
    (void)module_info;
    // program_t user_program = (program_t)pf_allocate_frames(1)[0];
    // user_program = user_entry;
    // program_t user_program = (program_t) malloc(sizeof(program_t));
    // user_program = user_entry;
    // user_program();
    // int res = sys_exec((unsigned int *)user_entry);
    // printf("this is the res %d", res);
    // unsigned int vaddr = pdt_kernel_find_next_vaddr(4);
    // kernel_map_page((unsigned int *)0xc00b8000, (unsigned int *)0xc00b8000, 1);
    // unsigned int *mbinfo_vaddr = (unsigned int *)pdt_kernel_find_next_vaddr(sizeof(multiboot_info_t *));
    // kernel_map_page((unsigned int *)module_info, mbinfo_vaddr, 1);
    enter_user_mode((unsigned int)&user_entry); 
    // multiboot_info_t *mbinfo = (multiboot_info_t *)(module_info);
    // kernel_run_module((multiboot_info_t *)mbinfo_vaddr);
    return 145;
}
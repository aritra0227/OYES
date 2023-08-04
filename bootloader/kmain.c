#include "stdio.h"
#include "idt.h"
#include "pic.h"
#include "multiboot.h"
#include "keyboard.h"
#include "memory_seg.h"
#include "system_interrupt_handler.h"
#include "page_frame_alloc.h"
#include "paging.h"
#include "exceptions.h"
#include "serial.h"

#define CHECK_FLAG(flags, bit) ((flags) & (1 << (bit)))
typedef void (*call_module_t)(void);

/*wrapper to call several initialization*/
void kernel_init()
{
    install_gdt();
    install_idt();
    install_pic();
    enable_interrupts();
    install_keyboard();
    enable_exceptions();
}

void kernel_run_module(multiboot_info_t *mbinfo)
{

    multiboot_module_t *modules = (multiboot_module_t *)mbinfo->mods_addr;
    unsigned int address_of_module = modules->mod_start;
    if (CHECK_FLAG(mbinfo->flags, 3) && (mbinfo->mods_count == 1))
    {
        printf("RUNNING MODULE");
        call_module_t start_program = (call_module_t)address_of_module;
        start_program();
    }
}

int kmain(unsigned int kernel_physical_start, unsigned int kernel_physical_end, unsigned int kernel_virtual_start, unsigned int kernel_virtual_end, unsigned int kernel_pt, unsigned int kernel_pd)
{
    kernel_init();
    (void)kernel_virtual_start, (void)kernel_virtual_end;
    // serial_write("HI I AM TESTING SERIAL");
    // multiboot_info_t *mbinfo = (multiboot_info_t *)ebx;
    // kernel_run_module(mbinfo);
    // printf("Still Working %d %i %hd %hi %u %hhd \r\n", 1234, -5678, (short)27, (short)-42, (unsigned int)20, (signed char)-10);
    //sanity checks:
    //Note to self: printf not properly working, unable to print big numbers e.g. 3222274048 (fix soonish?): update: it prints, it just takes super long, fix fast division
    // Note: kernel_virtual_start = 3222274048, kernel_virtual_end= 3222447264
    printf("ADDY OF: kernel_physical_start: %u, kernel_physical_end: %u \r\n", kernel_physical_start, kernel_physical_end);
    // if (kernel_virtual_start == 0xC0100000) printf("kernel_virtual_start: %u", (unsigned int) 3222274048);
    // printf("kernel_physical > kernel_virtual: %d, kernel_phy_st > kernel_phy_end: %d, kernel_virt_st > kernel_virt_end: %d", (int)(kernel_physical_start> kernel_virtual_start), (int)(kernel_physical_start> kernel_physical_end), (int)(kernel_virtual_start > kernel_virtual_end));
    page_frame_allocator_init(kernel_physical_end);
    paging_init(kernel_pd, kernel_pt);
    return 145;
}
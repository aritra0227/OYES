#include "stdio.h"
#include "idt.h"
#include "pic.h"
#include "multiboot.h"
#include "keyboard.h"
#include "memory_seg.h"
#include "system_interrupt_handler.h"

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
}

void kernel_run_module(multiboot_info_t* mbinfo){
    
    multiboot_module_t *modules = (multiboot_module_t *) mbinfo->mods_addr;
    unsigned int address_of_module = modules->mod_start;
    if (CHECK_FLAG(mbinfo->flags, 3) && (mbinfo->mods_count == 1))
    {
        printf("RUNNING MODULE");
        call_module_t start_program = (call_module_t)address_of_module;
        start_program();
    }
}

int kmain(unsigned int ebx)
{
    kernel_init();
    multiboot_info_t *mbinfo = (multiboot_info_t *)ebx;
    kernel_run_module(mbinfo);
    printf("Still Working %d %i %hd %hi %hhu %hhd \r\n", 1234, -5678, (short)27, (short)-42, (unsigned char)20, (signed char)-10);
    return 145;
}
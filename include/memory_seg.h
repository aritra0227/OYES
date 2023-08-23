#ifndef INCLUDE_MEMORY_SEG_H
#define INCLUDE_MEMORY_SEG_H

//Global Descriptor Table
struct GDT
{
    unsigned short size;
    unsigned int start_address;
} __attribute__((packed)); //makes sure there are no paddings in between the fields

/**
 * Main function to call under kmain()
 * Wrapper for initializing descriptors, loading GDT and loading segment registers
*/
void install_gdt();

void set_kernel_stack(unsigned char segment, unsigned int kernel_stack_vaddr);


#endif
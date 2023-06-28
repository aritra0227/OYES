#ifndef INCLUDE_MEMORY_SEG_H
#define INCLUDE_MEMORY_SEG_H

//Global Descriptor Table
struct GDT
{
    unsigned short size;
    unsigned int start_address;
} __attribute__((packed)); //makes sure there are no paddings in between the fields

/*Descriptors for GDT
* Refer to: https://wiki.osdev.org/GDT_Tutorial 
* and table 3-8:      https://www.intel.com/content/www/us/en/architecture-and-technology/64-ia-32-architectures-software-developer-vol-3a-part-1-manual.html/
*/
struct GDT_DESCRIPTOR
{
    unsigned short limit_low; // 15:0 (low)
    unsigned short base_low; // 31:16 (low)
    unsigned char base_middle; // 7:0 (high)
    unsigned short flag_and_limit_hi; // 23:8 (high) includes G,D/B,L, AVL, Limit 19:16, P, DPL, S, Type
    unsigned char base_high; //31:24 (high)
} __attribute__((packed));


/**
 * create descriptor for gdt
*/
void create_gdt_descriptor(unsigned int index, unsigned int base, unsigned int limit, unsigned short flag);


/**
 * Main function to call under kmain()
 * Wrapper for initializing descriptors, loading GDT and loading segment registers
*/
void install_gdt();


#endif
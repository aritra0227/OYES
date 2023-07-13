
#include "memory_seg.h"
#include "gdt.h"
#include "stdio.h"

#define GDT_DESCRIPTOR_COUNT 3 // null, code, data segments all under PL0 for now
#define BASE 0
#define LIMIT 0x000FFFFF

//macros taken from https://wiki.osdev.org/GDT_Tutorial
// Each define here is for a specific flag in the descriptor.
// Refer to the intel documentation for a description of what each one does.
#define SEG_DESCTYPE(x)  ((x) << 0x04) // Descriptor type (0 for system, 1 for code/data)
#define SEG_PRES(x)      ((x) << 0x07) // Present
//note that 8,9,A,B' th indices are unfilled and hence 0s
#define SEG_SAVL(x)      ((x) << 0x0C) // Available for system use
#define SEG_LONG(x)      ((x) << 0x0D) // Long mode
#define SEG_SIZE(x)      ((x) << 0x0E) // Size (0 for 16-bit, 1 for 32)
#define SEG_GRAN(x)      ((x) << 0x0F) // Granularity (0 for 1B - 1MB, 1 for 4KB - 4GB)
#define SEG_PRIV(x)     (((x) &  0x03) << 0x05)   // Set privilege level (0 - 3)

#define SEG_CODE_EXRD      0x0A // Execute/Read
#define SEG_DATA_RDWR      0x02 // Read/Write


#define GDT_CODE_PL0 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                     SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
                     SEG_PRIV(0)     | SEG_CODE_EXRD
 
#define GDT_DATA_PL0 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                     SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
                     SEG_PRIV(0)     | SEG_DATA_RDWR

struct GDT_DESCRIPTOR GDT_ARR[GDT_DESCRIPTOR_COUNT];

void create_gdt_descriptor(unsigned int index, unsigned int base, unsigned int limit, unsigned short flag){
    GDT_ARR[index].limit_low = limit & 0xFFFF; //15:0 of limit 
    GDT_ARR[index].base_low = base & 0xFFFF; // 15:0 of base
    GDT_ARR[index].base_middle = (base >> 16) & 0xFF; //23:16 of base 
    GDT_ARR[index].flag_and_limit_hi = ((limit >> 16) & 0x0F) << 8;
    GDT_ARR[index].flag_and_limit_hi |= flag & 0xF0FF;
    GDT_ARR[index].base_high = (base >> 24) & 0xFF; //31:24 
}

void install_gdt(){
    //create the first three essential descriptors
    create_gdt_descriptor(0, BASE, 0, 0); //NULL descriptor

    //using the beginning of this address as the GDT pointer for load_gdt():
    //Note null descriptor is 8 bytes
    struct GDT* gdt = (struct GDT*)GDT_ARR; //beginning of null descriptor
    gdt->start_address = (unsigned int) GDT_ARR; // 4 bytes
    gdt->size = (sizeof(struct GDT_DESCRIPTOR)* GDT_DESCRIPTOR_COUNT) - 1; // 2 bytes

    create_gdt_descriptor(1, BASE, LIMIT, (unsigned short) (GDT_CODE_PL0)); //descriptor for code segment
    create_gdt_descriptor(2, BASE, LIMIT, (unsigned short) (GDT_DATA_PL0)); //descriptor for data segment

    load_gdt(*gdt);
    load_segment_registers();
};
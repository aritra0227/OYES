#include "memory/memory_seg.h"
#include "memory/gdt.h"
#include "stdlib/stdio.h"
#include "utility/utilities.h"

#define GDT_DESCRIPTOR_COUNT 6 // null, code, data segments for PL0 and PL3, tss
#define BASE 0
#define LIMIT 0x000FFFFF

//macros taken from https://wiki.osdev.org/GDT_Tutorial
// Each define here is for a specific flag in the descriptor.
// Refer to the intel documentation for a description of what each one does.
#define SEG_DESCTYPE(x) ((x) << 0x04) // Descriptor type (0 for system, 1 for code/data)
#define SEG_PRES(x) ((x) << 0x07)     // Present
//note that 8,9,A,B' th indices are unfilled and hence 0s
#define SEG_SAVL(x) ((x) << 0x0C)        // Available for system use
#define SEG_LONG(x) ((x) << 0x0D)        // Long mode
#define SEG_SIZE(x) ((x) << 0x0E)        // Size (0 for 16-bit, 1 for 32)
#define SEG_GRAN(x) ((x) << 0x0F)        // Granularity (0 for 1B - 1MB, 1 for 4KB - 4GB)
#define SEG_PRIV(x) (((x)&0x03) << 0x05) // Set privilege level (0 - 3)

#define SEG_CODE_EXRD 0x0A // Execute/Read
#define SEG_DATA_RDWR 0x02 // Read/Write

#define GDT_CODE_PL0 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                         SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) | \
                         SEG_PRIV(0) | SEG_CODE_EXRD

#define GDT_DATA_PL0 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                         SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) | \
                         SEG_PRIV(0) | SEG_DATA_RDWR

#define GDT_CODE_PL3 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                         SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) | \
                         SEG_PRIV(3) | SEG_CODE_EXRD

#define GDT_DATA_PL3 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                         SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) | \
                         SEG_PRIV(3) | SEG_DATA_RDWR

/*Descriptors for GDT
* Refer to: https://wiki.osdev.org/GDT_Tutorial 
* and table 3-8:      https://www.intel.com/content/www/us/en/architecture-and-technology/64-ia-32-architectures-software-developer-vol-3a-part-1-manual.html/
*/
struct GDT_DESCRIPTOR
{
    unsigned short limit_low;         // 15:0 (low)
    unsigned short base_low;          // 31:16 (low)
    unsigned char base_middle;        // 7:0 (high)
    unsigned short flag_and_limit_hi; // 23:8 (high) includes G,D/B,L, AVL, Limit 19:16, P, DPL, S, Type
    unsigned char base_high;          //31:24 (high)
} __attribute__((packed));

struct tss_entry_struct
{
    unsigned int prev_tss; // The previous TSS - with hardware task switching these form a kind of backward linked list.
    unsigned int esp0;     // The stack pointer to load when changing to kernel mode.
    unsigned int ss0;      // The stack segment to load when changing to kernel mode.
    // Everything below here is unused.
    unsigned int esp1; // esp and ss 1 and 2 would be used when switching to rings 1 or 2.
    unsigned int ss1;
    unsigned int esp2;
    unsigned int ss2;
    unsigned int cr3;
    unsigned int eip;
    unsigned int eflags;
    unsigned int eax;
    unsigned int ecx;
    unsigned int edx;
    unsigned int ebx;
    unsigned int esp;
    unsigned int ebp;
    unsigned int esi;
    unsigned int edi;
    unsigned int es;
    unsigned int cs;
    unsigned int ss;
    unsigned int ds;
    unsigned int fs;
    unsigned int gs;
    unsigned int ldt;
    unsigned short trap;
    unsigned short iomap_base;
} __attribute__((packed));

typedef struct tss_entry_struct tss_entry_t;
tss_entry_t tss_entry;

static struct GDT_DESCRIPTOR GDT_ARR[GDT_DESCRIPTOR_COUNT];

static void create_gdt_descriptor(unsigned int index, unsigned int base, unsigned int limit, unsigned short flag)
{
    GDT_ARR[index].limit_low = limit & 0xFFFF;        //15:0 of limit
    GDT_ARR[index].base_low = base & 0xFFFF;          // 15:0 of base
    GDT_ARR[index].base_middle = (base >> 16) & 0xFF; //23:16 of base
    GDT_ARR[index].flag_and_limit_hi = ((limit >> 16) & 0x0F) << 8;
    GDT_ARR[index].flag_and_limit_hi |= flag & 0xF0FF;
    GDT_ARR[index].base_high = (base >> 24) & 0xFF; //31:24
}

void set_kernel_stack(unsigned char segment, unsigned int kernel_stack_vaddr){
    tss_entry.esp0 = kernel_stack_vaddr;
    tss_entry.ss0 = segment;
}

extern void flush_tss(void);
void install_gdt()
{
    //create the first three essential descriptors
    create_gdt_descriptor(0, BASE, 0, 0); //NULL descriptor

    //using the beginning of this address as the GDT pointer for load_gdt():
    //Note null descriptor is 8 bytes
    struct GDT *gdt = (struct GDT *)GDT_ARR;                                //beginning of null descriptor
    gdt->start_address = (unsigned int)GDT_ARR;                             // 4 bytes
    gdt->size = (sizeof(struct GDT_DESCRIPTOR) * GDT_DESCRIPTOR_COUNT) - 1; // 2 bytes

    create_gdt_descriptor(1, BASE, LIMIT, (unsigned short)GDT_CODE_PL0); //descriptor for code segment
    create_gdt_descriptor(2, BASE, LIMIT, (unsigned short)GDT_DATA_PL0); //descriptor for data segment

    //segments for user mode:
    create_gdt_descriptor(3, BASE, LIMIT, (unsigned short)GDT_CODE_PL3);
    create_gdt_descriptor(4, BASE, LIMIT, (unsigned short)GDT_DATA_PL3);

    //set up tss
    create_gdt_descriptor(5, (unsigned int)(&tss_entry), sizeof(tss_entry), (unsigned short)0x89);
    memset(&tss_entry, 0, sizeof(tss_entry));

    load_gdt(*gdt);
    load_segment_registers();
    flush_tss();
};
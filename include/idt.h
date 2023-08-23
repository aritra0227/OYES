#ifndef INCLUDE_IDT_H
#define INCLUDE_IDT_H

//Interrupt Descriptor Table
struct IDT
{
    unsigned short size;
    unsigned int start_address;
} __attribute__((packed));

//Reference: https://wiki.osdev.org/Interrupt_Descriptor_Table: See IA-32
struct IDT_DESCRIPTOR
{
    unsigned short base_low;         // offset bits 0->15
    unsigned short segment_selector; // a code segment selector in GDT or LDT
    unsigned char zero_and_reserved; // unused, set to 0
    unsigned char flags;             // gate type, dpl, and p fields
    unsigned short base_high;        // offset bits 16->31
}__attribute__((packed));

/**
 * Creates an entry in the IDT
 * @param index: Index in the IDT_ARR
 * @param base: Base address of handler
 * Note: Currently assumes the following:
 *  Every interrupt Handler is a trap handler
 *  Priviledge level is set to 0
*/
void create_idt_descriptor(unsigned int index, unsigned int base);

/* 
* Installs idt and fills the table with respective interrupt handlers
*/
void install_idt();
#endif
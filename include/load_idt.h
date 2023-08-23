#ifndef INCLUDE_LOAD_IDT_H
#define INCLUDE_LOAD_IDT_H


/**
 * loads idt through assembly
 * @param address_to_idt_struct: address idt struct
 * Similar to load_gdt
*/
void load_idt(struct IDT address_to_idt_struct);

#endif

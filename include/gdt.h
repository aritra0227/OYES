#ifndef INCLUDE_GDT_H
#define INCLUDE_GDT_H
/**
 * loads gdt into the processor
 * @param: address_to_gdt_struct: takes address of gdt struct
*/
void load_gdt(struct GDT address_to_gdt_struct);

/**
 * loads segment registers
*/
void load_segment_registers();

#endif
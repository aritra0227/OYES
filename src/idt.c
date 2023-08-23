#include "idt.h"
#include "stdio.h"
#include "load_idt.h"
#include "interrupt_handler.h"
#include "system_interrupt_handler.h"

#define IDT_DESCRIPTOR_COUNT 256
#define TRAP_GATE_FLAGS 0x8F //generic flags used: p=1, dpl=0b00, type=0b1111 => flags=1000_1111b=0x8F, refer to: https://wiki.osdev.org/Interrupt_Descriptor_Table
#define GDT_OFFSET 0X08
#define PL0 0x0

struct IDT_DESCRIPTOR IDT_ARR[IDT_DESCRIPTOR_COUNT];

void create_idt_descriptor(unsigned int index, unsigned int base)
{
    IDT_ARR[index].base_high = (base >> 16) & 0xFFFF;
    IDT_ARR[index].base_low = base & 0xFFFF;
    IDT_ARR[index].flags = TRAP_GATE_FLAGS;
    IDT_ARR[index].zero_and_reserved = PL0;
    IDT_ARR[index].segment_selector = GDT_OFFSET;
}

void install_idt()
{
    struct IDT idt;
    idt.start_address = (unsigned int) IDT_ARR; // 4 bytes
    idt.size = (sizeof(struct IDT_DESCRIPTOR)* IDT_DESCRIPTOR_COUNT) - 1; 

    create_idt_descriptor(0, (unsigned int)&interrupt_handler_0);
    create_idt_descriptor(1, (unsigned int)&interrupt_handler_1);
    create_idt_descriptor(2, (unsigned int)&interrupt_handler_2);
    create_idt_descriptor(3, (unsigned int)&interrupt_handler_3);
    create_idt_descriptor(4, (unsigned int)&interrupt_handler_4);
    create_idt_descriptor(5, (unsigned int)&interrupt_handler_5);
    create_idt_descriptor(6, (unsigned int)&interrupt_handler_6);
    create_idt_descriptor(7, (unsigned int)&interrupt_handler_7);
    create_idt_descriptor(8, (unsigned int)&interrupt_handler_8);
    create_idt_descriptor(9, (unsigned int)&interrupt_handler_9);
    create_idt_descriptor(10, (unsigned int)&interrupt_handler_10);
    create_idt_descriptor(11, (unsigned int)&interrupt_handler_11);
    create_idt_descriptor(12, (unsigned int)&interrupt_handler_12);
    create_idt_descriptor(13, (unsigned int)&interrupt_handler_13);
    create_idt_descriptor(14, (unsigned int)&interrupt_handler_14);
    create_idt_descriptor(15, (unsigned int)&interrupt_handler_15);
    create_idt_descriptor(16, (unsigned int)&interrupt_handler_16);
    create_idt_descriptor(17, (unsigned int)&interrupt_handler_17);
    create_idt_descriptor(18, (unsigned int)&interrupt_handler_18);
    create_idt_descriptor(19, (unsigned int)&interrupt_handler_19);
    create_idt_descriptor(20, (unsigned int)&interrupt_handler_20);

    //pic interrupts:
    create_idt_descriptor(32, (unsigned int)&interrupt_handler_32);
    create_idt_descriptor(33, (unsigned int)&interrupt_handler_33);
    create_idt_descriptor(34, (unsigned int)&interrupt_handler_34);
    create_idt_descriptor(35, (unsigned int)&interrupt_handler_35);
    create_idt_descriptor(36, (unsigned int)&interrupt_handler_36);
    create_idt_descriptor(37, (unsigned int)&interrupt_handler_37);
    create_idt_descriptor(38, (unsigned int)&interrupt_handler_38);
    create_idt_descriptor(39, (unsigned int)&interrupt_handler_39);
    create_idt_descriptor(40, (unsigned int)&interrupt_handler_40);
    create_idt_descriptor(41, (unsigned int)&interrupt_handler_41);
    create_idt_descriptor(42, (unsigned int)&interrupt_handler_42);
    create_idt_descriptor(43, (unsigned int)&interrupt_handler_43);
    create_idt_descriptor(44, (unsigned int)&interrupt_handler_44);
    create_idt_descriptor(45, (unsigned int)&interrupt_handler_45);
    create_idt_descriptor(46, (unsigned int)&interrupt_handler_46);
    create_idt_descriptor(47, (unsigned int)&interrupt_handler_47);

    load_idt(idt);
}
#ifndef INCLUDE_PIC_H
#define INCLUDE_PIC_H

// Refer to https://sigops.acm.illinois.edu/old/roll_your_own/i386/irq.html

/* The PIC interrupts have been remapped */
#define PIC1_START_INTERRUPT 0x20
#define PIC2_START_INTERRUPT 0x28
#define PIC2_END_INTERRUPT PIC2_START_INTERRUPT + 7

#define KEYBOARD_INTERRUPT_NUMBER PIC1_START_INTERRUPT+1


void install_pic(void);
void mask_interrupts(unsigned char mask_1, unsigned char mask_2);
void pic_acknowledge(unsigned int interrupt);

#endif
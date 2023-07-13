#ifndef INCLUDE_PIC_H
#define INCLUDE_PIC_H

// Refer to https://sigops.acm.illinois.edu/old/roll_your_own/i386/irq.html

void install_pic(void);
void mask_interrupts(unsigned char mask_1, unsigned char mask_2);
void pic_acknowledge(unsigned int interrupt);

#endif
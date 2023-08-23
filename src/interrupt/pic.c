#include "utility/io.h"
#include "stdlib/stdio.h"
#include "interrupt/pic.h"

#define PIC1_PORT_A 0x20
#define PIC2_PORT_A 0xA0

#define PIC1_PORT_B 0X21
#define PIC2_PORT_B 0XA1

#define PIC_ACK 0x20

/**
 * Initializes PIC
 * Refer to: https://sigops.acm.illinois.edu/old/roll_your_own/i386/irq.html
*/
void install_pic(void)
{
    /* ICW1 */
    outb(PIC1_PORT_A, 0x11); /* Master port A */
    outb(PIC2_PORT_A, 0x11); /* Slave port A */

    /* ICW2: This is where the pic interrupt numbers are remapped */
    outb(PIC1_PORT_B, PIC1_START_INTERRUPT); /* Master offset of 0x20 in the IDT */
    outb(PIC2_PORT_B, PIC2_START_INTERRUPT); /* Master offset of 0x28 in the IDT */

    /* ICW3 */
    outb(PIC1_PORT_B, 0x04); /* Slaves attached to IR line 2 */
    outb(PIC2_PORT_B, 0x02); /* This slave in IR line 2 of master */

    /* ICW4 */
    outb(PIC1_PORT_B, 0x05); /* Set as master */
    outb(PIC2_PORT_B, 0x01); /* Set as slave */

    mask_interrupts(0xFD,0xFF); //only listen to keyboard
}


/**
 * only listens to interrupts where the corresponding mask bit is zero
 * @param mask_1: mask for PIC1
 * @param mask_2: mask for PIC2
 * e.g. mask_1 = 0xf8(1111_1000) and mask_2 = 0xff (1111_1111) will only make PIC listen to irqs 0,1 and 2
*/
void mask_interrupts(unsigned char mask_1, unsigned char mask_2)
{
    outb(PIC1_PORT_B, mask_1); /* master PIC */
    outb(PIC2_PORT_B, mask_2); /* slave PIC */
}

/** pic_acknowledge:
     *  Acknowledges an interrupt from either PIC 1 or PIC 2.
     *
     *  @param num The number of the interrupt
     */
void pic_acknowledge(unsigned int interrupt)
{
    if (interrupt < PIC1_START_INTERRUPT || interrupt > PIC2_END_INTERRUPT)
    {
        printf("invalid interrupt number: Does not meet pic range");
        return;
    }

    if (interrupt < PIC2_START_INTERRUPT)
    {
        outb(PIC1_PORT_A, PIC_ACK);
    }
    else
    {
        outb(PIC2_PORT_A, PIC_ACK);
    }
}
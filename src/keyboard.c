#include "io.h"
#include "pic.h"
#include "stdio.h"
#include "interrupt_handler.h"
#include "keyboard.h"

#define KBD_DATA_PORT 0x60
#define KBD_BUFFER_SIZE 1000

#define KBD_UNUSED_SCANCODE -1

/* Define Scancodes */

//Numbers 0-9:
#define KBD_1 0x02
#define KBD_2 0X03
#define KBD_3 0x04
#define KBD_4 0x05
#define KBD_5 0x06
#define KBD_6 0x07
#define KBD_7 0x08
#define KBD_8 0x09
#define KBD_9 0x0a
#define KBD_0 0x0b

//Letters:
#define KBD_A 0x1e
#define KBD_S 0x1f
#define KBD_D 0x20
#define KBD_F 0x21
#define KBD_G 0x22
#define KBD_H 0x23
#define KBD_J 0x24
#define KBD_K 0x25
#define KBD_L 0x26

#define KBD_Q 0x10
#define KBD_W 0x11
#define KBD_E 0x12
#define KBD_R 0x13
#define KBD_T 0x14
#define KBD_Y 0x15
#define KBD_U 0x16
#define KBD_I 0x17
#define KBD_O 0x18
#define KBD_P 0X19

#define KBD_Z 0x2c
#define KBD_X 0x2d
#define KBD_C 0x2e
#define KBD_V 0x2f
#define KBD_B 0x30
#define KBD_N 0x31
#define KBD_M 0x32

//symbols,punchuation characters
#define KBD_hyphen 0x0c
#define KBD_equals 0x0d
#define KBD_colon 0x27
#define KBD_quote 0x28
#define KBD_LSQ_BRACKET 0x1a
#define KBD_RSQ_BRACKET 0x1b
#define KBD_comma 0x33
#define KBD_period 0x34
#define KBD_forward_slash 0x35
#define KBD_SPACE 0x39

//special codes
#define KBD_BACKSPACE 0x0e
#define KBD_LSHIFT 0x2a
#define KBD_RSHIFT 0x36
#define KBD_CAPS 0x3a

#define RELEASE(x) x + 0x80

#define KBD_LSHIFT_RELEASE RELEASE(KBD_LSHIFT)
#define KBD_RSHIFT_RELEASE RELEASE(KBD_RSHIFT)

static unsigned char is_shift_key_on = 0;
static unsigned char is_caps_lock_on = 0;

//ring buffer for storing scancodes
struct RING_BUFFER
{
    unsigned char buffer[KBD_BUFFER_SIZE];
    unsigned char *head;
    unsigned char *tail;
    unsigned int count;
};

static struct RING_BUFFER kbd_scancode_buffer;
static struct RING_BUFFER kbd_char_buffer;

unsigned char keyboard_convert_scancode_to_ascii(unsigned char scancode);
void keyboard_read(void);
void keyboard_write(void);

/** keyboard_read_scan_code:
     *  Reads a scan code from the keyboard
     *
     *  @return The scan code (NOT an ASCII character!)
     */
unsigned char keyboard_read_scan_code(void)
{
    return inb(KBD_DATA_PORT);
}

/**
 * Interrupt handler for keyboard key press & release
 * Populates buffer with scancodes
*/
void keyboard_interrupt_handler(struct CPU_STATE cpu, struct INTERRUPT_INFO interrupt_info, struct STACK_STATE stack)
{
    (void)cpu, (void)interrupt_info, (void)stack;
    if (kbd_scancode_buffer.count < KBD_BUFFER_SIZE)
    {
        *kbd_scancode_buffer.tail = keyboard_read_scan_code();
        kbd_scancode_buffer.tail++;
        kbd_scancode_buffer.count++;
        if (kbd_scancode_buffer.tail == kbd_scancode_buffer.buffer + KBD_BUFFER_SIZE)
            kbd_scancode_buffer.tail = kbd_scancode_buffer.buffer;
        //test out if the interrupt is handled:
        keyboard_read();
        keyboard_write();
    }
    pic_acknowledge(KEYBOARD_INTERRUPT_NUMBER);
    return;
}

/**
 * Should be called under kmain.
 * Initializes keyboard
*/
void install_keyboard(void)
{
    assign_interrupt_handler(KEYBOARD_INTERRUPT_NUMBER, keyboard_interrupt_handler);
    //scancode buffer
    kbd_scancode_buffer.head = kbd_scancode_buffer.buffer;
    kbd_scancode_buffer.tail = kbd_scancode_buffer.buffer;
    kbd_scancode_buffer.count = 0;

    //character buffer
    kbd_char_buffer.head = kbd_char_buffer.buffer;
    kbd_char_buffer.tail = kbd_char_buffer.buffer;
    kbd_char_buffer.count = 0;
    return;
}

/**
 * Reads keyboard_scancode_buffer and populates keyboard_char_buffer
*/
void keyboard_read(void)
{
    char ch;
    if (kbd_scancode_buffer.count != 0)
    {
        ch = keyboard_convert_scancode_to_ascii(*kbd_scancode_buffer.head);
        kbd_scancode_buffer.head++; //increment head regardless of successful convertion to ascii
        if (kbd_scancode_buffer.head == kbd_scancode_buffer.buffer + KBD_BUFFER_SIZE)
            kbd_scancode_buffer.head = kbd_scancode_buffer.buffer;
        kbd_scancode_buffer.count--;
        if ((ch != KBD_UNUSED_SCANCODE) && (kbd_char_buffer.count < KBD_BUFFER_SIZE))
        {
            *kbd_char_buffer.tail = ch;
            kbd_char_buffer.tail++;
            kbd_char_buffer.count++;
            if (kbd_char_buffer.tail == kbd_char_buffer.buffer + KBD_BUFFER_SIZE)
                kbd_char_buffer.tail = kbd_char_buffer.buffer;
        }
    }
    return;
}

/**
 * Writes content of the kbd_char_buffer onto the screen via putchar
*/
void keyboard_write(void)
{
    if (kbd_char_buffer.count != 0)
    {
        // putchar(*kbd_char_buffer.head, 0); //comment out for now so the buffer is simply flushed
        kbd_char_buffer.head++;
        if (kbd_char_buffer.head == kbd_char_buffer.buffer + KBD_BUFFER_SIZE)
            kbd_char_buffer.head = kbd_char_buffer.buffer;
        kbd_char_buffer.count--;
    }
}

/**
 * Converts a given scancode to ascii character
 * @param: scancode
*/
unsigned char keyboard_convert_scancode_to_ascii(unsigned char scancode)
{
    unsigned char ascii_ch = KBD_UNUSED_SCANCODE;
    switch (scancode)
    {
        /*Special scancodes:*/
    case KBD_LSHIFT:
    case KBD_RSHIFT:
        is_shift_key_on = 1;
        break;
    case KBD_CAPS:
        is_caps_lock_on = is_caps_lock_on ? 0 : 1;
    case (KBD_LSHIFT_RELEASE):
    case (KBD_RSHIFT_RELEASE):
        is_shift_key_on = 0;
        break;
    case KBD_1:
        ascii_ch = is_shift_key_on ? '!' : '1';
        break;
    case KBD_2:
        ascii_ch = is_shift_key_on ? '@' : '2';
        break;
    case KBD_3:
        ascii_ch = is_shift_key_on ? '#' : '3';
        break;
    case KBD_4:
        ascii_ch = is_shift_key_on ? '$' : '4';
        break;
    case KBD_5:
        ascii_ch = is_shift_key_on ? '%' : '5';
        break;
    case KBD_6:
        ascii_ch = is_shift_key_on ? '^' : '6';
        break;
    case KBD_7:
        ascii_ch = is_shift_key_on ? '&' : '7';
        break;
    case KBD_8:
        ascii_ch = is_shift_key_on ? '*' : '8';
        break;
    case KBD_9:
        ascii_ch = is_shift_key_on ? '(' : '9';
        break;
    case KBD_0:
        ascii_ch = is_shift_key_on ? ')' : '0';
        break;
    case KBD_A:
        ascii_ch = (is_shift_key_on || is_caps_lock_on) ? 'A' : 'a';
        break;
    case KBD_B:
        ascii_ch = (is_shift_key_on || is_caps_lock_on) ? 'B' : 'b';
        break;
    case KBD_C:
        ascii_ch = (is_shift_key_on || is_caps_lock_on) ? 'C' : 'c';
        break;
    case KBD_D:
        ascii_ch = (is_shift_key_on || is_caps_lock_on) ? 'D' : 'd';
        break;
    case KBD_E:
        ascii_ch = (is_shift_key_on || is_caps_lock_on) ? 'E' : 'e';
        break;
    case KBD_F:
        ascii_ch = (is_shift_key_on || is_caps_lock_on) ? 'F' : 'f';
        break;
    case KBD_G:
        ascii_ch = (is_shift_key_on || is_caps_lock_on) ? 'G' : 'g';
        break;
    case KBD_H:
        ascii_ch = (is_shift_key_on || is_caps_lock_on) ? 'H' : 'h';
        break;
    case KBD_I:
        ascii_ch = (is_shift_key_on || is_caps_lock_on) ? 'I' : 'i';
        break;
    case KBD_J:
        ascii_ch = (is_shift_key_on || is_caps_lock_on) ? 'J' : 'j';
        break;
    case KBD_K:
        ascii_ch = (is_shift_key_on || is_caps_lock_on) ? 'K' : 'k';
        break;
    case KBD_L:
        ascii_ch = (is_shift_key_on || is_caps_lock_on) ? 'L' : 'l';
        break;
    case KBD_M:
        ascii_ch = (is_shift_key_on || is_caps_lock_on) ? 'M' : 'm';
        break;
    case KBD_N:
        ascii_ch = (is_shift_key_on || is_caps_lock_on) ? 'N' : 'n';
        break;
    case KBD_O:
        ascii_ch = (is_shift_key_on || is_caps_lock_on) ? 'O' : 'o';
        break;
    case KBD_P:
        ascii_ch = (is_shift_key_on || is_caps_lock_on) ? 'P' : 'p';
        break;
    case KBD_Q:
        ascii_ch = (is_shift_key_on || is_caps_lock_on) ? 'Q' : 'q';
        break;
    case KBD_R:
        ascii_ch = (is_shift_key_on || is_caps_lock_on) ? 'R' : 'r';
        break;
    case KBD_S:
        ascii_ch = (is_shift_key_on || is_caps_lock_on) ? 'S' : 's';
        break;
    case KBD_T:
        ascii_ch = (is_shift_key_on || is_caps_lock_on) ? 'T' : 't';
        break;
    case KBD_U:
        ascii_ch = (is_shift_key_on || is_caps_lock_on) ? 'U' : 'u';
        break;
    case KBD_V:
        ascii_ch = (is_shift_key_on || is_caps_lock_on) ? 'V' : 'v';
        break;
    case KBD_W:
        ascii_ch = (is_shift_key_on || is_caps_lock_on) ? 'W' : 'w';
        break;
    case KBD_X:
        ascii_ch = (is_shift_key_on || is_caps_lock_on) ? 'X' : 'x';
        break;
    case KBD_Y:
        ascii_ch = (is_shift_key_on || is_caps_lock_on) ? 'Y' : 'y';
        break;
    case KBD_Z:
        ascii_ch = (is_shift_key_on || is_caps_lock_on) ? 'Z' : 'z';
        break;
    case KBD_SPACE:
        ascii_ch = ' ';
        break;
    default:
        ascii_ch = KBD_UNUSED_SCANCODE;
        break;
    }
    return ascii_ch;
}
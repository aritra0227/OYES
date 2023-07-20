#include "io.h"

#define VADDR_OFFSET    0xC0000000

/* The I/O ports */
#define FB_COMMAND_PORT 0x3D4
#define FB_DATA_PORT 0x3D5

/* The I/O port commands */
#define FB_HIGH_BYTE_COMMAND 14
#define FB_LOW_BYTE_COMMAND 15

/* framebuffer memory */
char *fb = (char *) (0x000B8000 + 0xC0000000);

/* colours */
#define FB_GREEN     2
#define FB_DARK_GREY 8
unsigned int fb_cursor_pos = 0;


/** fb_move_cursor:
     *  Moves the cursor of the framebuffer to the given position
     *
     *  @param pos The new position of the cursor
     */
void fb_move_cursor(unsigned short pos)
{
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
    outb(FB_DATA_PORT, ((pos >> 8) & 0x00FF));
    outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
    outb(FB_DATA_PORT, pos & 0x00FF);
}

/** fb_write_cell:
     *  Writes a character with the given foreground and background to position i
     *  in the framebuffer.
     *
     *  @param i  The location in the framebuffer
     *  @param c  The character
     *  @param fg The foreground color
     *  @param bg The background color
     */
void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg)
{
    fb[i] = c;
    fb[i + 1] = ((fg & 0x0F) << 4) | (bg & 0x0F);
}

/**
 fb_write function:
 * @param buf content to write to framebuffer
 * @param len length of content
*/

int fb_write(char *buf, unsigned int buf_len)
{
    unsigned int pos = fb_cursor_pos;
    for(unsigned int i= 0; i < buf_len; ++i){
        pos = fb_cursor_pos * 2;
        fb_cursor_pos += 1; 
        fb_write_cell(pos, buf[i], FB_GREEN, FB_DARK_GREY);
    }
    fb_move_cursor(fb_cursor_pos);
    return 0;
}
#include "../display/fb.c"
#include "../hardware/serial.c"
#include "stdio.h"
#include "x86.h"
#include "utilities.h"

#define PRINT_FB 0
#define PRINT_SERIAL 1

/**
 * Wrapper for printf,
 * @param c: char to output
 * @param mode: PRINT_FB(0) to output on framebuffer
 *              PRINT_SERIAL(1) to output on serial port
*/
void putchar(char c, unsigned int mode)
{
    // dummy way to cast char to string:
    char c_str[2] = {c, '\0'};
    if (mode == PRINT_FB)
    {
        fb_write(c_str, 1);
    }
    else
    {
        serial_write(c_str);
    }
}

unsigned int str_length(char *buf)
{
    unsigned int count = 0;
    while (buf[count])
    {
        count += 1;
    }
    return count;
}

/**
 * aprintf: currently dummy wrapper to output characters on either serial port or framebuffer
 * @param c: characters to output
 * @param channel: 0: framebuffer
 *                 1: serial port
*/
void aprintf(char *c, int channel)
{
    if (channel == PRINT_FB)
    {
        fb_write(c, str_length(c));
    }
    else
    {
        serial_write(c);
    }
}

/*
* Should mimic printf from stdlib
*/
#define PRINTF_STATE_NORMAL 0
#define PRINTF_STATE_LENGTH 1
#define PRINTF_STATE_LENGTH_SHORT 2
#define PRINTF_STATE_LENGTH_LONG 3
#define PRINTF_STATE_SPECIFIER 4

#define PRINTF_LENGTH_DEFAULT 0
#define PRINTF_LENGTH_SHORT_SHORT 1
#define PRINTF_LENGTH_SHORT 2
#define PRINTF_LENGTH_LONG 3
#define PRINTF_LENGTH_LONG_LONG 4

int *printf_number(int *argp, int length, int sign, int radix);

void printf(const char *fmt, ...)
{
    int *argp = (int *)&fmt;
    int state = PRINTF_STATE_NORMAL;
    int length = PRINTF_LENGTH_DEFAULT;
    int radix = 10;
    int sign = 0; //treat as boolean
    argp++;

    while (*fmt)
    {
        switch (state)
        {
        case PRINTF_STATE_NORMAL:
            switch (*fmt)
            {
            case '%':
                state = PRINTF_STATE_LENGTH;
                break;
            default:
                putchar(*fmt, 0);
                break;
            }
            break;
        case PRINTF_STATE_LENGTH:
            switch (*fmt)
            {
            case 'h':
                length = PRINTF_LENGTH_SHORT;
                state = PRINTF_STATE_LENGTH_SHORT;
                break;
            case 'l':
                length = PRINTF_LENGTH_LONG;
                state = PRINTF_STATE_LENGTH_LONG;
                break;
            default:
                goto PRINTF_STATE_SPECIFIER_;
            }
        break;

        case PRINTF_STATE_LENGTH_SHORT:
            switch (*fmt)
            {
            case 'h':
                length = PRINTF_LENGTH_SHORT_SHORT;
                state = PRINTF_STATE_SPECIFIER;
                break;
            default:
                goto PRINTF_STATE_SPECIFIER_;
            }

        case PRINTF_STATE_LENGTH_LONG:
            switch (*fmt)
            {
            case 'l':
                length = PRINTF_LENGTH_LONG_LONG;
                state = PRINTF_STATE_SPECIFIER;
                break;
            default:
                goto PRINTF_STATE_SPECIFIER_;
                break;
            }

        case PRINTF_STATE_SPECIFIER:
        PRINTF_STATE_SPECIFIER_:
            switch (*fmt)
            {
            case 'c':
                putchar((char)*argp, 0);
                argp++;
                break;
            case 's':
                fb_write(*(char **)argp, str_length(*(char **)argp));
                argp++;
                break;

            case '%':
                putchar('%', 0);
                break;

            //for now simply handle base 10;
            case 'd':
            case 'i':
                radix = 10;
                sign = 1;
                argp = printf_number(argp, length, sign, radix);
                break;

            case 'u':
                radix = 10;
                sign = 0;
                argp = printf_number(argp, length, sign, radix);
                break;

            default:
                break;
            }
            state = PRINTF_STATE_NORMAL;
            length = PRINTF_LENGTH_DEFAULT;
            radix = 10;
            sign = 0;
            break;
        }
        fmt++;
    }
}
//
const char g_Chars[] = "0123456789";
int *printf_number(int *argp, int length, int sign, int radix)
{   
    char buffer[32];
    unsigned long long number;
    int number_sign = 1;
    int pos = 0;

    //first process length
    switch (length)
    {
    case PRINTF_LENGTH_SHORT_SHORT:
    case PRINTF_LENGTH_SHORT:
    case PRINTF_LENGTH_DEFAULT:
        if (sign == 1)
        {
            int n = *argp;
            if (n < 0)
            {
                n = -n;
                number_sign = -1;
            }
            number = (unsigned long long)n;
        }
        else
        {
            number = *(unsigned int *)argp;
        }
        argp++;
        break;

    case PRINTF_LENGTH_LONG:
        if (sign == 1)
        {
            long int n = *(long int *)argp;
            if (n < 0)
            {
                n = -n;
                number_sign = -1;
            }
            number = (unsigned long long)n;
        }
        else
        {
            number = *(unsigned long int *)argp;
        }
        argp += 2;
        break;

    case PRINTF_LENGTH_LONG_LONG:
        if (sign == 1)
        {
            long long int n = *(long long int *)argp;
            if (n < 0)
            {
                n = -n;
                number_sign = -1;
            }
            number = (unsigned long long)n;
        }
        else
        {
            number = *(unsigned long long *)argp;
        }
        argp += 4;
        break;
    }

    do
    {
        unsigned int remainder = number - divideUnsigned(number, radix) * radix;
        number = divideUnsigned(number, radix);
        buffer[pos++] = g_Chars[remainder];
        (void)radix;
        // putchar('a', 0);
    } while (number > 0);

    //now add the sign
    if (sign == 1 && number_sign < 0)
    {
        buffer[pos++] = '-';
    }

    //remember the buffer contains the elements in reverse order
    while (--pos >= 0)
    {   
        putchar(buffer[pos], 0);
    }
    return argp;
}
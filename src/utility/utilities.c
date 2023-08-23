#include "utility/utilities.h"

unsigned int divideUnsigned(unsigned int dividend, unsigned int divisor)
{
    unsigned int quotient = 0;
    unsigned int temp_divisor = divisor;
    unsigned int temp_quotient = 1;

    if (divisor == 0)
    {
        return 0;
    }

    while (temp_divisor < dividend && temp_divisor < (1U << 31))
    {
        temp_divisor <<= 1;
        temp_quotient <<= 1;
    }

    while (dividend >= divisor)
    {
        while (dividend < temp_divisor)
        {
            temp_divisor >>= 1;
            temp_quotient >>= 1;
        }
        dividend -= temp_divisor;
        quotient += temp_quotient;
    }

    return quotient;
}

/**
 * Returns of the ceil of a number
 * If dividend%divisor = 0, then returns dividend/divisor + 1 */
unsigned int div_absolute_ceil(unsigned int dividend, unsigned int divisor)
{
    return ((dividend + divisor - 1) / divisor + 1);
}

/**
 * Returns of the ceil of a number
*/
unsigned int div_ceil(unsigned int dividend, unsigned int divisor)
{
    return ((dividend - 1) / divisor + 1);
}

/*dummy division function*/
unsigned int slowDivide(unsigned int dividend, unsigned int divisor)
{
    unsigned int quotient = 0;
    while (dividend >= divisor)
    {
        dividend -= divisor;
        quotient++;
    }
    return quotient;
}

unsigned int modulo(unsigned int dividend, unsigned int divisor)
{
    return dividend - (divisor * (dividend / divisor));
}

unsigned int align_up(unsigned int n, unsigned int a)
{
    unsigned int m = n % a;
    if (m == 0)
    {
        return n;
    }
    return n + (a - m);
}

unsigned int align_down(unsigned int n, unsigned int a)
{
    return n - (n % a);
}

void *memset(void *buffer, const unsigned char byte, const unsigned int len)
{
    unsigned char *ptr = (unsigned char *)buffer;

    for (unsigned int i = 0; i < len; i++)
        ptr[i] = byte;

    return buffer;
}

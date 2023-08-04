#include "utilities.h"

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
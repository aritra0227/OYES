#include "user_interrupt.h"

int main(void)
{
    interrupt();
    interrupt();
    while (10)
    {
    }
    return 20;
}
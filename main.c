#include <stdio.h>

#include <stdint.h>
#include <stdlib.h>

#include <time.h>
#include "uuid.h"


void print_bits(UUID num)
{
    int bits = sizeof(num) * 8;

    for (int i = bits - 1; i >= 0; i--)
    {
        UUID l = 1;
        putchar((num & (l << i)) ? '1' :
                                   '0');
    }

    putchar('\n');
}


int main(void)
{
    srandom(time(NULL));

    for (int i = 0; i < 128; ++i)
    {
        UUID r = uuid4();

        UUIDx0 u = uuids(r);

        print_bits(NIL_UUID);
    }

    return 0;
}

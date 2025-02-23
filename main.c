#include <stdio.h>

#include <stdint.h>
#include <stdlib.h>

#include <time.h>
#include "uuid.h"

/*
void print_bits(UUID num)
{
    int bits = sizeof(num) * 8;

    for (int i = bits - 1; i >= 0; i--)
    {
        int k = i+1;

        if (k % 32 == 0 && k != 128)
        {
            putchar('-');
        }

//        if (i >= 48 && i <= 51)
//        {
//            putchar('x');
//        }
//        else if (i == 64 || i == 65)
//        {
//            putchar('y');
//        }
//        else
        {
            UUID l = 1;
            putchar((num & (l << i)) ? '1' :
                                       '0');
        }
    }

    putchar('\n');
}
*/

// TODO: figure out endianness, try printing a string and compare to
//  .
//  00000000-0000-4000-8000-000000000000
//  00000000-0000-4000-9000-000000000000
//  00000000-0000-4000-A000-000000000000
//  00000000-0000-4000-B000-000000000000
//  xxxxxxxx-xxxx-Mxxx-Nxxx-xxxxxxxxxxxx
//                ^    ^
//                |    |
//  correct locations marked

int main(void)
{
    UUID r;
    uuid4(r);


    UUIDs s;
    uuids(s,
          r);

    UUID r2;
    uuidrs(r2,
           s);

    UUIDi j = uuidi(r);
    UUIDi k = uuidi(r2);

    VariantField var_f = uuid_var(r);
    VersionField ver_f = uuid_ver(r);

    printf("%s\n%02X\n%02X\n",
           s,
           var_f,
           ver_f);

    return 0;
}

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "uuid.h"
#include "sha1.h"

// TODO: test passing NULL to any array-type parameter for uuid funcs


int main(void)
{
    MessageDigest digest;

    int res = hash_sha1(digest,
                        NULL);

    for(int i = 0; i < 20 ; ++i)
    {
        printf("%02X",
               digest[i]);
    }

    return res;
}

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "uuid.h"


int main(void)
{
    for (int i = 0; i < 5000; ++i)
    {
        UUID uuid = { 0 };
        UUIDs urn = { 0 };

        if (uuid7(uuid))
        {
            return EXIT_FAILURE;
        }

        if (uuids(urn,
                  uuid))
        {
            return EXIT_FAILURE;
        }

        VariantField var = uuid_var(uuid);

        printf("%04d: %s\n",
               i,
               urn);
    }

    return EXIT_SUCCESS;
}

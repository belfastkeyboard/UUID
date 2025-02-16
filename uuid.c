#include <stdlib.h>
#include <stdio.h>
#include "uuid.h"


#define UUID4_VAR_AND_VER_SET_MASK ~(((UUID)1 << 48) | ((UUID)1 << 50) | ((UUID)1 << 51) | ((UUID)1 << 65))
#define UUID4_VAR_AND_VER_UNSET_MASK (((UUID)1 << 49) | ((UUID)1 << 64))


UUID uuid4(void)
{
    UUID uuid = 0;

    unsigned int temp;

    for (int i = 0; i < 5; i++) {
        temp = random() & 0x7FFFFFFF;
        uuid |= (UUID)temp << (31 * (4 - i));
    }

    uuid &= UUID4_VAR_AND_VER_SET_MASK;
    uuid |= UUID4_VAR_AND_VER_UNSET_MASK;

    return uuid;
}


UUIDx0 uuids(UUID uuid)
{
    static char string[UUID_STRLEN] = { 0 };

    u_int32_t part1 = (u_int32_t)(uuid >> 96);
    u_int16_t part2 = (u_int16_t)(uuid >> 80)&0xFFFFFFFF;
    u_int16_t part3 = (u_int16_t)(uuid >> 64)&0xFFFFFFFF;
    u_int16_t part4 = (u_int16_t)(uuid >> 48)&0xFFFFFFFF;
    u_int16_t part5 = (u_int16_t)(uuid >> 32)&0xFFFFFFFF;
    u_int32_t part6 = (u_int32_t)(uuid)&0xFFFFFFFF;

    snprintf(string,
             UUID_STRLEN,
             "%08X-%04X-%04X-%04X-%04X%08X",
             part1,
             part2,
             part3,
             part4,
             part5,
             part6);

    return string;
}

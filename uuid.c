#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/random.h>
#include <time.h>
#include "uuid.h"


#define IS_LITTLE_ENDIAN !(*(uint16_t *)"\0\xff" < 0x100)


#define S_TO_MS(seconds) (uint64_t)(seconds * 1000)
#define NS_TO_MS(nanoseconds) (uint64_t)(nanoseconds / 1000000)

#define VARIANT_FIELD_POSITION 8
#define VERSION_FIELD_POSITION 6

#define UNSET(uuid, field) uuid &= field
#define SET(uuid, field) uuid |= field

#define VARIANT_FIELD_CLEAR 0x3F
#define VERSION_FIELD_CLEAR 0x0F

#define VARIANT_FIELD_RFC4122 0x80

#define VERSION_FIELD_V4 0x40
#define VERSION_FIELD_V7 0x70


static uint64_t swap_bytes48(uint64_t c)
{
    uint64_t r = 0;

    for (int i = 0; i < 6; ++i)
    {
        r |= ((c >> (i * 8)) & 0xFF) << ((7 - i) * 8);
    }

    return r;
}

static __uint128_t swap_bytes128(__uint128_t c)
{
    __uint128_t r = 0;

    for (int i = 0; i < 16; ++i)
    {
        r |= ((c >> (i * 8)) & 0xFF) << ((15 - i) * 8);
    }

    return r;
}


static uint64_t be_epoch_ms_ts(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME,
                  &ts);

    uint64_t timestamp = S_TO_MS(ts.tv_sec) + NS_TO_MS(ts.tv_nsec);

    if (IS_LITTLE_ENDIAN)
    {
        timestamp = swap_bytes48(timestamp);
    }

    return timestamp;
}


void uuid4(UUID uuid)
{
    getrandom(uuid,
              UUID_WIDTH,
              GRND_NONBLOCK);

    UNSET(uuid[VARIANT_FIELD_POSITION], VARIANT_FIELD_CLEAR);
    UNSET(uuid[VERSION_FIELD_POSITION], VERSION_FIELD_CLEAR);

    SET(uuid[VARIANT_FIELD_POSITION], VARIANT_FIELD_RFC4122);
    SET(uuid[VERSION_FIELD_POSITION], VERSION_FIELD_V4);
}

void uuid7(UUID uuid)
{
    uint64_t ts = be_epoch_ms_ts();

    ts = (ts >> 16);

    memcpy(uuid,
           &ts,
           6);

    getrandom(uuid + 6,
              10,
              GRND_NONBLOCK);

    UNSET(uuid[VARIANT_FIELD_POSITION], VARIANT_FIELD_CLEAR);
    UNSET(uuid[VERSION_FIELD_POSITION], VERSION_FIELD_CLEAR);

    SET(uuid[VARIANT_FIELD_POSITION], VARIANT_FIELD_RFC4122);
    SET(uuid[VERSION_FIELD_POSITION], VERSION_FIELD_V7);
}


void uuids(UUIDs string,
           UUID uuid)
{
    snprintf(string, UUID_STRLEN,
             "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
             uuid[0], uuid[1], uuid[2], uuid[3],
             uuid[4], uuid[5],
             uuid[6], uuid[7],
             uuid[8], uuid[9],
             uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15]
    );
}

UUIDi uuidi(const UUID uuid)
{
    UUIDi result = *(UUIDi*)uuid;

    if (IS_LITTLE_ENDIAN)
    {
        result = swap_bytes128(result);
    }

    return result;
}

void uuidrs(UUID uuid,
            const UUIDs string)
{
    char byte[3] = { 0 };
    size_t index = 0;

    for (int i = 0; i < UUID_STRLEN; i++)
    {
        const char c = string[i];

        if (c != '-')
        {
            byte[0] = c;
            byte[1] = string[++i];

            uint8_t r = (uint8_t)strtoul(byte,
                                         NULL,
                                         16);

            uuid[index++] = r;
        }
    }
}


VariantField uuid_var(const UUID uuid)
{
    VariantField var = (uuid[VARIANT_FIELD_POSITION] >> 6) & 0x03;

    return var;
}

VersionField uuid_ver(const UUID uuid)
{
    VersionField var = (uuid[VERSION_FIELD_POSITION] >> 4) & 0x0F;

    return var;
}

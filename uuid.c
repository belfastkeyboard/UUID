#include <malloc.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/random.h>
#include <time.h>
#include "sha1.h"
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
#define VERSION_FIELD_V5 0x50
#define VERSION_FIELD_V7 0x70

#define UINT12_MAX 0xFFF


static int malformed_uuid(const UUID uuid)
{
    int result = FAILMAL;

    VariantField var = uuid_var(uuid);
    VersionField ver = uuid_ver(uuid);

    if (var == RFC4122 &&
        (ver == VERSION4 ||
        ver == VERSION7) ||
        ver == VERSION5  ||
        ver == VERSION1)
    {
        result = SUCCESS;
    }

    return result;
}

static int malformed_uuids(const UUIDs uuid)
{
    int result = FAILMAL;

    if (strlen(uuid) == 36 &&
        uuid[8] == '-' &&
        uuid[13] == '-' &&
        uuid[18] == '-' &&
        uuid[23] == '-')
    {
        result = SUCCESS;
    }

    return result;
}


static uint16_t swap_bytes16(uint16_t c)
{

    uint16_t r = 0;

    for (int i = 0; i < 2; ++i)
    {
        r |= ((c >> (i * 8)) & 0xFF) << ((1 - i) * 8);
    }

    return r;
}

static uint64_t swap_bytes48(uint64_t c)
{
    uint64_t r = 0;

    for (int i = 0; i < 6; ++i)
    {
        r |= ((c >> (i * 8)) & 0xFF) << ((7 - i) * 8);
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

    return timestamp >> 16;
}


int uuid4(UUID uuid)
{
    if (!uuid)
    {
        return FAILNUL;
    }

    ssize_t bytes = getrandom(uuid,
                              UUID_WIDTH,
                              GRND_NONBLOCK);

    UNSET(uuid[VARIANT_FIELD_POSITION], VARIANT_FIELD_CLEAR);
    UNSET(uuid[VERSION_FIELD_POSITION], VERSION_FIELD_CLEAR);

    SET(uuid[VARIANT_FIELD_POSITION], VARIANT_FIELD_RFC4122);
    SET(uuid[VERSION_FIELD_POSITION], VERSION_FIELD_V4);

    return (bytes == UUID_WIDTH) ? SUCCESS :
                                   FAILENT;
}

void print_digest(MessageDigest digest)
{
    for (int i = 0; i < 22; i++)
    {
        printf("%02X ",
               digest[i]);
    }

    puts("\n");
}

int uuid5(UUID uuid,
          UUIDs namespace,
          const char *name)
{
    if (!uuid ||
        !namespace ||
        !name)
    {
        return FAILNUL;
    }

    if (malformed_uuids(namespace))
    {
        return FAILMAL;
    }

    UUID byte_ns;
    uuidrs(byte_ns,
           namespace);

    const size_t ns_len = UUID_WIDTH;
    const size_t n_len = strlen(name);

    char *buffer = malloc(ns_len + n_len + 1);

    memcpy(buffer,
           byte_ns,
           ns_len);

    memcpy(buffer + ns_len,
           name,
           n_len);

    MessageDigest digest;
    hash_sha1(digest,
              buffer,
              ns_len + n_len);

    free(buffer);

    memcpy(uuid,
           digest,
           UUID_WIDTH);

    UNSET(uuid[VARIANT_FIELD_POSITION], VARIANT_FIELD_CLEAR);
    UNSET(uuid[VERSION_FIELD_POSITION], VERSION_FIELD_CLEAR);

    SET(uuid[VARIANT_FIELD_POSITION], VARIANT_FIELD_RFC4122);
    SET(uuid[VERSION_FIELD_POSITION], VERSION_FIELD_V5);

    return SUCCESS;
}

int uuid7(UUID uuid)
{
    static uint64_t prev_ts = 0;
    static uint16_t counter = 0;

    if (!uuid)
    {
        return FAILNUL;
    }

    uint64_t ts = be_epoch_ms_ts();

    if (ts != prev_ts)
    {
        prev_ts = ts;
        counter = 0;
    }
    else
    {
        ++counter;
    }

    if (counter > UINT12_MAX)
    {
        return FAILSRT;
    }

    uint16_t counter12 = counter;

    if (LITTLE_ENDIAN)
    {
        counter12 = swap_bytes16(counter12);
    }

    memcpy(uuid,
           &ts,
           6);

    memcpy(uuid + 6,
           &counter12,
           sizeof(counter12));

    ssize_t bytes = getrandom(uuid + 8,
                              8,
                              GRND_NONBLOCK);

    UNSET(uuid[VARIANT_FIELD_POSITION], VARIANT_FIELD_CLEAR);
    UNSET(uuid[VERSION_FIELD_POSITION], VERSION_FIELD_CLEAR);

    SET(uuid[VARIANT_FIELD_POSITION], VARIANT_FIELD_RFC4122);
    SET(uuid[VERSION_FIELD_POSITION], VERSION_FIELD_V7);

    return (bytes == 8) ? SUCCESS :
                          FAILENT;
}


int uuids(UUIDs string,
          const UUID uuid)
{
    if (!string ||
        !uuid)
    {
        return FAILNUL;
    }

    if (malformed_uuid(uuid))
    {
        return FAILMAL;
    }

    snprintf(string, UUID_STRLEN,
             "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
             uuid[0], uuid[1], uuid[2], uuid[3],
             uuid[4], uuid[5],
             uuid[6], uuid[7],
             uuid[8], uuid[9],
             uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15]
    );

    return SUCCESS;
}

int uuidrs(UUID uuid,
           const UUIDs string)
{
    int err;
    if ((err = malformed_uuids(string)))
    {
        return err;
    }

    int index = 0;
    for (int i = 0; i < UUID_STRLEN; i++)
    {
        char byte[3] = { 0 };
        char c = string[i];

        if (c == '-')
        {
            continue;
        }

        byte[0] = c;
        byte[1] = string[++i];

        uuid[index++] = strtol(byte,
                               NULL,
                               16);
    }

    return SUCCESS;
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

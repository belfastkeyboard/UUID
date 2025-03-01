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
#define VERSION_FIELD_V5 0x50
#define VERSION_FIELD_V7 0x70

#define UINT12_MAX 0xFFF

/* -------SHA-1-for-UUID5------- */
#define SHA1_CIRCULAR_SHIFT(bits, word) (((word) << (bits)) | ((word) >> (32 - (bits))))
#define MESSAGE_BLOCK_LENGTH 64
#define SHA_SUCCESS 0x00
#define SHA_NULL    0x01
#define SHA_CORRUPT 0x02
#define SHA1_HASH_LENGTH 20
typedef uint8_t MessageDigest[SHA1_HASH_LENGTH];


typedef struct SHA1Context
{
    uint32_t hash[SHA1_HASH_LENGTH / 4];
    uint32_t low;
    uint32_t high;
    int_least16_t mbi;
    uint8_t mb[MESSAGE_BLOCK_LENGTH];
    int corrupted;
} SHA1Context;

static void process_message_block(SHA1Context *context)
{
    const uint32_t k[] = {
        0x5A827999,
        0x6ED9EBA1,
        0x8F1BBCDC,
        0xCA62C1D6
    };

    uint32_t words[80];

    for(int i = 0; i < 16; i++)
    {
        words[i] = context->mb[i * 4] << 24;
        words[i] |= context->mb[i * 4 + 1] << 16;
        words[i] |= context->mb[i * 4 + 2] << 8;
        words[i] |= context->mb[i * 4 + 3];
    }

    for(int i = 16; i < 80; i++)
    {
        words[i] = SHA1_CIRCULAR_SHIFT(1,
                                       words[i - 3] ^ words[i - 8] ^ words[i - 14] ^ words[i - 16]);
    }

    uint32_t A = context->hash[0];
    uint32_t B = context->hash[1];
    uint32_t C = context->hash[2];
    uint32_t D = context->hash[3];
    uint32_t E = context->hash[4];

    for(int i = 0; i < 20; i++)
    {
        uint32_t temp = SHA1_CIRCULAR_SHIFT(5,
                                            A) + ((B & C) | ((~B) & D)) + E + words[i] + k[0];

        E = D;
        D = C;
        C = SHA1_CIRCULAR_SHIFT(30,
                                B);

        B = A;
        A = temp;
    }

    for(int i = 20; i < 40; i++)
    {
        uint32_t temp = SHA1_CIRCULAR_SHIFT(5,
                                            A) + (B ^ C ^ D) + E + words[i] + k[1];

        E = D;
        D = C;
        C = SHA1_CIRCULAR_SHIFT(30,
                                B);

        B = A;
        A = temp;
    }

    for(int i = 40; i < 60; i++)
    {
        uint32_t temp = SHA1_CIRCULAR_SHIFT(5,
                                            A) + ((B & C) | (B & D) | (C & D)) + E + words[i] + k[2];

        E = D;
        D = C;
        C = SHA1_CIRCULAR_SHIFT(30,
                                B);

        B = A;
        A = temp;
    }

    for(int i = 60; i < 80; i++)
    {
        uint32_t temp = SHA1_CIRCULAR_SHIFT(5,
                                            A) + (B ^ C ^ D) + E + words[i] + k[3];

        E = D;
        D = C;
        C = SHA1_CIRCULAR_SHIFT(30,
                                B);

        B = A;
        A = temp;
    }

    context->hash[0] += A;
    context->hash[1] += B;
    context->hash[2] += C;
    context->hash[3] += D;
    context->hash[4] += E;

    context->mbi = 0;
}


static void pad_message(SHA1Context *context)
{
    if (context->mbi > 55)
    {
        context->mb[context->mbi++] = 0x80;

        while(context->mbi < MESSAGE_BLOCK_LENGTH)
        {
            context->mb[context->mbi++] = 0;
        }

        process_message_block(context);

        while(context->mbi < 56)
        {
            context->mb[context->mbi++] = 0;
        }
    }
    else
    {
        context->mb[context->mbi++] = 0x80;

        while(context->mbi < 56)
        {
            context->mb[context->mbi++] = 0;
        }
    }

    context->mb[56] = context->high >> 24;
    context->mb[57] = context->high >> 16;
    context->mb[58] = context->high >> 8;
    context->mb[59] = context->high;

    context->mb[60] = context->low >> 24;
    context->mb[61] = context->low >> 16;
    context->mb[62] = context->low >> 8;
    context->mb[63] = context->low;

    process_message_block(context);
}


SHA1Context context_init(void)
{
    SHA1Context context = { 0 };

    context.hash[0] = 0x67452301;
    context.hash[1] = 0xEFCDAB89;
    context.hash[2] = 0x98BADCFE;
    context.hash[3] = 0x10325476;
    context.hash[4] = 0xC3D2E1F0;

    return context;
}


int result(SHA1Context *context,
           uint8_t Message_Digest[20])
{
    if (!context || !Message_Digest)
    {
        return SHA_NULL;
    }

//    if (context->corrupted)
//    {
//        return context->corrupted;
//    }

    pad_message(context);

    for(int i = 0; i < MESSAGE_BLOCK_LENGTH; ++i)
    {
        context->mb[i] = 0;
    }

    for(int i = 0; i < SHA1_HASH_LENGTH; ++i)
    {
        Message_Digest[i] = context->hash[i >> 2] >> 8 * (3 - (i & 0x03));
    }

    return SHA_SUCCESS;
}


int input(SHA1Context *context,
          const uint8_t *message,
          size_t length)
{
    int result = SHA_SUCCESS;

    if (!length)
    {
        result = SHA_SUCCESS;
    }
    else if (!message)
    {
        result = SHA_NULL;
    }
    else
    {
        while(length-- && !context->corrupted)
        {
            context->mb[context->mbi++] = *message;

            context->low += 8;

            if (context->low == 0)
            {
                context->high++;

                if (context->high == 0)
                {
                    result = SHA_CORRUPT;

                    break;
                }
            }

            if (context->mbi == MESSAGE_BLOCK_LENGTH)
            {
                process_message_block(context);
            }

            message++;
        }
    }

    return result;
}


int hash_sha1(MessageDigest digest,
              char *message,
              const size_t length)
{
    SHA1Context context = context_init();
    int err = SHA_SUCCESS;

    if (message)
    {
        err = input(&context,
                    (const uint8_t*)message,
                    length);
    }

    if (err == SHA_SUCCESS)
    {
        err = result(&context,
                     digest);
    }

    return err;
}
/* -------SHA-1-for-UUID5------- */


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

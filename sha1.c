#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "sha1.h"


#define MESSAGE_BLOCK_LENGTH 64

#define SHA1_CIRCULAR_SHIFT(bits, word) (((word) << (bits)) | ((word) >> (32 - (bits))))


#define SHA_SUCCESS 0x00
#define SHA_NULL    0x01
#define SHA_CORRUPT 0x02


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
              const char *message)
{
    SHA1Context context = context_init();
    int err = SHA_SUCCESS;

    if (message)
    {
        const size_t len = strlen(message);

        err = input(&context,
                    (const uint8_t*)message,
                    len);
    }

    if (err == SHA_SUCCESS)
    {
        err = result(&context,
                     digest);
    }

    return err;
}

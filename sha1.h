#pragma once


#include <stdint.h>


#define SHA1_HASH_LENGTH 20


typedef uint8_t MessageDigest[SHA1_HASH_LENGTH];


int hash_sha1(MessageDigest digest,
              char *message,
              size_t length);

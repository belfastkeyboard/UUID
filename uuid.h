#pragma once

#include <stdint.h>


#define UUID_WIDTH  sizeof(__uint128_t)
#define UUID_STRLEN 37


#define RFC4122 0x02

#define VERSION1 0x01
#define VERSION2 0x02
#define VERSION3 0x03
#define VERSION4 0x04
#define VERSION5 0x05
#define VERSION6 0x06
#define VERSION7 0x07
#define VERSION8 0x08


#define NIL_UUID (UUID){ 0 }

#define MAX_UUID (UUID){ 0xFF, 0xFF, 0xFF, 0xFF, \
                         0xFF, 0xFF, 0xFF, 0xFF, \
                         0xFF, 0xFF, 0xFF, 0xFF, \
                         0xFF, 0xFF, 0xFF, 0xFF }


#define SUCCESS 0x00 // Function succeeded
#define FAILENT 0x01 // Function failed because system lacked sufficient entropy to seed random number generation
#define FAILSRT 0X02 // Function failed to provide a sortable UUID because too many were created at once
#define FAILNUL 0x03 // NULL values were passed to a function
#define FAILMAL 0x04 // UUID or its hexadecimal string representation is malformed and cannot be operated on
#define FAILLEN 0x05 // UUIDs is not of the correct length (36 characters)


// Pre-computed namespaces as defined in RFC-9562
#define NAMESPACE_DNS  "6ba7b810-9dad-11d1-80b4-00c04fd430c8"
#define NAMESPACE_URL  "6ba7b811-9dad-11d1-80b4-00c04fd430c8"
#define NAMESPACE_OID  "6ba7b812-9dad-11d1-80b4-00c04fd430c8"
#define NAMESPACE_X500 "6ba7b814-9dad-11d1-80b4-00c04fd430c8"


typedef uint8_t Byte;
typedef Byte UUID[UUID_WIDTH];
typedef __uint128_t UUIDi;
typedef char UUIDs[UUID_STRLEN];
typedef Byte VariantField;
typedef Byte VersionField;


int uuid4(UUID uuid);

int uuid5(UUID uuid,
          UUIDs namespace,
          const char *name);

int uuid7(UUID uuid);


int uuids(UUIDs string,
          const UUID uuid);

int uuidrs(UUID uuid,
           const UUIDs string);


VariantField uuid_var(const UUID uuid);

VersionField uuid_ver(const UUID uuid);

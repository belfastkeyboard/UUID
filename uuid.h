#pragma once


#define UUID_WIDTH  sizeof(__uint128_t)
#define UUID_STRLEN 37


#define RFC4122   0x02

#define VERSION4 0x04
#define VERSION7 0x07


#define NIL_UUID (UUID){ 0 }
#define MAX_UUID (UUID){ 0xFF, 0xFF, 0xFF, 0xFF, \
                         0xFF, 0xFF, 0xFF, 0xFF, \
                         0xFF, 0xFF, 0xFF, 0xFF, \
                         0xFF, 0xFF, 0xFF, 0xFF }


#define SUCCESS 0x00
#define FAILENT 0x01
#define FAILSRT 0X02
#define FAILLEN 0x03
#define FAILMAL 0x04


typedef uint8_t Byte;
typedef Byte UUID[UUID_WIDTH];
typedef __uint128_t UUIDi;
typedef char UUIDs[UUID_STRLEN];
typedef Byte VariantField;
typedef Byte VersionField;


int uuid4(UUID uuid);

int uuid7(UUID uuid);


int uuids(UUIDs string,
          const UUID uuid);


VariantField uuid_var(const UUID uuid);

VersionField uuid_ver(const UUID uuid);

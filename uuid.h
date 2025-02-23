#pragma once


#define UUID_STRLEN 37
#define UUID_WIDTH sizeof(__uint128_t)

#define RFC4122 0x02

#define VERSION4 0x04


#define NIL_UUID (UUID){ 0 }
#define MAX_UUID (UUID){ 0xFF, 0xFF, 0xFF, 0xFF, \
                         0xFF, 0xFF, 0xFF, 0xFF, \
                         0xFF, 0xFF, 0xFF, 0xFF, \
                         0xFF, 0xFF, 0xFF, 0xFF }


typedef uint8_t UUID[UUID_WIDTH];
typedef __uint128_t UUIDi;
typedef char UUIDs[UUID_STRLEN];
typedef uint8_t VariantField;
typedef uint8_t VersionField;


void uuid4(UUID uuid);

void uuid7(UUID uuid);


void uuids(UUIDs string,
           UUID uuid);

void uuidrs(UUID uuid,
            const UUIDs string);


UUIDi uuidi(const UUID uuid);


VariantField uuid_var(const UUID uuid);

VersionField uuid_ver(const UUID uuid);

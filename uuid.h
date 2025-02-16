#pragma once


#define UUID_STRLEN 37

typedef __uint128_t UUID;
typedef char *UUIDx0;

#define NIL_UUID (UUID)0
#define MAX_UUID ((UUID)0xFFFFFFFFFFFFFFFF | (UUID)0xFFFFFFFFFFFFFFFF << 64)


UUID uuid4(void);

UUIDx0 uuids(UUID uuid);

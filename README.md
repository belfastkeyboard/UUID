# UUID Generator
A lightweight C library for generating universally unique identifiers (UUIDs) according to the RFC 4122 standards.
This library supports the generation of UUID versions 4 and 7.

## Supported Versions
- **Version 4**:  Generates a UUID using a cryptographically secure random number generator.
- **Version 5**: Generates a consistent UUID by hashing a name and namespace using the SHA-1 algorithm.
- **Version 7**: Combines a UNIX timestamp and a cryptographically secure random number generator to produce sortable a UUID.

Endianness-safe: Uses arrays to avoid issues with endianness.

## Example

```C
#include <stdio.h>
#include "uuid.h"


int main(void)
{
    UUID uuid;
    uuid4(uuid);

    UUIDs hex_string;
    uuids(hex_string, uuid);

    printf("Generated UUID: %s\n", 
           hex_string);

    return 0;
}

Generated UUID: 0d5be048-2233-4031-bea0-61b36e7eb628
```

## API Reference
### Functions
```C
// generate UUID
int uuid4(UUID uuid);
int uuid5(UUID uuid, UUIDs namespace, const char *name);
int uuid7(UUID uuid);

// generate hexadecimal string from UUID
int uuids(UUIDs string, const UUID uuid);

// helper functions for determining type and version of UUID
VariantField uuid_var(const UUID uuid);
VersionField uuid_ver(const UUID uuid);
```

## Types

- ```UUID``` An array representing the 128-bit UUID.
- ```UUIDs``` An array representing the UUID in hexadecimal string format 
- ```VariantField``` Helper type for determining UUID type
- ```VersionField``` Helper type for determining UUID version

## Sources
- [A Universally Unique IDentifier (UUID) URN Namespace](https://datatracker.ietf.org/doc/html/rfc4122)
- [Universally Unique IDentifiers (UUIDs)](https://www.rfc-editor.org/rfc/rfc9562.html)
#include <stdio.h>
#include "uuid.h"

// TODO: test passing NULL to any array-type parameter for uuid funcs

#include "sha1.h"



#include <string.h>

int main(void)
{
    UUID uuid;
    uuid5(uuid,
          NAMESPACE_URL,
          "google.com");

    UUIDs string;
    uuids(string,
          uuid);

    printf("Test:   %s.\nPython: %s.\n",
           string,
           "fedb2fa3-8f5c-5189-80e6-f563dd1cb8f9");

    return 0;
}

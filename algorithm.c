#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "messages.h"

#include <openssl/sha.h>

int algoritm (uint8_t hash[], uint64_t start, uint64_t end) {

    int i;
    int j;
    int similar = 1;
    for (i = start; i < end; i++) {

        const char *s;
        sprintf(s, "%d", i);
	    unsigned char *d = SHA256(s, strlen(s), 0);
        printf("%s", s);

        for (int j = 0; j < SHA256_DIGEST_LENGTH; j++) {
            if (d[j] != hash[j]) {
                similar = 0;
            }
        }

        if (similar == 1) {
            return i;
        }
    }
}


int main (int argc, char *argv[]) {
    uint8_t hash[] = "4a44dc15364204a80fe80e9039455cc1608281820fe2b24f1e5233ade6af1dd5";
    uint64_t start = 1;
    uint64_t end = 11;

    printf("%d", algoritm(hash, start, end));
}
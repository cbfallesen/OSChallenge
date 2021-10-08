#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>

int main( int argc, char *argv[] ) {
    unsigned char dank[] = "4a44dc15364204a80fe80e9039455cc1608281820fe2b24f1e5233ade6af1dd5";
    const char *s = "10";

    unsigned char *d = SHA256(s, strlen(s), 0);
    unsigned char *dankHex = "4a44dc15364204a80fe80e9039455cc1608281820fe2b24f1e5233ade6af1dd5";

    for (int i = 0; i < 64; i++)
    {
        printf("%c", dank[i]);
    }
    printf("\n");
    for (int i = 0; i < 32; i++)
    {
        printf("%02x", d[i]);
    }

    int digitsRight = 0;
    for (int i = 0; i < 32; i++)
    {
        if(dankHex[i] == d[i]) {
            digitsRight++;
        }
    }
    
    printf("\n");
    if(digitsRight == 32) {
        printf("%s", "Equal");
    } else {
        printf("%s", "Not equal");
    }
    printf("\n");
    return 0;
}
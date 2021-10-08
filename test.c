#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>

int main( int argc, char *argv[] ) {
    unsigned char dank[] = "4a44dc15364204a80fe80e9039455cc1608281820fe2b24f1e5233ade6af1dd5";
    unsigned char arr[] = {0,0,0,0,0,0,0,11};

    uint64_t j = 0;
    for (int i = 0, k = 8; i < 8; i++, k--)
    {
        j += arr[i] << (k*8);
    }
    
    for (int i = 0; i < strlen(dank); i++)
    {
        
    }
    
    
    printf("%u\n", j );


    return 0;
}
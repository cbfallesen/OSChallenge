#include <stdio.h>
#include <stdlib.h>
#include <openssl/sha.h>

#define PACKET_REQUEST_SIZE           (SHA256_DIGEST_LENGTH + 8 + 8 + 1)
#define PACKET_REQUEST_HASH_OFFSET    0
#define PACKET_REQUEST_START_OFFSET   (SHA256_DIGEST_LENGTH)
#define PACKET_REQUEST_END_OFFSET     (SHA256_DIGEST_LENGTH + 8)
#define PACKET_REQUEST_PRIO_OFFSET    (SHA256_DIGEST_LENGTH + 8 + 8)

#define PACKET_RESPONSE_SIZE          8
#define PACKET_RESPONSE_ANSWER_OFFSET 0

int main (int argc, char *argv[]) {
    int inputPacket = argv[1];
}
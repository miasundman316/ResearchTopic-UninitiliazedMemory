#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// An example from Gemini. This code demonstrates how, by reading uninitialized
// memory, a program can leak sensitive information. In this case, the secret
// token is placed in a heap chunk and then freed. However, when the
// send_greeting function runs, the same size chunk is allocated again, and the
// allocator may return the same address, allowing the uninitialized memory to
// be read and leaked.

// To run this example, compile using "gcc -O0 example2.c -o example2" and run
// the code. You should see that the secret token is leaked in the
// output even though it was never explicitly printed and was previously freed.

void process_login(void) {
    // 1. Allocate a heap chunk
    char* session_secret = malloc(64);

    // Store the sensitive secret past the first 16 bytes so
    // allocator pointers (written to offsets 0-15 during free) don't overwrite
    // it.
    strcpy(session_secret + 16, "SUPER_SECRET_TOKEN_98765");

    printf("[Auth Service] Secret placed at: %p\n", (void*)session_secret);

    // Release back to the heap allocator (chunk is NOT zeroed out)
    free(session_secret);
}

void send_greeting(void) {
    // 2. Reallocate the exact same size chunk.
    // glibc serves this directly from the top of the tcache bin (same address).
    char* response = malloc(64);

    // Overwrite the first 16 bytes with normal greeting text without a trailing
    // null byte. That way printf("%s") flows directly into the uninitialized
    // residual payload.
    memcpy(response, "GREETINGS_USER: ", 16);

    printf("[Web Service]  Response buffer: %p\n", (void*)response);
    printf("[Web Service]  Output sent:     \"%s\"\n", response);

    free(response);
}

int main(void) {
    process_login();
    printf("--- (Simulating subsequent user request) ---\n");
    send_greeting();
    return 0;
}
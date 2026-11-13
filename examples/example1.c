#include <stdio.h>

// One of Gemini's examples (that I had to fix), this shows how a compiler set
// to optimize can accidentally strip out a security check if the code has
// uninitialized memory.

// Only token 1234 is authorized to access the system. Any other token should be
// denied access. However, due to the variable 'unauthorized' only being
// initialized when the token is correct, the compiler may optimize away the
// check (by assuming no undefined behavior occurs and thus assuming the token
// must always be 1234 so unauthorized gets initialized before read) and allow
// access for invalid tokens.

// To run this example, first compile using -00 (i.e. "gcc -O0 example1.c -o
// example1unoptimized") and run the resulting binary. You should see that
// access is denied for the invalid token.

// Then, recompile using -O2 (i.e. "gcc -O2 example1.c -o example1optimized")
// and run the resulting binary. You should see that access is granted for the
// invalid token, demonstrating how uninitialized memory can lead to security
// vulnerabilities when compiler optimizations are applied.

int check_access(int token) {
    int unauthorized;  // UNINITIALIZED: contains garbage data

    if (token == 1234) {
        unauthorized = 0;  // 0 means false, so only authorize user 1234
    }

    // If token != 1234, 'authorized' was never assigned.
    // Reading an uninitialized scalar here invokes Undefined Behavior (UB).
    if (!unauthorized) {
        return 1;  // Access Granted
    }

    return 0;  // Access Denied
}

int main(void) {
    int fake_token = 9999;
    printf("Testing with invalid token: %d\n", fake_token);

    if (check_access(fake_token)) {
        printf("RESULT: [!] Access Granted! (Security check bypassed)\n");
    } else {
        printf("RESULT: [X] Access Denied. (Normal behavior)\n");
    }

    return 0;
}
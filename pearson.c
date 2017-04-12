#include <stdio.h>
#include "pearson.h"

int main(void) {
    char t[][100] = {"a", "b", "z", "aba", "aab", "bab", "aaa",
        "a very very long vord...",
        "a very very long word..."};
    for (int i = 0; i < sizeof(t) / sizeof(t[0]); i++) {
        printf("%s: %u\n", t[i], hash128(t[i]));
        printf("%s: %u\n", t[i], hash512(t[i]));
        printf("%s: %u\n", t[i], hash1024(t[i]));
        printf("%s: %u\n", t[i], hash2048(t[i]));
    }
    return 0;
}

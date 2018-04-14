#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "scanner.h"

#define BUFSIZE 65536


int main(void) {
    char *buf = malloc(BUFSIZE);
    if (!buf)
        errx(1, "could not allocate buffer");
    size_t read = fread(buf, sizeof(char), BUFSIZE, stdin);
    if (read == 0)
        errx(1, "could not read input");
    if (!feof(stdin))
        errx(1, "input is too long");

    struct Scanner scanner;
    scanner.stream = buf;
    scanner.err_msg = "no value";
    while (scanner.curr_token != TOK_EOF) {
        if (next(&scanner) == RESULT_FAIL) {
            errx(1, "%s", scanner.err_msg);
        }
    }
    return 0;
}

#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "scanner.h"

#define BUFSIZE 65536


int main(void) {
    char buf[BUFSIZE];
    struct Scanner scanner;

    while (!feof(stdin)) {
        fgets(buf, BUFSIZE, stdin);

        scanner.stream = buf;
        scanner.err_msg = "no value";
        while (scanner.curr_token != TOK_EOF) {
            if (next(&scanner) == RESULT_FAIL) {
                errx(1, "%s", scanner.err_msg);
            }
        }
    }

    return 0;
}

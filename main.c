#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "parser.h"
#include "scanner.h"

#define BUFSIZE 65536

int main(void) {
    char buf[BUFSIZE];
    struct Scanner scanner;
    struct Parser parser;


    while (1) {
        if (fgets(buf, BUFSIZE, stdin) == NULL) {
            if (feof(stdin))
                break;
            err(1, "fgets");
        }

        char *p;
        if ((p = strchr(buf, '\n')) == NULL) {
            warnx("line too long, skipping");
            while (getchar() != '\n')
                ;
            continue;
        }
        *p = '\0';

        scanner.err_msg = NULL;
        scanner.stream = buf;
        scanner.curr_token = TOK_NONE;

        parser.err_msg = NULL;
        parser.scanner = &scanner;
        parser.curr_token = TOK_NONE;

        if (spotless_parser_parse(&parser) == RESULT_FAIL)
            errx(1, "%s", parser.err_msg);
    }
    return 0;
}

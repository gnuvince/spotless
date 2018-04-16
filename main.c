#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "parser.h"
#include "scanner.h"

#define BUFSIZE 65536

int spotless_readline(char *buf, size_t bufsize) {
    int read = 0;
    while (1) {
        if (bufsize == 0)
            break;

        int c = getchar();
        if (c == '\n') {
            *buf = 0;
            break;
        }
        *buf++ = c;
        read++;
        bufsize--;
    }
    return read;
}

int main_scan(void) {
    char buf[BUFSIZE];
    struct Scanner scanner;

    fread(buf, sizeof(char), BUFSIZE, stdin);

    scanner.err_msg = NULL;
    scanner.stream = buf;

    while (scanner.curr_token != TOK_EOF) {
        if (spotless_scanner_next(&scanner) == RESULT_FAIL)
            errx(1, "%s", scanner.err_msg);
    }
    return 0;
}

int main_parse(void) {
    char buf[BUFSIZE];
    struct Scanner scanner;
    struct Parser parser;


    while (1) {
        spotless_readline(buf, BUFSIZE);

        if (feof(stdin))
            break;

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


int main(int argc, char **argv) {
    if (argc != 2)
        errx(1, "usage: spotless <scan | parse>");

    if (strcmp(argv[1], "scan") == 0)
        return main_scan();
    else if (strcmp(argv[1], "parse") == 0)
        return main_parse();
    else
        errx(1, "usage: spotless <scan | parse>");
}

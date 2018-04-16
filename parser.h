#ifndef SPOTLESS_PARSER_H
#define SPOTLESS_PARSER_H

#include "common.h"
#include "scanner.h"

struct Parser {
    struct Scanner *scanner;
    const char *err_msg;
    enum Token curr_token;
};

enum Result spotless_parser_parse(struct Parser *);

#endif

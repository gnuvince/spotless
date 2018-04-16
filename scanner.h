#ifndef SPOTLESS_SCANNER_H
#define SPOTLESS_SCANNER_H

#include "common.h"

enum Token {
    TOK_NONE,
    TOK_EOF,

    /* Keywords */
    TOK_NULL,
    TOK_FALSE,
    TOK_TRUE,

    /* Syntax */
    TOK_LBRACE,
    TOK_RBRACE,
    TOK_LBRACK,
    TOK_RBRACK,
    TOK_COLON,
    TOK_COMMA,

    /* Literals */
    TOK_NUMBER,
    TOK_STRING,
};

struct Scanner {
    enum Token curr_token;
    char *stream;
    const char *err_msg;
};

enum Result spotless_scanner_next(struct Scanner *);

#endif

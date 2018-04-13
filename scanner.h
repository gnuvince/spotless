#ifndef SPOTLESS_SCANNER_H
#define SPOTLESS_SCANNER_H

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

enum ScanResult {
    RESULT_OK,
    RESULT_FAIL
};

struct Scanner {
    enum Token curr_token;
    char *stream;
    const char *err_msg;
};

enum ScanResult next(struct Scanner *);

#endif

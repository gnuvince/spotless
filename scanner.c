/*
 * Useful invariants to know:
 *   - next() skips blanks (using isspace()) before dispatching;
 *     scanner->stream points to the first non-blank character
 *     of the rest of the input.
 *   - after a scan_*() function successfully returns,
 *     scanner->stream points to the first character after
 *     the sub-string that generated the token.
 *   - scanner->curr_token is set *before* scanning begins.
 *     If an error occurs, you can know what kind of token was
 *     being scanned.
 */
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

#include "common.h"
#include "scanner.h"

#define DIGIT               \
         '0':               \
    case '1':               \
    case '2':               \
    case '3':               \
    case '4':               \
    case '5':               \
    case '6':               \
    case '7':               \
    case '8':               \
    case '9'

static char advance(struct Scanner *scanner) {
    return *scanner->stream++;
}

static char peek(struct Scanner *scanner) {
    return *scanner->stream;
}

static bool eof(struct Scanner *scanner) {
    return peek(scanner) == 0;
}

static void skip_spaces(struct Scanner *scanner) {
    while (isspace(peek(scanner)))
        (void) advance(scanner);
}

static enum Result scan_keyword(
    struct Scanner *scanner,
    char *keyword,
    size_t keyword_len,
    enum Token tok,
    const char *err_msg)
{
    scanner->curr_token = tok;
    if (memcmp(scanner->stream, keyword, keyword_len) == 0) {
        scanner->stream += keyword_len;
        return RESULT_OK;
    }
    else {
        scanner->err_msg = err_msg;
        return RESULT_FAIL;
    }
}

static enum Result scan_integer(struct Scanner *scanner, bool *was_zero) {
    char c = advance(scanner);
    if (c == '0') {
        *was_zero = true;
        return RESULT_OK;
    } else {
        while (isdigit(peek(scanner)))
            (void) advance(scanner);
        *was_zero = false;
        return RESULT_OK;
    }
}

static enum Result scan_frac(struct Scanner *scanner) {
    if (advance(scanner) != '.') {
        scanner->err_msg = "missing '.' in number";
        return RESULT_FAIL;
    }

    if (!isdigit(peek(scanner))) {
        scanner->err_msg = "missing fractional part";
        return RESULT_FAIL;
    }

    while (isdigit(peek(scanner)))
        (void) advance(scanner);

    return RESULT_OK;
}

static enum Result scan_exp(struct Scanner *scanner) {
    char c = advance(scanner);
    if (c != 'e' && c != 'E') {
        scanner->err_msg = "missing 'e' in number";
        return RESULT_FAIL;
    }

    c = peek(scanner);
    if (c == '+' || c == '-')
        (void) advance(scanner);

    if (!isdigit(peek(scanner))) {
        scanner->err_msg = "missing exponent part";
        return RESULT_FAIL;
    }

    while (isdigit(peek(scanner)))
        (void) advance(scanner);

    return RESULT_OK;
}


static enum Result scan_number(struct Scanner *scanner, bool negative) {
    scanner->curr_token = TOK_NUMBER;

    bool was_zero;
    if (scan_integer(scanner, &was_zero) == RESULT_FAIL)
        return RESULT_FAIL;

    char c = peek(scanner);
    if (c == '.') {
        if (scan_frac(scanner) == RESULT_FAIL)
            return RESULT_FAIL;
        was_zero = false;
    }

    c = peek(scanner);
    if (c == 'E' || c == 'e') {
        if (scan_exp(scanner) == RESULT_FAIL)
            return RESULT_FAIL;
        was_zero = false;
    }

    if (was_zero && negative) {
        scanner->err_msg = "zero cannot be negative";
        return RESULT_FAIL;
    }

    return RESULT_OK;
}

static bool ishexdigit(char c) {
    return (c >= '0' && c <= '9')
        || (c >= 'a' && c <= 'f')
        || (c >= 'A' && c <= 'F');
}

static enum Result scan_four_hex(struct Scanner *scanner) {
    if (!ishexdigit(advance(scanner))) return RESULT_FAIL;
    if (!ishexdigit(advance(scanner))) return RESULT_FAIL;
    if (!ishexdigit(advance(scanner))) return RESULT_FAIL;
    if (!ishexdigit(advance(scanner))) return RESULT_FAIL;
    return RESULT_OK;
}

static enum Result scan_escape_sequence(struct Scanner *scanner) {
    switch (peek(scanner)) {
    case '"':
    case '\\':
    case '/':
    case 'b':
    case 'f':
    case 'n':
    case 'r':
    case 't':
        (void) advance(scanner);
        return RESULT_OK;
    case 'u':
        (void) advance(scanner);
        if (scan_four_hex(scanner) == RESULT_FAIL) {
            scanner->err_msg = "invalid unicode escape sequence";
            return RESULT_FAIL;
        }
        else {
            return RESULT_OK;
        }
    default:
        scanner->err_msg = "invalid escape sequence";
        return RESULT_FAIL;
    }
}

static enum Result scan_string(struct Scanner *scanner) {
    scanner->curr_token = TOK_STRING;
    if (advance(scanner) != '"') {
        scanner->err_msg = "expected opening '\"'";
        return RESULT_FAIL;
    }

    char c;
    while (!eof(scanner) && (c = advance(scanner)) != '"') {
        if (c == '\\' && scan_escape_sequence(scanner) == RESULT_FAIL) {
            return RESULT_FAIL;
        }
    }

    if (c != '"') {
        scanner->err_msg = "expected closing '\"'";
        return RESULT_FAIL;
    }

    return RESULT_OK;
}

static enum Result scan_punctuation(struct Scanner *scanner, enum Token tok) {
    (void) advance(scanner);
    scanner->curr_token = tok;
    return RESULT_OK;
}

enum Result spotless_scanner_next(struct Scanner *scanner) {
    skip_spaces(scanner);
    switch (peek(scanner)) {
    case 0:
        if (scanner->curr_token == TOK_NONE) {
            scanner->err_msg = "no token";
            return RESULT_FAIL;
        }
        scanner->curr_token = TOK_EOF;
        return RESULT_OK;

    case '"':
        return scan_string(scanner);

    case DIGIT:
        return scan_number(scanner, false);

    case 't':
        return scan_keyword(scanner, "true", 4, TOK_TRUE, "error reading 'true'");

    case 'f':
        return scan_keyword(scanner, "false", 5, TOK_FALSE, "error reading 'false'");

    case 'n':
        return scan_keyword(scanner, "null", 4, TOK_NULL, "error reading 'null'");

    case '{':
        return scan_punctuation(scanner, TOK_LBRACE);

    case '}':
        return scan_punctuation(scanner, TOK_RBRACE);

    case '[':
        return scan_punctuation(scanner, TOK_LBRACK);

    case ']':
        return scan_punctuation(scanner, TOK_RBRACK);

    case ',':
        return scan_punctuation(scanner, TOK_COMMA);

    case ':':
        return scan_punctuation(scanner, TOK_COLON);

    case '-':
        (void) advance(scanner);
        return scan_number(scanner, true);

    default:
        scanner->err_msg = "unknown character";
        return RESULT_FAIL;
    }
}

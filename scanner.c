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


static void skip_spaces(struct Scanner *scanner) {
    while (*scanner->stream && isspace(*scanner->stream))
        (scanner->stream)++;
}

static enum Result scan_keyword(
    struct Scanner *scanner,
    char *keyword,
    enum Token tok,
    const char *err_msg)
{
    scanner->curr_token = tok;
    while (*keyword) {
        if (*scanner->stream++ != *keyword++) {
            scanner->err_msg = err_msg;
            return RESULT_FAIL;
        }
    }
    return RESULT_OK;
}

static enum Result scan_integer(struct Scanner *scanner, bool *was_zero) {
    char c = *scanner->stream++;
    if (c == '0') {
        *was_zero = true;
        return RESULT_OK;
    } else {
        while (*scanner->stream && isdigit(*scanner->stream))
            scanner->stream++;
        *was_zero = false;
        return RESULT_OK;
    }
}

static enum Result scan_frac(struct Scanner *scanner) {
    if (*scanner->stream++ != '.') {
        scanner->err_msg = "missing '.' in number";
        return RESULT_FAIL;
    }

    if (!isdigit(*scanner->stream)) {
        scanner->err_msg = "missing fractional part";
        return RESULT_FAIL;
    }

    while (*scanner->stream && isdigit(*scanner->stream))
        scanner->stream++;

    return RESULT_OK;
}

static enum Result scan_exp(struct Scanner *scanner) {
    if (*scanner->stream != 'e' && *scanner->stream != 'E') {
        scanner->err_msg = "missing 'e' in number";
        return RESULT_FAIL;
    }
    scanner->stream++;

    if (*scanner->stream == '+' || *scanner->stream == '-')
        scanner->stream++;

    if (!isdigit(*scanner->stream)) {
        scanner->err_msg = "missing exponent part";
        return RESULT_FAIL;
    }

    while (*scanner->stream && isdigit(*scanner->stream))
        scanner->stream++;

    return RESULT_OK;
}


static enum Result scan_number(struct Scanner *scanner, bool negative) {
    scanner->curr_token = TOK_NUMBER;

    bool was_zero;
    if (scan_integer(scanner, &was_zero) == RESULT_FAIL)
        return RESULT_FAIL;

    if (*scanner->stream == '.') {
        if (scan_frac(scanner) == RESULT_FAIL)
            return RESULT_FAIL;
        was_zero = false;
    }

    if (*scanner->stream == 'E' || *scanner->stream == 'e') {
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
    if (!ishexdigit(*++scanner->stream)) return RESULT_FAIL;
    if (!ishexdigit(*++scanner->stream)) return RESULT_FAIL;
    if (!ishexdigit(*++scanner->stream)) return RESULT_FAIL;
    if (!ishexdigit(*++scanner->stream)) return RESULT_FAIL;
    return RESULT_OK;
}

static enum Result scan_string(struct Scanner *scanner) {
    scanner->curr_token = TOK_STRING;
    if (*scanner->stream != '"') {
        scanner->err_msg = "expected opening '\"'";
        return RESULT_FAIL;
    }
    scanner->stream++;

    bool escape = false;
    while (*scanner->stream) {
        char c = *scanner->stream;

        if (!escape && c == '"')
            break;

        if (escape) {
            switch (c) {
            case '"':
            case '\\':
            case '/':
            case 'b':
            case 'f':
            case 'n':
            case 'r':
            case 't':
                escape = false;
                break;
            case 'u':
                if (scan_four_hex(scanner) == RESULT_FAIL) {
                    scanner->err_msg = "invalid unicode escape sequence";
                    return RESULT_FAIL;
                }
                escape = false;
                break;
            default:
                scanner->err_msg = "invalid escape sequence";
                return RESULT_FAIL;
            }
        } else {
            escape = (c == '\\');
        }

        scanner->stream++;
    }

    if (*scanner->stream != '"') {
        scanner->err_msg = "expected closing '\"'";
        return RESULT_FAIL;
    }
    scanner->stream++;

    return RESULT_OK;
}

static enum Result scan_punctuation(struct Scanner *scanner, enum Token tok) {
    scanner->stream++;
    scanner->curr_token = tok;
    return RESULT_OK;
}

enum Result spotless_scanner_next(struct Scanner *scanner) {
    skip_spaces(scanner);
    switch (*scanner->stream) {
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
        return scan_keyword(scanner, "true", TOK_TRUE, "error reading 'true'");

    case 'f':
        return scan_keyword(scanner, "false", TOK_FALSE, "error reading 'false'");

    case 'n':
        return scan_keyword(scanner, "null", TOK_NULL, "error reading 'null'");

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
        scanner->stream++;
        return scan_number(scanner, true);

    default:
        scanner->err_msg = "unknown character";
        return RESULT_FAIL;
    }
}

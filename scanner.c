#include <ctype.h>
#include <stdbool.h>

#include "scanner.h"

#define NON_ZERO_DIGIT      \
         '1':               \
    case '2':               \
    case '3':               \
    case '4':               \
    case '5':               \
    case '6':               \
    case '7':               \
    case '8':               \
    case '9'

#define DIGIT               \
    '0':                    \
    case NON_ZERO_DIGIT

void skip_spaces(struct Scanner *scanner) {
    while (scanner->stream && isspace(*scanner->stream))
        (scanner->stream)++;
}

enum ScanResult scan_keyword(
    struct Scanner *scanner,
    char *keyword,
    enum Token tok,
    const char *err_msg)
{
    while (*keyword) {
        if (*scanner->stream++ != *keyword++) {
            scanner->err_msg = err_msg;
            return RESULT_FAIL;
        }
    }
    scanner->curr_token = tok;
    return RESULT_OK;
}

enum ScanResult scan_integer(struct Scanner *scanner, bool negative) {
    char c = *scanner->stream++;
    if (c == '0' && negative) {
        scanner->err_msg = "zero cannot be negative";
        return RESULT_FAIL;
    } else if (c == '0') {
        return RESULT_OK;
    } else {
        while (scanner->stream && isdigit(*scanner->stream))
            scanner->stream++;
        return RESULT_OK;
    }
}

enum ScanResult scan_frac(struct Scanner *scanner) {
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

enum ScanResult scan_exp(struct Scanner *scanner) {
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


enum ScanResult scan_number(struct Scanner *scanner, bool negative) {
    scanner->curr_token = TOK_NUMBER;

    if (scan_integer(scanner, negative) == RESULT_FAIL)
        return RESULT_FAIL;

    if (*scanner->stream == '.')
        if (scan_frac(scanner) == RESULT_FAIL)
            return RESULT_FAIL;

    if (*scanner->stream == 'E' || *scanner->stream == 'e')
        if (scan_exp(scanner) == RESULT_FAIL)
            return RESULT_FAIL;

    return RESULT_OK;
}

enum ScanResult next(struct Scanner *scanner) {
    skip_spaces(scanner);
    switch (*scanner->stream) {
    case 0:
        if (scanner->curr_token == TOK_NONE)
            return RESULT_FAIL;
        scanner->curr_token = TOK_EOF;
        return RESULT_OK;

    case 't':
        return scan_keyword(scanner, "true", TOK_TRUE, "error reading 'true'");

    case 'f':
        return scan_keyword(scanner, "false", TOK_FALSE, "error reading 'false'");

    case 'n':
        return scan_keyword(scanner, "null", TOK_NULL, "error reading 'null'");

    case '-':
        scanner->stream++;
        return scan_number(scanner, true);

    case DIGIT:
        return scan_number(scanner, false);

    default:
        scanner->err_msg = "unknown character";
        return RESULT_FAIL;
    }
}

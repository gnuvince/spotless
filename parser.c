#include <stdlib.h>

#include "common.h"
#include "scanner.h"
#include "parser.h"

static enum Result advance(struct Parser *parser) {
    if (spotless_scanner_next(parser->scanner) == RESULT_FAIL) {
        parser->err_msg = parser->scanner->err_msg;
        return RESULT_FAIL;
    }
    parser->curr_token = parser->scanner->curr_token;
    return RESULT_OK;
}

static enum Result eat(struct Parser *parser, enum Token tok, const char *err_msg) {
    if (parser->curr_token != tok) {
        parser->err_msg = err_msg;
        return RESULT_FAIL;
    }
    return advance(parser);
}

static enum Result parse_value(struct Parser *);

static enum Result parse_pair(struct Parser *parser) {
    if (eat(parser, TOK_STRING, "expected a string key") == RESULT_FAIL)
        return RESULT_FAIL;
    if (eat(parser, TOK_COLON, "expected ':'") == RESULT_FAIL)
        return RESULT_FAIL;
    return parse_value(parser);
}

static enum Result parse_rest_object(struct Parser *parser) {
    while (parser->curr_token != TOK_EOF) {
        switch (parser->curr_token) {
        case TOK_RBRACE:
            if (advance(parser) == RESULT_FAIL) return RESULT_FAIL;
            return RESULT_OK;

        case TOK_COMMA:
            if (advance(parser) == RESULT_FAIL) return RESULT_FAIL;
            if (parse_pair(parser) == RESULT_FAIL)
                return RESULT_FAIL;
            break;

        default:
            parser->err_msg = "expected ','";
            return RESULT_FAIL;
        }
    }
    parser->err_msg = "unterminated object";
    return RESULT_FAIL;
}

static enum Result parse_object(struct Parser *parser) {
    if (advance(parser) == RESULT_FAIL) return RESULT_FAIL;
    switch (parser->curr_token) {
    case TOK_RBRACE:
        /* Empty object */
        return advance(parser);

    case TOK_STRING:
        if (parse_pair(parser) == RESULT_FAIL) {
            return RESULT_FAIL;
        }
        return parse_rest_object(parser);

    default:
        parser->err_msg = "expected a key-value pair or nothing";
        return RESULT_FAIL;
    }
}

static enum Result parse_rest_of_array(struct Parser *parser) {
    while (parser->curr_token != TOK_EOF) {
        switch (parser->curr_token) {
        case TOK_RBRACK:
            return advance(parser);

        case TOK_COMMA:
            if (advance(parser) == RESULT_FAIL) return RESULT_FAIL;
            if (parse_value(parser) == RESULT_FAIL)
                return RESULT_FAIL;
            break;

        default:
            parser->err_msg = "expected ','";
            return RESULT_FAIL;
        }
    }
    parser->err_msg = "unterminated array";
    return RESULT_FAIL;
}

static enum Result parse_array(struct Parser *parser) {
    if (advance(parser) == RESULT_FAIL) return RESULT_FAIL;
    switch (parser->curr_token) {
    case TOK_RBRACK:
        /* Empty array */
        return advance(parser);

    case TOK_NUMBER:
    case TOK_STRING:
    case TOK_FALSE:
    case TOK_TRUE:
    case TOK_NULL:
    case TOK_LBRACE:
    case TOK_LBRACK:
        if (parse_value(parser) == RESULT_FAIL)
            return RESULT_FAIL;
        return parse_rest_of_array(parser);

    default:
        parser->err_msg = "not an array";
        return RESULT_FAIL;
    }
}

static enum Result parse_value(struct Parser *parser) {
    switch (parser->curr_token) {
    case TOK_NUMBER:
    case TOK_STRING:
    case TOK_FALSE:
    case TOK_TRUE:
    case TOK_NULL:
        return advance(parser);

   case TOK_LBRACE:
        return parse_object(parser);

    case TOK_LBRACK:
        return parse_array(parser);

    default:
        parser->err_msg = "expected value";
        return RESULT_FAIL;
    }
}

enum Result spotless_parser_parse(struct Parser *parser) {
    if (advance(parser) == RESULT_FAIL)
        return RESULT_FAIL;

    if (parse_value(parser) == RESULT_FAIL)
        return RESULT_FAIL;

    return eat(parser, TOK_EOF, "expected EOF");
}

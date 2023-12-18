#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include "../../includes/alex/lexer.h"
#include "../../includes/alex/utils.h"

const char *enum_names[] = {
    "ID",
    "FUNCTION", "IF", "ELSE", "RETURN", "END", "VAR", "WHILE", "FOR",
    "TYPE_INT", "TYPE_REAL", "TYPE_STR",
    "COMMA", "FINISH", "COLON", "SEMICOLON",
    "ASSIGN", "EQUAL", "LESS", "ADD", "SUB", "MUL", "DIV", "NOT", "AND", "OR",
    "COMMENT",
    "LPAR", "RPAR",
    "INT", "REAL",
    "STR"};

// copy in the dst buffer the string between [begin,end)
char *copyn(char *dst, const char *begin, const char *end)
{
    char *p = dst;
    if (end - begin > MAX_STR)
        err("string too long");

    while (begin != end)
        *p++ = *begin++;

    *p = '\0';

    return dst;
}

char *parse_string_literal(char *buf)
{
    pch++;
    const char *begin = pch;

    while (*pch != '\0' && *pch != '"')
    {
        pch++;
    }

    if (*pch == '\0')
    {
        err("unterminated string literal");
    }

    const char *end = pch;
    pch++;

    if (end - begin > MAX_STR)
    {
        err("string literal too long");
    }

    return copyn(buf, begin, end);
}

char *parse_number(char *buf)
{
    const char *begin = pch;
    while (isdigit(*pch) || (*pch == '.'))
    {
        pch++;
    }
    const char *end = pch;

    if (end - begin > MAX_STR)
    {
        err("number too long");
    }

    buf[0] = '\0';
    return copyn(buf, begin, end);
}

char *parse_identifier(char *buf)
{
    const char *begin = pch;
    while (isalnum(*pch) || *pch == '_')
    {
        pch++;
    }
    const char *end = pch;

    buf[0] = '\0';
    return copyn(buf, begin, end);
}

bool is_comment()
{
    return (*pch != '\n' && *pch != '\r' && *pch != '\0');
}

bool count_char_occurrences(char *str, char c)
{
    int count = 0;
    for (int i = 0; i < strlen(str); i++)
    {
        if (str[i] == c)
        {
            count++;
        }
    }

    if (count == 1)
        return true;

    if (count > 1)
        err("invalid real number");

    return false;
}
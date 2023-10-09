#include <stdbool.h>
#include <ctype.h>

#include "../includes/lexer.h"
#include "../includes/utils.h"

const char *enum_names[] = {
    "ID",
    "TYPE_INT",
    "COMMA", "FINISH", "COLON", "SEMICOLON",
    "ASSIGN", "EQUAL", "LESS", "ADD",
	"COMMENT",
	"LPAR", "RPAR",
	"INT",
	"STR"
};

// copy in the dst buffer the string between [begin,end)
char *copyn(char *dst,const char *begin,const char *end)
{
	char *p = dst;
	if(end-begin > MAX_STR)
		err("string too long");

	while(begin != end)
		*p++ = *begin++;

	*p='\0';

	return dst;
}

char *parse_string_literal(const char *pch, char *buf)
{
    const char *begin = pch;
    pch++;

    while (*pch != '\0' && *pch != '"') {
        pch++;
    }

    if (*pch == '\0') {
        err("unterminated string literal");
    }

    const char *end = pch;
    pch++;

    if (end - begin > MAX_STR) {
        err("string literal too long");
    }

    return copyn(buf, begin, end);
}

char *parse_number(const char *pch, char *buf)
{
    const char *begin = pch;
    while (isdigit(*pch)) {
        pch++;
    }
    const char *end = pch;

    if (end - begin > MAX_STR) {
        err("number too long");
    }

    buf[0] = '\0';
    return copyn(buf, begin, end);
}

char *parse_identifier(const char *pch, char *buf)
{
    const char *begin = pch;
    while (isalnum(*pch) || *pch == '_') {
        pch++;
    }
    const char *end = pch;

    buf[0] = '\0';
    return copyn(buf, begin, end);
}

bool is_comment(const char *pch)
{
    return (*pch != '\n' && *pch != '\r' && *pch != '\0');
}
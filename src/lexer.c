#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "../includes/lexer.h"
#include "../includes/utils.h"

Token tokens[MAX_TOKENS];
int nTokens;

int line=1;		// the current line in the input file

// adds a token to the end of the tokens list and returns it
// sets its code and line
Token *addTk(int code)
{
	if(nTokens == MAX_TOKENS)
		err("too many tokens");

	Token *tk = &tokens[nTokens];
	tk->code = code;
	tk->line = line;
	nTokens++;

	return tk;
}

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

/*

pch - (pointer at current char) este folosit ca iterator în textul de intrare.

*/
void tokenize(const char *pch)
{
	const char *start;
	Token *tk;
	char buf[MAX_STR+1];

	for(;;)
	{
		switch(*pch)
		{
			case EOF:
			case '\0':
				addTk(FINISH);
				return;

			case ',':
				addTk(COMMA);
				pch++;
				break;

			case ':':
				addTk(COLON);
				pch++;
				break;

			case ';':
				addTk(SEMICOLON);
				pch++;
				break;

			case '=':
				if(pch[1] == '=')
				{
					addTk(EQUAL);
					pch+=2;
				}
				else
				{
					addTk(ASSIGN);
					pch++;
				}
				break;

			case '#':
				while (*pch != '\n' && *pch != '\r' && *pch != '\0')
					pch++;

				break;

			case '(':
				addTk(LPAR);
				pch++;
				break;

			case ')':
				addTk(RPAR);
				pch++;
				break;

			case '<':
				addTk(LESS);
				pch++;
				break;

			case '0' ... '9':
				// integer
				const char *begin = pch;
				while (isdigit(*pch)) {
					pch++;
				}
				const char *end = pch;

				if (end - begin > MAX_STR) {
					err("number too long");
				}

				buf[0] = '\0';
				char *text = copyn(buf, begin, end);
				tk = addTk(INT);
				strcpy(tk->text, text);
				break;

			default:
				if(isspace(*pch)) // handles diff. types of spaces(\n, \t, \r,)
				{
					pch++;
					break;
				}
				else if(isalpha(*pch) || *pch == '_')
				{
					// verify if it is a keyword or an identifier
					for(start = pch++; isalnum(*pch) || *pch == '_'; pch++){}

					buf[0] = '\0';
					char *text = copyn(buf, start, pch);
					if(strncmp(text,"int", 3) == 0)
						addTk(TYPE_INT);
					else
					{
						tk = addTk(ID);
						strcpy(tk->text, text);
					}
				}
				else
					err("invalid char: %c (%d)", *pch, *pch);
		}
	}
}

void showTokens()
{
	printf("%-5s %-15s %s", "CODE", "NAME", "TEXT\n\n");
	for(int i = 0; i < nTokens; i++)
	{
		Token *tk = &tokens[i];
		printf("%-5d %-15s %s\n",
			tk->code,
			enum_names[tk->code],
			tk->text != NULL ? tk->text : "");
	}
}

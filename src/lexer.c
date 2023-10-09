#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include "../includes/lexer.h"
#include "../includes/utils.h"

Token tokens[MAX_TOKENS];
int nTokens;

int line = 1;		// the current line in the input file

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

void tokenize(const char *pch)
{
	Token *tk;
	char buf[MAX_STR+1];
	char *text;

	for(;;)
	{
		// text[0] = '\0';
		switch(*pch)
		{
			// end of file
			case EOF:
			case '\0':
				addTk(FINISH);
				return;
			// delimiters
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

			// operators
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

			case '<':
				addTk(LESS);
				pch++;
				break;

			case '+':
				addTk(ADD);
				pch++;
				break;

			// comments
			case '#':
				while (is_comment(pch))
					pch++;
				break;

			// parantheses
			case '(':
				addTk(LPAR);
				pch++;
				break;

			case ')':
				addTk(RPAR);
				pch++;
				break;

			// string literals
			case '"':
				text = parse_string_literal(pch, buf);
				tk = addTk(STR);
				strcpy(tk->text, text);
				break;

			// constants
			case '0' ... '9':
				text = parse_number(pch, buf);
				tk = addTk(INT);
				strcpy(tk->text, text);
				break;

			default:
			// spaces and new lines
				if(isspace(*pch)) // handles diff. types of spaces(\n, \t, \r,)
				{
					pch++;
					break;
				}
			// identifiers
				else if(isalpha(*pch) || *pch == '_')
				{
					text = parse_identifier(pch, buf);
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
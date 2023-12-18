#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../../includes/alex/lexer.h"
#include "../../includes/alex/utils.h"

Token tokens[MAX_TOKENS];
int nTokens;

int line = 1; // the current line in the input file
char *pch = NULL;
char *fileName1 = "/home/ursu/Univer/Year3/LFTC/compiler/test_alex/1.q";
char *fileName2 = "/home/ursu/Univer/Year3/LFTC/compiler/test_alex/2.q";

// adds a token to the end of the tokens list and returns it
// sets its code and line
Token *addTk(int code)
{
	if (nTokens == MAX_TOKENS)
		err("too many tokens");

	Token *tk = &tokens[nTokens];
	tk->code = code;
	tk->line = line;
	nTokens++;

	return tk;
}

void tokenize()
{
	Token *tk;
	char buf[MAX_STR + 1];
	char *text;

	for (;;)
	{
		// text[0] = '\0';
		switch (*pch)
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
			if (pch[1] == '=')
			{
				addTk(EQUAL);
				pch += 2;
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

		case '-':
			addTk(SUB);
			pch++;
			break;

		case '*':
			addTk(MUL);
			pch++;
			break;

		case '/':
			addTk(DIV);
			pch++;
			break;

		case '!':
			addTk(NOT);
			pch++;
			break;

		case '&':
			if (pch[1] == '&')
			{
				addTk(AND);
				pch += 2;
			}
			else
				err("invalid 'and' operator: %c (%d)", *pch, *pch);
			break;

		case '|':
			if (pch[1] == '|')
			{
				addTk(OR);
				pch += 2;
			}
			else
				err("invalid 'or' operator: %c (%d)", *pch, *pch);
			break;

		// comments
		case '#':
			while (is_comment())
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
			text = parse_string_literal(buf);
			tk = addTk(STR);
			strcpy(tk->text, text);
			break;

		// constants
		case '0' ... '9':
			text = parse_number(buf);
			if (count_char_occurrences(text, '.'))
			{
				tk = addTk(REAL);
				tk->r = atof(text);
			}
			else
			{
				tk = addTk(INT);
				tk->i = atoi(text);
			}
			break;

		default:
			// spaces and new lines
			if (isspace(*pch)) // handles diff. types of spaces(\n, \t, \r,)
			{
				// if new line then increment line
				if (*pch == '\n')
					line++;
				pch++;
				break;
			}
			// identifiers
			else if (isalpha(*pch) || *pch == '_')
			{
				text = parse_identifier(buf);
				if (strncmp(text, "int", 3) == 0)
					addTk(TYPE_INT);
				else if (strncmp(text, "real", 4) == 0)
					addTk(TYPE_REAL);
				else if (strncmp(text, "str", 3) == 0)
					addTk(TYPE_STR);
				else if (strncmp(text, "function", 8) == 0)
					addTk(FUNCTION);
				else if (strncmp(text, "if", 2) == 0)
					addTk(IF);
				else if (strncmp(text, "else", 4) == 0)
					addTk(ELSE);
				else if (strncmp(text, "return", 6) == 0)
					addTk(RETURN);
				else if (strncmp(text, "end", 3) == 0)
					addTk(END);
				else if (strncmp(text, "var", 3) == 0)
					addTk(VAR);
				else if (strncmp(text, "while", 5) == 0)
					addTk(WHILE);
				else if (strncmp(text, "for", 3) == 0)
					addTk(FOR);
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
	printf("%-5s %-15s %s", "CODE", "TOKEN", "TEXT\n\n");
	for (int i = 0; i < nTokens; i++)
	{
		Token *tk = &tokens[i];
		printf("%-5d %-15s ",
			   tk->code,
			   enum_names[tk->code]);

		switch (tk->code)
		{
		case ID:
		case STR:
			printf("%s\n", tk->text);
			break;
		case INT:
			printf("%d\n", tk->i);
			break;
		case REAL:
			printf("%g\n", tk->r);
			break;
		default:
			printf("\n");
		}
	}
}
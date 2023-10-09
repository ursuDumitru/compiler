#pragma once

#include <stdbool.h>

enum{
	ID,
	// keywords
	TYPE_INT,
	// delimiters
	COMMA, FINISH, COLON, SEMICOLON,
	// operators
	ASSIGN, EQUAL, LESS, ADD,
	// commments
	COMMENT,
	// parantheses
	LPAR, RPAR,
	// constants
	INT,
	// String literals
	STR
	};

extern const char *enum_names[];

#define MAX_STR		127

typedef struct{
	int code;		// ID, TYPE_INT, ...
	int line;		// the line from the input file
	union{
		char text[MAX_STR+1];		// the chars for ID, STR
		int i;						// the value for INT
		double r;					// the value for REAL
		};
	}Token;

#define MAX_TOKENS		4096
extern Token tokens[];
extern int nTokens;

void tokenize(const char *pch);
void showTokens();

// helper functions
char *copyn(char *dst,const char *begin,const char *end);
char *parse_string_literal(const char *pch, char *buf);
char *parse_number(const char *pch, char *buf);
char *parse_identifier(const char *pch, char *buf);
bool is_comment(const char *pch);

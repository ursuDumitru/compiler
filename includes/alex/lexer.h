#pragma once

#include <stdbool.h>

enum
{
	ID,
	// keywords
	FUNCTION,
	IF,
	ELSE,
	RETURN,
	END,
	VAR,
	WHILE,
	FOR,
	TYPE_INT,
	TYPE_REAL,
	TYPE_STR,
	// delimiters
	COMMA,
	FINISH,
	COLON,
	SEMICOLON,
	// operators
	ASSIGN,
	EQUAL,
	LESS,
	ADD,
	SUB,
	MUL,
	DIV,
	NOT,
	AND,
	OR,
	// commments
	COMMENT,
	// parantheses
	LPAR,
	RPAR,
	// constants
	INT,
	REAL,
	// String literals
	STR
};

extern char *pch;
extern char *fileName1;
extern char *fileName2;
extern const char *enum_names[];

#define MAX_STR 127

typedef struct
{
	int code; // ID, TYPE_INT, ...
	int line; // the line from the input file
	union
	{
		char text[MAX_STR + 1]; // the chars for ID, STR
		int i;					// the value for INT
		double r;				// the value for REAL
	};
} Token;

#define MAX_TOKENS 4096
extern Token tokens[];
extern int nTokens;

void tokenize();
void showTokens();

// helper functions
char *copyn(char *dst, const char *begin, const char *end);
char *parse_string_literal(char *buf);
char *parse_number(char *buf);
char *parse_identifier(char *buf);
bool is_comment();
bool count_char_occurrences(char *str, char c);

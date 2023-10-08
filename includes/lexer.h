#pragma once

enum{
	ID,
	// keywords
	TYPE_INT,
	// delimiters
	COMMA, FINISH, COLON, SEMICOLON,
	// operators
	ASSIGN, EQUAL,
	// commments
	COMMENT,
	// parantheses
	LPAR, RPAR,
	// operators
	LESS,
	// constants
	INT
	};

const char *enum_names[] = {
    "ID",
    "TYPE_INT",
    "COMMA", "FINISH", "COLON", "SEMICOLON",
    "ASSIGN", "EQUAL",
	"COMMENT",
	"LPAR", "RPAR",
	"LESS",
	"INT"
};

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

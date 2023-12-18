#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

extern int iTk;		 // the iterator in tokens

// same as err, but also prints the line of the current token
_Noreturn void tkerr(const char *fmt, ...);
// parse the extracted tokens
void parse();
// consumes the token if it matches the code
bool consume(int code);
// factor ::= INT | REAL | STR | LPAR expr RPAR | ID ( LPAR ( expr ( COMMA expr )* )? RPAR )?
bool factor();
// exprPrefix ::= ( SUB | NOT )? factor
bool exprPrefix();
// exprMul ::= exprPrefix ( ( MUL | DIV ) exprPrefix )*
bool exprMul();
// exprAdd ::= exprMul ( ( ADD | SUB ) exprMul )*
bool exprAdd();
// exprComp ::= exprAdd ( ( LESS | EQUAL ) exprAdd )?
bool exprComp();
// exprAssign ::= ( ID ASSIGN )? exprComp
bool exprAssign();
// exprLogic ::= exprAssign ( ( AND | OR ) exprAssign )*
bool exprLogic();
// expr ::= exprLogic
bool expr();
// instr ::= expr? SEMICOLON | IF LPAR expr RPAR block ( ELSE block )? END | RETURN expr SEMICOLON | WHILE LPAR expr RPAR block END
bool instr();
// funcParam ::= ID COLON baseType
bool funcParam();
// funcParams ::= funcParam ( COMMA funcParam )*
bool funcParams();
// block ::= instr+
bool block();
// defFunc ::= FUNCTION ID LPAR funcParams? RPAR COLON baseType defVar* block END
bool defFunc();
// baseType ::= TYPE_INT | TYPE_REAL | TYPE_STR
bool baseType();
// defVar ::= VAR ID COLON baseType SEMICOLON
bool defVar();
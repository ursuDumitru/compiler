#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../../includes/alex/lexer.h"
#include "../../includes/sintactic/parser.h"
#include "../../includes/symbols/ad.h"
#include "../../includes/atip/at.h"
#include "../../includes/gen/gen.h"

int iTk;

// factor ::= INT | REAL | STR | LPAR expr RPAR | ID ( LPAR ( expr ( COMMA expr )* )? RPAR )?
bool factor()
{
    // printf("Entering factor(), token: %s\n", enum_names[tokens[iTk].code]);

    if (consume(INT))
    {
        setRet(TYPE_INT, false);
        Text_write(crtCode, "%d", tokens[iTk - 1].i);

        return true;
    }
    if (consume(REAL))
    {
        setRet(TYPE_REAL, false);
        Text_write(crtCode, "%g", tokens[iTk - 1].r);

        return true;
    }
    if (consume(STR))
    {
        setRet(TYPE_STR, false);
        Text_write(crtCode, "\"%s\"", tokens[iTk - 1].text);

        return true;
    }
    if (consume(LPAR))
    {
        Text_write(crtCode, "(");

        if (expr())
        {
            if (consume(RPAR))
            {
                Text_write(crtCode, ")");

                return true;
            }
            else
            {
                tkerr("Missing closing parenthesis ')' in defining expresion");
            }
        }
    }
    // ID ( LPAR ( expr ( COMMA expr )* )? RPAR )?
    // ID ( LPAR ( expr ( COMMA expr )* )? RPAR | epsilon )
    if (consume(ID))
    {
        Symbol *s = searchSymbol(tokens[iTk - 1].text);
        if (!s)
            tkerr("Undefined symbol: %s", tokens[iTk - 1].text);

        Text_write(crtCode, "%s", s->name);

        if (consume(LPAR))
        {
            if (s->kind != KIND_FN)
                tkerr("'%s' Cannot be called, because it is not a function", s->name);
            Symbol *argDef = s->args;

            Text_write(crtCode, "(");

            if (expr())
            {
                if (!argDef)
                    tkerr("The function %s is called with too many arguments", s->name);
                if (argDef->type != ret.type)
                    tkerr("The argument type at function %s call is different from the one given at its definition", s->name);
                argDef = argDef->next;

                while (consume(COMMA))
                {
                    Text_write(crtCode, ",");

                    if (expr())
                    {
                        if (!argDef)
                            tkerr("The function %s is called with too many arguments", s->name);
                        if (argDef->type != ret.type)
                            tkerr("The argument type at function %s call is different from the one given at its definition", s->name);
                        argDef = argDef->next;

                        continue;
                    }
                    else
                    {
                        tkerr("Invalid expression inside function call");
                    }
                }
            }
            if (consume(RPAR))
            {
                if (argDef)
                    tkerr("The function %s is called with too few arguments", s->name);
                setRet(s->type, false);

                Text_write(crtCode, ")");

                return true;
            }
            else
                tkerr("Missing closing parenthesis ')' in function call");
        }

        if (s->kind == KIND_FN)
            tkerr("The function %s can only be called", s->name);
        setRet(s->type, true);

        return true;
    }

    return false;
}

// exprPrefix ::= ( SUB | NOT )? factor
//                  SUB factor | NOT factor | factor
bool exprPrefix()
{
    // printf("Entering exprPrefix(), token: %s\n", enum_names[tokens[iTk].code]);
    if (consume(SUB))
    {
        Text_write(crtCode, "-");

        if (factor())
        {
            if (ret.type == TYPE_STR)
                tkerr("The expression of unary - must be of type int or real");
            ret.lval = false;

            return true;
        }
        // else
        //     tkerr("Missing factor after substraction(-)");
    }

    if (consume(NOT))
    {
        Text_write(crtCode, "!");

        if (factor())
        {
            if (ret.type == TYPE_STR)
                tkerr("The expression of ! must be of type int or real");
            setRet(TYPE_INT, false);

            return true;
        }
        // else
        //     tkerr("Missing factor after not(!)");
    }

    if (factor())
    {
        return true;
    }

    return false;
}

// exprMul ::= exprPrefix ( ( MUL | DIV ) exprPrefix )*
bool exprMul()
{
    // printf("Entering exprMul(), token: %s\n", enum_names[tokens[iTk].code]);

    if (exprPrefix())
    {
        while (consume(MUL) || consume(DIV))
        {
            Ret leftType = ret;
            if (leftType.type == TYPE_STR)
                tkerr("the operands of * or / cannot be of type str");

            char msg[18];
            if (tokens[iTk - 1].code == DIV)
            {
                Text_write(crtCode, "/");
                strcpy(msg, "division(/)");
            }
            else if (tokens[iTk - 1].code == MUL)
            {
                Text_write(crtCode, "*");
                strcpy(msg, "multiplication(*)");
            }

            if (!exprPrefix())
            {
                tkerr("No expresion prefix after %s", msg);
            }

            if (leftType.type != ret.type)
                tkerr("different types for the operands of * or /");
            ret.lval = false;
        }

        return true;
    }

    return false;
}

// exprAdd ::= exprMul ( ( ADD | SUB ) exprMul )*
bool exprAdd()
{
    // printf("Entering exprAdd(), token: %s\n", enum_names[tokens[iTk].code]);

    if (exprMul())
    {
        while (consume(ADD) || consume(SUB))
        {
            Ret leftType = ret;
            if (leftType.type == TYPE_STR)
                tkerr("The operands of + or - cannot be of type str");

            char msg[16];
            if (tokens[iTk - 1].code == ADD)
            {
                Text_write(crtCode, "+");
                strcpy(msg, "addition(+)");
            }
            else if (tokens[iTk - 1].code == SUB)
            {
                Text_write(crtCode, "-");
                strcpy(msg, "substraction(-)");
            }

            if (!exprMul())
            {
                tkerr("No multiplication expresion after %s", msg);
            }

            if (leftType.type != ret.type)
                tkerr("Different types for the operands of + or -");
            ret.lval = false;
        }

        return true;
    }

    return false;
}

// exprComp ::= exprAdd ( ( LESS | EQUAL ) exprAdd )?
bool exprComp()
{
    // printf("Entering exprComp(), token: %s\n", enum_names[tokens[iTk].code]);

    if (exprAdd())
    {
        if (consume(LESS) || consume(EQUAL))
        {
            Ret leftType = ret;

            char msg[10];
            if (tokens[iTk - 1].code == LESS)
            {
                Text_write(crtCode, "<");
                strcpy(msg, "less(<)");
            }
            else if (tokens[iTk - 1].code == EQUAL)
            {
                Text_write(crtCode, "==");
                strcpy(msg, "equal(==)");
            }

            if (!exprAdd())
            {
                tkerr("No addition expresion after %s operator", msg);
            }

            if (leftType.type != ret.type)
                tkerr("Different types for the operands of < or ==");
            setRet(TYPE_INT, false); // the result of comparation is int 0 or 1
        }
        return true;
    }

    return false;
}

// exprAssign ::= ( ID ASSIGN )? exprComp
//                  ID ASSIGN exprComp | exprComp
bool exprAssign()
{
    // printf("Entering exprAssign(), token: %s\n", enum_names[tokens[iTk].code]);
    int start = iTk;
    if (consume(ID))
    {
        const char *name = tokens[iTk - 1].text; // needs to be checked
        if (consume(ASSIGN))
        {
            Text_write(crtCode, "%s=", name);

            if (exprComp())
            {
                Symbol *s = searchSymbol(name);
                if (!s)
                    tkerr("Undefined symbol: %s", name);
                if (s->kind == KIND_FN)
                    tkerr("A function (%s) cannot be used as a destination for assignment ", name);
                if (s->type != ret.type)
                    tkerr("The source and destination for assignment must have the same type");
                ret.lval = false;

                return true;
            }
        }
        iTk = start;
    }

    if (exprComp())
    {
        return true;
    }

    return false;
}

// exprLogic ::= exprAssign ( ( AND | OR ) exprAssign )*
bool exprLogic()
{
    // printf("Entering exprLogic(), token: %s\n", enum_names[tokens[iTk].code]);

    if (exprAssign())
    {
        while (consume(AND) || consume(OR))
        {
            Ret leftType = ret;
            if (leftType.type == TYPE_STR)
                tkerr("The left operand of && or || cannot be of type str");

            char msg[8];
            if (tokens[iTk - 1].code == OR)
            {
                Text_write(crtCode, "||");
                strcpy(msg, "or(||)");
            }
            else if (tokens[iTk - 1].code == AND)
            {
                Text_write(crtCode, "&&");
                strcpy(msg, "and(&&)");
            }

            if (!exprAssign())
            {
                tkerr("No assign expresion after %s operand", msg);
            }

            if (ret.type == TYPE_STR)
                tkerr("the right operand of && or || cannot be of type str");
            setRet(TYPE_INT, false);
        }

        return true;
    }

    return false;
}

// expr ::= exprLogic
bool expr()
{
    // printf("Entering expr(), token: %s\n", enum_names[tokens[iTk].code]);

    if (exprLogic())
    {
        return true;
    }

    return false;
}

// instr ::= expr? SEMICOLON | IF LPAR expr RPAR block ( ELSE block )? END | RETURN expr SEMICOLON | WHILE LPAR expr RPAR block END
bool instr()
{
    // printf("Entering instr(), token: %s\n", enum_names[tokens[iTk].code]);

    // expr? SEMICOLON
    if (expr())
    {
        if (consume(SEMICOLON))
        {
            Text_write(crtCode, ";\n");

            return true;
        }
    }

    if (consume(SEMICOLON))
    {
        return true;
    }

    // IF LPAR expr RPAR block ( ELSE block )? END
    if (consume(IF))
    {
        if (consume(LPAR))
        {
            Text_write(crtCode, "if(");

            if (expr())
            {
                if (ret.type == TYPE_STR)
                    tkerr("The if condition must have type int or real");

                if (consume(RPAR)) // when it arrives here it is already in return(next line)
                {
                    Text_write(crtCode, "){\n");

                    if (block())
                    {
                        Text_write(crtCode, "}\n");

                        if (consume(ELSE))
                        {
                            Text_write(crtCode, "else{\n");

                            if (block())
                            {
                                Text_write(crtCode, "}\n");
                                if (consume(END))
                                {
                                    return true;
                                }
                                tkerr("Missing end after else block");
                            }
                        }

                        if (consume(END))
                        {
                            return true;
                        }
                        tkerr("Missing end after if block");
                    }
                }
                tkerr("Missing closing parenthesis ')' in if statement");
            }
        }
        tkerr("Missing opening parenthesis '(' in if statement");
    }

    // RETURN expr SEMICOLON
    if (consume(RETURN))
    {
        Text_write(crtCode, "return ");

        if (expr())
        {
            if (!crtFn)
                tkerr("Teturn can be used only in a function");

            if (ret.type != crtFn->type)
                tkerr("The return type must be the same as the function return type");

            if (consume(SEMICOLON))
            {
                Text_write(crtCode, ";\n");

                return true;
            }
            tkerr("Missing semicolon(;) after return statement");
        }
        tkerr("Missing expression after return statement");
    }

    // WHILE LPAR expr RPAR block END
    if (consume(WHILE))
    {
        Text_write(crtCode, "while(");

        if (consume(LPAR))
        {
            if (expr())
            {
                if (ret.type == TYPE_STR)
                    tkerr("The while condition must have type int or real");

                if (consume(RPAR))
                {
                    Text_write(crtCode, "){\n");

                    if (block())
                    {
                        if (consume(END))
                        {
                            Text_write(crtCode, "}\n");

                            return true;
                        }
                        tkerr("Missing end after while block");
                    }
                    tkerr("Missing block after closing parenthesis ')' in while statement");
                }
                tkerr("Missing closing parenthesis ')' in while statement");
            }
            tkerr("Missing expresion after opening parathesis in while statement");
        }
        tkerr("Missing opening parenthesis '(' in while statement");
    }

    return false;
}

// funcParam ::= ID COLON baseType
bool funcParam()
{
    // printf("Entering funcParam(), token: %s\n", enum_names[tokens[iTk].code]);

    if (consume(ID))
    {
        const char *name = tokens[iTk - 1].text;
        Symbol *s = searchInCurrentDomain(name);
        if (s)
            tkerr("symbol redefinition: %s", name);
        s = addSymbol(name, KIND_ARG);
        Symbol *sFnParam = addFnArg(crtFn, name);

        if (consume(COLON))
        {
            if (baseType())
            {
                s->type = ret.type;
                sFnParam->type = ret.type;

                Text_write(&tFnHeader, "%s %s", cType(ret.type), name);

                return true;
            }
            tkerr("Missing base type in function parameter");
        }
    }

    return false;
}

// funcParams ::= funcParam ( COMMA funcParam )*
bool funcParams()
{
    // printf("Entering funcParams(), token: %s\n", enum_names[tokens[iTk].code]);

    if (funcParam())
    {
        while (consume(COMMA))
        {
            Text_write(&tFnHeader, ",");

            if (!funcParam())
            {
                tkerr("Missing function parameter after comma(,) in function parameters");
            }
        }

        return true;
    }

    return false;
}

// block ::= instr+
bool block()
{
    // printf("Entering block(), token: %s\n", enum_names[tokens[iTk].code]);

    if (instr())
    {
        while (instr())
        {
            continue;
        }

        return true;
    }

    return false;
}

// defFunc ::= FUNCTION ID LPAR funcParams? RPAR COLON baseType defVar* block END
bool defFunc()
{
    // printf("Entering defFunc(), token: %s\n", enum_names[tokens[iTk].code]);

    if (consume(FUNCTION))
    {
        if (consume(ID))
        {
            const char *name = tokens[iTk - 1].text;
            Symbol *s = searchInCurrentDomain(name);
            if (s)
                tkerr("Symbol redefinition: %s", name);
            crtFn = addSymbol(name, KIND_FN);
            crtFn->args = NULL;
            addDomain();

            crtCode = &tFunctions;
            crtVar = &tFunctions;
            Text_clear(&tFnHeader);
            Text_write(&tFnHeader, "%s(", name);

            if (consume(LPAR))
            {
                if (funcParams())
                {
                    if (consume(RPAR))
                    {
                        if (consume(COLON))
                        {
                            if (baseType())
                            {
                                crtFn->type = ret.type;

                                Text_write(&tFunctions, "\n%s %s){\n", cType(ret.type), tFnHeader.buf);

                                while (defVar())
                                {
                                    continue;
                                }

                                if (block())
                                {
                                    if (consume(END))
                                    {
                                        delDomain();
                                        crtFn = NULL;

                                        Text_write(&tFunctions, "}\n");
                                        crtCode = &tMain;
                                        crtVar = &tBegin;

                                        return true;
                                    }
                                    tkerr("Missing end after defining the function");
                                }
                                tkerr("Missing function block in defining the function");
                            }
                            tkerr("Missing base type in defining the function");
                        }
                        tkerr("Missing colon(:) after parenthesis ')' in defining the function");
                    }
                    tkerr("Missing closing parenthesis ')' in defining the function");
                }

                if (consume(RPAR))
                {
                    if (consume(COLON))
                    {
                        if (baseType())
                        {
                            while (defVar())
                            {
                                continue;
                            }

                            if (block())
                            {
                                if (consume(END))
                                {
                                    return true;
                                }
                                tkerr("Missing end after defining the function");
                            }
                            tkerr("Missing function block in defining the function");
                        }
                        tkerr("Missing base type in defining the function");
                    }
                    tkerr("Missing colon(:) after parenthesis ')' in defining the function");
                }
                tkerr("Missing closing parenthesis ')' in defining the function");
            }
            tkerr("Missing opening parenthesis '(' in defining the function");
        }
        tkerr("Missing identificator in defining the function");
    }

    return false;
}

// baseType ::= TYPE_INT | TYPE_REAL | TYPE_STR
bool baseType()
{
    // printf("Entering baseType(), token: %s\n", enum_names[tokens[iTk].code]);

    if (consume(TYPE_INT))
    {
        ret.type = TYPE_INT;
        return true;
    }
    if (consume(TYPE_REAL))
    {
        ret.type = TYPE_REAL;
        return true;
    }
    if (consume(TYPE_STR))
    {
        ret.type = TYPE_STR;
        return true;
    }

    return false;
}

// defVar ::= VAR ID COLON baseType SEMICOLON
bool defVar()
{
    // printf("Entering defVar(), token: %s\n", enum_names[tokens[iTk].code]);

    if (consume(VAR))
    {
        if (consume(ID))
        {
            const char *name = tokens[iTk - 1].text; // CHECK THIS
            Symbol *s = searchInCurrentDomain(name);
            if (s)
                tkerr("symbol redefinition: %s", name);
            s = addSymbol(name, KIND_VAR);
            s->local = crtFn != NULL;

            if (consume(COLON))
            {
                if (baseType())
                {
                    s->type = ret.type;

                    if (consume(SEMICOLON))
                    {
                        Text_write(crtVar, "%s %s;\n", cType(ret.type), name);

                        return true;
                    }
                    else
                        tkerr("Missing semicolon(;) in defining a variable");
                }
                else
                    tkerr("Missing base type in defining a variable");
            }
            else
                tkerr("Missing colon(:) in defining a variable");
        }
        else
            tkerr("Missing identificator in defining a variable");
    }
    return false;
}
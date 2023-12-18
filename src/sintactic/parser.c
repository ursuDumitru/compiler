#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "../../includes/sintactic/parser.h"
#include "../../includes/alex/lexer.h"
#include "../../includes/symbols/ad.h"
#include "../../includes/atip/at.h"
#include "../../includes/gen/gen.h"

// int iTk;		 // the iterator in tokens
Token *consumed; // the last consumed token

// same as err, but also prints the line of the current token
#include <string.h>

_Noreturn void tkerr(const char *fmt, ...)
{
    FILE *file = fopen(fileName1, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Failed to open file: %s\n", fileName1);
        exit(EXIT_FAILURE);
    }

    char line[256];
    int lineNumber = tokens[iTk].line; // problem with iTk when an error is at the end of the line
    for (int i = 1; i <= lineNumber; i++)
    {
        if (fgets(line, sizeof(line), file) == NULL)
        {
            fprintf(stderr, "Failed to read line %d from file: %s\n", i, fileName1);
            fclose(file);
            exit(EXIT_FAILURE);
        }
    }

    fprintf(stderr, "\nError in %s:%d\n%sAt token: %s\n", fileName1, lineNumber, line, enum_names[tokens[iTk].code]);

    va_list va;
    va_start(va, fmt);
    vfprintf(stderr, fmt, va);
    va_end(va);

    fprintf(stderr, "\n");

    fclose(file);
    exit(EXIT_FAILURE);
}

bool consume(int code)
{
    if (tokens[iTk].code == code)
    {
        consumed = &tokens[iTk++];
        return true;
    }
    return false;
}

// // program ::= ( defVar | defFunc | block )* FINISH
bool program()
{
    addDomain();
    addPredefinedFns();

    crtCode = &tMain;
    crtVar = &tBegin;
    Text_write(&tBegin, "#include \"quick.h\"\n\n"); // TODO: adapt the path to the header file
    Text_write(&tMain, "\nint main(){\n");

    while (true)
    {
        if (defVar())
            continue;
        else if (defFunc())
            continue;
        else if (block())
            continue;
        else if (consume(FINISH))
            break;
        else
        {
            tkerr("ERR: Invalid statement in program, expected defVar, defFunc, block, or FINISH");
            return false;
        }
    }

    delDomain();

    Text_write(&tMain, "return 0;\n}\n");
    FILE *fis = fopen("out_gen/out_1.c", "w"); // TODO: adapt the path for the output file
    if (!fis)
    {
        printf("cannot write to file 1.c\n");
        exit(EXIT_FAILURE);
    }
    fwrite(tBegin.buf, sizeof(char), tBegin.n, fis);
    fwrite(tFunctions.buf, sizeof(char), tFunctions.n, fis);
    fwrite(tMain.buf, sizeof(char), tMain.n, fis);
    fclose(fis);

    return true;
}

void parse()
{
    iTk = 0;
    program();
}

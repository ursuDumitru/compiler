#include "../../includes/alex/lexer.h"
#include "../../includes/alex/utils.h"

#include "../../includes/sintactic/parser.h"

#include <stdlib.h>
#include <stdio.h>

int main()
{
    // char fileName1[] = "/home/ursu/Univer/Year3/LFTC/compiler/test_alex/1.q";
    // char fileName2[] = "/home/ursu/Univer/Year3/LFTC/compiler/test_alex/2.q";
    int option;
    printf("\nChoose the file you want to compile(1) or sintactic(2) : ");
    scanf("%d", &option);
    if (option == 1)
        pch = loadFile(fileName1);
    else if (option == 2)
        pch = loadFile(fileName2);
    else
        err("Invalid option !");

    tokenize();
    // showTokens();
    pch = NULL;
    parse();
    // printf("\n pointer : %s", pch);
    // free(pch);
    return 0;
}
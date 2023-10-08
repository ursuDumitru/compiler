#include "../includes/lexer.h"
#include "../includes/utils.h"

#include <stdlib.h>
#include <stdio.h>

int main()
{
    // --------------- developing --------------- //

    int option;
    char *inbuf;

    printf("\nChoose the file you want to compile(1 or 2) : ");
    scanf("%d", &option);
    if(option==1)
        inbuf=loadFile("/home/ursu/Univer/Year3/LFTC/compiler/test/1.q");
    else if(option==2)
        inbuf=loadFile("/home/ursu/Univer/Year3/LFTC/compiler/test/2.q");
    else
        err("Invalid option !");

    // --------------- developing --------------- //

    tokenize(inbuf); // Trebuie de mai schimbat functia asta sa accepte toate caracterele
    showTokens();
    free(inbuf);
    return 0;
}
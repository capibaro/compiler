#include <stdio.h>
#include <stdlib.h>
#include "lex.h"
#include "parse.h"
#include "target.h"
#include "util.h"

char filename[BUF_SIZE];

FILE* open_file(char hint[]) {
    FILE* fptr;
    while (1) {
        printf("%s", hint);
        scanf("%s", filename);
        fptr = fopen(filename, "r");
        if (fptr == NULL) {
            printf("invalid filename!\n");
            fflush(stdin);
            continue;
        } else break;
    }
    return fptr;
}

int main(int argc, char const *argv[])
{
    int op, flag;
    FILE *fsource = NULL;
    FILE *fwords = NULL;
    FILE *fsymbols = NULL;
    FILE *fquadruples = NULL;
    printf("------------naive-complier------------\n");
    flag = 1;
    while (flag) {
        printf("1.lex 2.parse 3.target 4.compile 5.exit\n");
        scanf("%d", &op);
        if (op < 1 || op > 5) {
            printf("invalid option!\n");
            fflush(stdin);
            continue;
        }
        switch (op) {
            case 1:
                fsource = open_file("source file: ");
                lex(fsource);
                printf("word sequence saved in words.txt\n");
                printf("symbol table saved in symbols.txt\n");
                break;
            case 2:
                fwords = open_file("word sequence file: ");
                fsymbols = open_file("symbol table file: ");
                parse(fwords, fsymbols);
                printf("production sequence saved in production.txt\n");
                printf("quadruple sequence saved in quadruples.txt\n");
                printf("symbol table saved in symbols.txt\n");
                break;
            case 3:
                fsymbols = open_file("symbol table file: ");
                fquadruples = open_file("quadruple sequence file: ");
                target(fsymbols, fquadruples);
                printf("assemble code saved in assemble.txt\n");
                break;
            case 4:
                fsource = open_file("source file: ");
                lex(fsource);
                fwords = fopen("words.txt", "r");
                fsymbols = fopen("symbols.txt", "r");
                parse(fwords, fsymbols);
                fsymbols = fopen("symbols.txt", "r");
                fquadruples = fopen("quadruples.txt", "r");
                target(fsymbols, fquadruples);
                printf("assemble code saved in assemble.txt\n");
                break;
            case 5:
                flag = 0;
                break;
        }
    }
    
}

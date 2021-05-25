#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "util.h"
#include "lex.h"

int pos = 0;
int symbol_count = 0;
int buf_len = 0;
int end_of_file = FALSE;
char buf[BUF_SIZE];

void rollback() {
    if (end_of_file == FALSE) pos--;
}

char getNextChar(FILE* fsource) {
    if (pos < buf_len) {
        return buf[pos++]; 
    } else {
        if (fgets(buf, BUF_SIZE, fsource) != 0) {
            buf_len = strlen(buf);
            pos = 0;
            return buf[pos++];
        } else {
            end_of_file = TRUE;
            return EOF;
        }
    }
}

Word matchReservedWord(char *string) {
    for (int i = 0; i < RESERVED_WORD_CNT; i++) {
        if (strcmp(string, reservedWords[i].string) == 0) {
            return reservedWords[i].word;
        }
    }
    return ID;
}

void addSymbol(const char *symbol) {
    int already_exist = 0;
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symbol, symbol_table[i].symbol) == 0) {
            already_exist = TRUE;
        }
    }
    if (already_exist == FALSE) {
        strcpy(symbol_table[symbol_count++].symbol, symbol);
    }
}

void printSimpleSymbols() {
    FILE *fsymbols = fopen("symbols.txt", "w");
    for (int i = 0; i < symbol_count; i++) {
        fprintf(fsymbols, "(%s, ?, ?)\n", symbol_table[i].symbol);
    }
    fclose(fsymbols);
}

void printWord(FILE* fwords, Word word, const char *string) {
    switch (word) {
        case INT:
            fprintf(fwords, "(INT, %s)\n", string);
            break;
        case VOID:
            fprintf(fwords, "(VOID, %s)\n", string);
            break;
        case IF:
            fprintf(fwords, "(IF, %s)\n", string);
            break;
        case ELSE:
            fprintf(fwords, "(ELSE, %s)\n", string);
            break;
        case WHILE:
            fprintf(fwords, "(WHILE, %s)\n", string);
            break;
        case RETURN:
            fprintf(fwords, "(RETURN, %s)\n", string);
            break;
        case ID:
            fprintf(fwords, "(ID, %s)\n", string);
            addSymbol(string);
            break;
        case NUM:
            fprintf(fwords, "(NUM, %s)\n", string);
            break;
        case LBRK:
            fprintf(fwords, "(LBRK, [)\n");
            break;
        case RBRK:
            fprintf(fwords, "(RBRK, ])\n");
            break;
        case LBRACE:
            fprintf(fwords, "(LBRACE, {)\n");
            break;
        case RBRACE:
            fprintf(fwords, "(RBRACE, })\n");
            break;
        case LPAREN:
            fprintf(fwords, "(LPAREN, ()\n");
            break;
        case RPAREN:
            fprintf(fwords, "(RPAREN, ))\n");
            break;
        case GTE:
            fprintf(fwords, "(GTE, >=)\n");
            break;
        case LTE:
            fprintf(fwords, "(LTE, <=)\n");
            break;
        case NEQ:
            fprintf(fwords, "(NEQ, !=)\n");
            break;
        case EQ:
            fprintf(fwords, "(EQ, ==)\n");
            break;
        case GT:
            fprintf(fwords, "(GT, >)\n");
            break;
        case LT:
            fprintf(fwords, "(LT, <)\n");
            break;
        case ASSIGN:
            fprintf(fwords, "(ASSIGN, =)\n");
            break;
        case PLUS:
            fprintf(fwords, "(PLUS, +)\n");
            break;
        case MINUS:
            fprintf(fwords, "(MINUS, -)\n");
            break;
        case TIMES:
            fprintf(fwords, "(TIMES, *)\n");
            break;
        case DIV:
            fprintf(fwords, "(DIV, /)\n");
            break;
        case COMMA:
            fprintf(fwords, "(COMMA, ,)\n");
            break;
        case SEMICOLON:
            fprintf(fwords, "(SEMICOLON, ;)\n");
            break;
        case END:
            break;
        case ERROR:
            fprintf(fwords, "(ERROR, %s)\n", string);
            break;
        default:
            fprintf(fwords, "(UNKNOWN, %d)\n", word);
    }
}

Word getCurrentWord(char ch) {
    Word currentWord;
    switch (ch) {
        case '+':
            currentWord = PLUS;
            break;
        case '-':
            currentWord = MINUS;
            break;
        case '*':
            currentWord = TIMES;
            break;
        case '(':
            currentWord = LPAREN;
            break;
        case ')':
            currentWord = RPAREN;
            break;
        case '[':
            currentWord = LBRK;
            break;
        case ']':
            currentWord = RBRK;
            break;
        case '{':
            currentWord = LBRACE;
            break;
        case '}':
            currentWord = RBRACE;
            break;
        case ',':
            currentWord = COMMA;
            break;
        case ';':
            currentWord = SEMICOLON;
            break;
        default:
            currentWord = ERROR;
            break;
    }
    return currentWord;
}

Word getWord(FILE* fsource, FILE* fwords) {
    int flag;
    int index = 0;
    Word currentWord;
    State state = START;
    char str[STR_LEG];
    while (state != DONE) {
        char ch = getNextChar(fsource);
        flag = TRUE;
        switch (state) {
            case START: {
                if (isdigit(ch)) state = ISNUM;
                else if (isalpha(ch)) state = ISID;
                else if ((ch == ' ') || (ch == '\t') || (ch == '\n')) flag = FALSE;
                else if (ch == '=') state = ISEEQ;
                else if (ch == '>') state = ISGTE;
                else if (ch == '<') state = ISLTE;
                else if (ch == '!') state = ISNEQ;
                else if (ch == '/') state = ISLCOM;
                else if (ch == EOF) {
                    state = DONE;
                    flag = FALSE;
                    currentWord = END;
                } else {
                    state = DONE;
                    currentWord = getCurrentWord(ch);
                }
                break;
            }
            case ISLCOM: {
                if (ch == '*') {
                    index = 0;
                    flag = FALSE;
                    state = ISCOMMENT;
                } else if (ch == EOF) {
                    state = DONE;
                    currentWord = END;
                } else {
                    currentWord = DIV;
                    state = DONE;
                }
                break;
            }
            case ISCOMMENT: {
                flag = FALSE;
                if (ch == '*') state = ISRCOM;
                else if (ch == EOF) {
                    state = DONE;
                    currentWord = END;
                    pos--;
                }
                break;
            }
            case ISRCOM: {
                flag = FALSE;
                if (ch == '/') state = START;
                else if (ch == EOF) {
                    state = DONE;
                    currentWord = END;
                } else state = ISCOMMENT;
                break;
            }
            case ISNUM: {
                if (!isdigit(ch)) {
                    rollback();
                    flag = FALSE;
                    state = DONE;
                    currentWord = NUM;
                }
                break;
            }
            case ISID: {
                if (!(isalpha(ch) || ch == '_')) {
                    rollback();
                    flag = FALSE;
                    state = DONE;
                    currentWord = ID;
                }
                break;
            }
            case ISEEQ: {
                if (ch == '=') {
                    state = DONE;
                    currentWord = EQ;
                } else {
                    rollback();
                    flag = FALSE;
                    state = DONE;
                    currentWord = ASSIGN;
                }
                break;
            }
            case ISLTE: {
                if (ch == '=') {
                    state = DONE;
                    currentWord = LTE;
                } else {
                    rollback();
                    flag = FALSE;
                    state = DONE;
                    currentWord = LT;
                }
                break;
            }
            case ISGTE: {
                if (ch == '=') {
                    state = DONE;
                    currentWord = GTE;
                } else {
                    rollback();
                    flag = FALSE;
                    state = DONE;
                    currentWord = GT;
                }
                break;
            }
            case ISNEQ: {
                if (ch == '=') {
                    state = DONE;
                    currentWord = NEQ;
                } else {
                    rollback();
                    flag = FALSE;
                    state = DONE;
                    currentWord = ERROR;
                }
                break;
            }
            case DONE: {
                break;
            }
            default: {
                printf("ERROR: state=%d\n", state);
                state = DONE;
                currentWord = ERROR;
                break;
            }
        }
        if ((flag) && (index <= WORD_LEG)) {
            str[index++] = ch;
        }
        if (state == DONE) {
            str[index] = 0;
            if (currentWord == ID) {
                currentWord = matchReservedWord(str);
            }
        }
    }
    printWord(fwords, currentWord, str);
    return currentWord;
}

void lex(FILE* fsource) {
    FILE* fwords = fopen("words.txt", "w");
    while (getWord(fsource, fwords) != END);
    printSimpleSymbols();
    fclose(fsource);
    fclose(fwords);
}
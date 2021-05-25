#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "stack.h"
#include "parse.h"
#include "util.h"

int action_table[ACTION_TABLE_ROW][ACTION_TABLE_COL];
char action[ACTION_CNT][ACTION_LEG];
int action_count;

int goto_table[GOTO_TABLE_ROW][GOTO_TABLE_COL];
int goto_state[GOTO_CNT];
int goto_state_count;

int id_cnt;
int addr;
int addrs[GOTO_TABLE_COL];
int tag[GOTO_TABLE_COL];

char buf[BUF_SIZE];
    
void readAnalysis() {
    int row, col, head, tail;
    FILE *fanalysis = NULL;
    char data[DATA_LEG];
    fanalysis = fopen("analysis.csv", "r");
    if (fanalysis == NULL) {
        printf("missing analysis.csv\n");
        exit(-1);
    }
    while (fgets(buf, BUF_SIZE, fanalysis) != 0) {
        if (isdigit(buf[0])) {
            row = atoi(buf);
            col = 0;
            head = (row < 10) ? 2 : 3;
            while (buf[head] != 0) {
                if (col == 15 || buf[head] == '\n') break;
                if (buf[head] == ',') {
                    col++;
                    head++;
                } else {
                    tail = head;
                    while (buf[tail] != ',' && buf[tail] != '\n') tail++;
                    memmove(data, &buf[head], tail - head);
                    data[tail - head] = 0;
                    if (col < 10) {
                        action_count++;
                        action_table[row][col] = action_count;
                        memmove(action[action_count], data, strlen(data));
                    } else {
                        goto_state_count++;
                        goto_table[row][col - 10] = goto_state_count;
                        goto_state[goto_state_count] = atoi(data);
                    }
                    head = tail;
                }
            }
        }
    }
    fclose(fanalysis);
}

void readWords(FILE *fwords) {
    int word, str;
    while (fgets(buf, BUF_SIZE, fwords) != 0) {
        word = 1;
        while (buf[word] != ',') word++;
        memmove(words[word_count].word, &buf[1], word - 1);
        str = word + 2;
        while (buf[str] != '\n') str++;
        memmove(words[word_count].str, &buf[word + 2], str - word - 3);
        word_count++;
    }
    fclose(fwords);
}

void readSimpleSymbols(FILE *fsymbols) {
    symbol_count = 0;
    memset(symbol_table, 0, SYMBOL_CNT*sizeof(aSymbol));
    int pos;
    while (fgets(buf, BUF_SIZE, fsymbols) != 0) {
        pos = 1;
        while (buf[pos] != ',') pos++;
        memmove(symbol_table[symbol_count].symbol, &buf[1], pos - 1);
        symbol_count++;
    }
    fclose(fsymbols);
}

int getTerminal(int count) {
    int terminal;
    if (strcmp(words[count].word, "PLUS") == 0) terminal = TPLUS;
    else if (strcmp(words[count].word, "ASSIGN") == 0) terminal = TASSIGN;
    else if (strcmp(words[count].word, "MINUS") == 0) terminal = TMINUS;
    else if (strcmp(words[count].word, "TIMES") == 0) terminal = TTIMES;
    else if (strcmp(words[count].word, "LPAREN") == 0) terminal = TLPAREN;
    else if (strcmp(words[count].word, "RPAREN") == 0) terminal = TRPAREN;
    else if (strcmp(words[count].word, "ID") == 0) terminal = TID;
    else if (strcmp(words[count].word, "NUM") == 0) terminal = TNUM;
    else if (strcmp(words[count].word, "INT") == 0) terminal = TINT;
    else if (strcmp(words[count].word, "SEMICOLON") == 0) terminal = TSEMICOLON;
    else {
        printf("unknown termial %s!\n", words[count].word);
        exit(-1);
    }
    return terminal;
}

int getVariable(char *str) {
    int variable;
    if (str[0] == 'S') {
        if (str[1] == '\'') variable = SQUOTE;
        else variable = S;
    } else if (str[0] == 'A') variable = A;
    else if (str[0] == 'B') variable = B;
    else if (str[0] == 'C') variable = C;
    else {
        printf("unknown variable %s!\n", str);
        exit(-1);
    }
    return variable;
}

int getPopNum(int act_num) {
    int pop_num = 1;
    char operator[5] = "+-*=)";
    for (int i = 0; i < strlen(operator); i++) {
        if (strchr(&action[act_num][12], operator[i])) {
            pop_num = 3;
            break;
        }
    }
    if (strchr(&action[act_num][12], 'i')) {
        pop_num = 2;
    }
    return pop_num;
}

int getSymbolIndex(char *str) {
    int i;
    for (i = 0; i < symbol_count; i++) {
        if (strcmp(str, symbol_table[i].symbol) == 0) break;
    }
    return (i == symbol_count) ? -1 : i;
}

void gencode(int addr1, int addr2, int addr3, char op) {
    quadruples[quadruple_count].arg1 = addr1;
    quadruples[quadruple_count].arg2 = addr2;
    quadruples[quadruple_count].arg3 = addr3;
    quadruples[quadruple_count].op = op;
    quadruple_count++;
}

void translate(int act_num, Stack *sta) {
    char op;
    char *str;
    char integer[4] = "int";
    int index, variable1, variable2, variable3, addr1, addr2;
    if (strchr(&action[act_num][12], 'i') && strchr(&action[act_num][12], 'n') && strchr(&action[act_num][12], 't')) {
        str = sta->top->str;
        index = getSymbolIndex(str);
        memmove(symbol_table[index].type, integer, 4);
        symbol_table[index].addr = addr;
        addr += 4;
    } else if (strchr(&action[act_num][12], '(') && strchr(&action[act_num][12], ')')) {
        variable1 = getVariable(&action[act_num][7]);
        variable2 = getVariable(&action[act_num][14]);
        addrs[variable1] = addrs[variable2];
        tag[variable1] = tag[variable2];
    } else if (strchr(&action[act_num][12], '+') || strchr(&action[act_num][12], '-') || strchr(&action[act_num][12], '*')) {
        variable1 = getVariable(&action[act_num][12]);
        variable2 = getVariable(&action[act_num][16]);
        variable3 = getVariable(&action[act_num][7]);
        op = action[act_num][14];
        if (id_cnt == 0) {
            addr1 = quadruples[quadruple_count - 2].arg3;
            addr2 = quadruples[quadruple_count - 1].arg3;
        } else if (id_cnt == 1) {
            addr1 = (tag[variable1]) ? addrs[variable1] : quadruples[quadruple_count - 1].arg3;
            addr2 = (tag[variable2]) ? addrs[variable2] : quadruples[quadruple_count - 1].arg3;
        } else if (id_cnt >= 2) {
            addr1 = addrs[variable1];
            addr2 = addrs[variable2];
        }
        gencode(addr1, addr2, addr, op);
        addr += 4;
        id_cnt = 0;
        tag[variable3] = 0;
    } else if (strchr(&action[act_num][12], '=')) {
        str = sta->top->prev->prev->str;
        index = getSymbolIndex(str);
        addr1 = quadruples[quadruple_count - 1].arg3;
        gencode(addr1, -1, symbol_table[index].addr, '=');
    } else {
        if (strchr(&action[act_num][12], 'I') && strchr(&action[act_num][12], 'D')) {
            variable1 = getVariable(&action[act_num][7]);
            str = sta->top->str;
            index = getSymbolIndex(str);
            addrs[variable1] = symbol_table[index].addr;
            tag[variable1] = 1;
            id_cnt++;
        } else {
            variable1 = getVariable(&action[act_num][7]);
            variable2 = getVariable(&action[act_num][12]);
            addrs[variable1] = addrs[variable2];
            tag[variable1] = tag[variable2];
        }
    }
}

void lr1() {
    int word_num = 0;
    int pop_num, act_num, goto_num, terminal, variable;
    Stack *stack;
    FILE *fproduction = NULL;
    fproduction = fopen("production.txt", "w");
    while (word_num < word_count) {
        stack = InitStack();
        Push(stack, 0, ";");
        while (1) {
            terminal = getTerminal(word_num);
            act_num = action_table[stack->top->state][terminal];
            if (act_num != 0) {
                if (action[act_num][0] == 's') {
                    Push(stack, atoi(&action[act_num][6]), words[word_num].str);
                    word_num++;
                } else if (action[act_num][0] == 'r') {
                    fprintf(fproduction, "%s\n", &action[act_num][7]);
                    translate(act_num, stack);
                    variable = getVariable(&action[act_num][7]);
                    pop_num = getPopNum(act_num);
                    for (int i = 0; i < pop_num; i++) {
                        Pop(stack);
                    }
                    goto_num = goto_table[stack->top->state][variable];
                    Push(stack, goto_state[goto_num], words[word_num].str);
                } else if (action[act_num][0] == 'a') {
                    word_num++;
                    break;
                }
            }
        }
    }
    fclose(fproduction);
}

void printQuad() {
    FILE *fquadruples = NULL;
    fquadruples = fopen("quadruples.txt", "w");
    int index1, index2, index3;
    for (int i = 0; i < quadruple_count; i++) {
        if (quadruples[i].arg2 == -1) {
            index1 = quadruples[i].arg3 / 4;
            index2 = quadruples[i].arg1 / 4;
            if (index1 < symbol_count) {
                if (index2 < symbol_count) {
                    fprintf(fquadruples, "%s%c%s\n", symbol_table[index1].symbol, quadruples[i].op, symbol_table[index2].symbol);
                } else {
                    fprintf(fquadruples, "%s%ct%d\n", symbol_table[index1].symbol, quadruples[i].op, index2 - symbol_count);
                }
            } else {
                if (index2 < symbol_count) {
                    fprintf(fquadruples, "t%d%c%s\n", index1 - symbol_count, quadruples[i].op, symbol_table[index2].symbol);
                } else {
                    fprintf(fquadruples, "t%d%ct%d\n", index1 - symbol_count, quadruples[i].op, index2 - symbol_count);
                }
            }
        } else {
            index1 = quadruples[i].arg3 / 4;
            index2 = quadruples[i].arg1 / 4;
            index3 = quadruples[i].arg2 / 4;
            if (index1 < symbol_count) {
                if (index2 < symbol_count) {
                    if (index3 < symbol_count) {
                        fprintf(fquadruples, "%s=%s%c%s\n", symbol_table[index1].symbol, symbol_table[index2].symbol, quadruples[i].op, symbol_table[index3].symbol);
                    } else {
                        fprintf(fquadruples, "%s=%s%ct%d\n", symbol_table[index1].symbol, symbol_table[index2].symbol, quadruples[i].op, index3 - symbol_count);
                    }
                } else {
                    if (index3 < symbol_count) {
                        fprintf(fquadruples, "%s=t%d%c%s\n", symbol_table[index1].symbol, index2 - symbol_count, quadruples[i].op, symbol_table[index3].symbol);
                    } else {
                        fprintf(fquadruples, "%s=t%d%ct%d\n", symbol_table[index1].symbol, index2 - symbol_count, quadruples[i].op, index3 - symbol_count);
                    }
                }
            } else {
                if (index2 < symbol_count) {
                    if (index3 < symbol_count) {
                        fprintf(fquadruples, "t%d=%s%c%s\n", index1 - symbol_count, symbol_table[index2].symbol, quadruples[i].op, symbol_table[index3].symbol);
                    } else {
                        fprintf(fquadruples, "t%d=%s%ct%d\n", index1 - symbol_count, symbol_table[index2].symbol, quadruples[i].op, index3 - symbol_count);
                    }
                } else {
                    if (index3 < symbol_count) {
                        fprintf(fquadruples, "t%d=t%d%c%s\n", index1 - symbol_count, index2 - symbol_count, quadruples[i].op, symbol_table[index3].symbol);
                    } else {
                        fprintf(fquadruples, "t%d=t%d%ct%d\n", index1 - symbol_count, index2 - symbol_count, quadruples[i].op, index3 - symbol_count);
                    }
                }
            }
        }
    }
    fclose(fquadruples);
}

void printExtendedSymbols() {
    FILE *fsymbols = fopen("symbols.txt", "w");
    for (int i = 0; i < symbol_count; i++) {
        fprintf(fsymbols, "(%s, %s, %d)\n", symbol_table[i].symbol, symbol_table[i].type, symbol_table[i].addr);
    }
    fclose(fsymbols);
}

void parse(FILE *fwords, FILE *fsymbols) {
    readAnalysis();
    readWords(fwords);
    readSimpleSymbols(fsymbols);
    lr1();
    printQuad();
    printExtendedSymbols();
}
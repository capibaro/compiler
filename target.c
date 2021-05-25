#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "target.h"
#include "util.h"

int regs[REGISTER_CNT];
int reg_count;

int addr;

char buf[BUF_SIZE];

int getOperandIndex(char *str) {
    int i;
    for (i = 0; i < symbol_count; i++) {
        if (strcmp(str, symbol_table[i].symbol) == 0) {
            break;
        }
    }
    return (i == symbol_count) ? -1 : i;
}

int new_temp(char *symbol) {
    int index;
    char integer[4] = "int";
    memmove(symbol_table[symbol_count].symbol, symbol, strlen(symbol));
    memmove(symbol_table[symbol_count].type, integer, strlen(integer));
    symbol_table[symbol_count].addr = addr;
    index = symbol_count;
    symbol_count++;
    addr += 4;
    return index;
}

void readQuad(FILE *fquadruples) {
    quadruple_count = 0;
    memset(quadruples, 0, QUADRUPLE_CNT*sizeof(aQuadruple));
    int head, tail, index;
    char operand[SYMBOL_LEG];
    while (fgets(buf, BUF_SIZE, fquadruples) != 0) {
        head = 0;
        tail = head;
        while (buf[tail] != '=') tail++;
        memmove(operand, &buf[head], tail - head);
        operand[tail - head] = 0;
        index = getOperandIndex(operand);
        if (index < 0) index = new_temp(operand);
        quadruples[quadruple_count].arg3 = symbol_table[index].addr;
        
        head = tail + 1;
        tail = head;
        while (buf[tail] != '+' && buf[tail] != '-' && buf[tail] != '*' && buf[tail] != '\n') tail++;
        memmove(operand, &buf[head], tail - head);
        operand[tail - head] = 0;
        index = getOperandIndex(operand);
        if (index < 0) index = new_temp(operand);
        quadruples[quadruple_count].arg1 = symbol_table[index].addr;

        if (buf[tail] == '\n') {
            quadruples[quadruple_count].op = '=';
            quadruples[quadruple_count].arg2 = -1;
            quadruple_count++;
            break;
        } else {
            quadruples[quadruple_count].op = buf[tail];
            head = tail + 1;
            tail = head;
            while (buf[tail] != '\n') tail++;
            memmove(operand, &buf[head], tail - head);
            operand[tail - head] = 0;
            index = getOperandIndex(operand);
            if (index < 0) index = new_temp(operand);
            quadruples[quadruple_count].arg2 = symbol_table[index].addr;
        }
        quadruple_count++;
    }
    fclose(fquadruples);
}

void readExtendedSymbols(FILE *fsymbols) {
    symbol_count = 0;
    memset(symbol_table, 0, SYMBOL_CNT*sizeof(aSymbol));
    int head, tail;
    while (fgets(buf, BUF_SIZE, fsymbols) != 0) {
        head = 1;
        tail = head;
        while (buf[tail] != ',') tail++;
        memmove(symbol_table[symbol_count].symbol, &buf[head], tail - head);
        
        head = tail + 2;
        tail = head;
        while (buf[tail] != ',') tail++;
        memmove(symbol_table[symbol_count].type, &buf[head], tail - head);
        
        head = tail + 2;
        tail = head;
        while (buf[tail] != ')') tail++;
        symbol_table[symbol_count].addr = atoi(&buf[head]);
        addr += 4;
        symbol_count++;
    }
    fclose(fsymbols);
}

int findReg(int addr) {
    int reg = -1;
    for (int i = 0; i < reg_count; i++) {
        if (regs[i] == addr) {
            reg = i;
            break;
        }
    }
    return reg;
}

int getReg(int addr) {
    int reg = findReg(addr);
    if (reg == -1) {
        regs[reg_count] = addr;
        reg = reg_count;
        reg_count++;
    }
    return reg;
}

char *getSymbol(int addr) {
    char *symbol = NULL;
    for (int i = 0; i < symbol_count; i++) {
        if (symbol_table[i].addr == addr) {
            symbol = symbol_table[i].symbol;
        }
    }
    return symbol;
}

void assemble() {
    char *symbol1, *symbol2;
    int reg1, reg2;
    FILE *fassemble = NULL;
    fassemble = fopen("assemble.txt", "w");
    for (int i = 0; i < quadruple_count; i++) {
        if (quadruples[i].op != '=') {
            symbol1 = getSymbol(quadruples[i].arg1);
            if (findReg(quadruples[i].arg1) == -1) {
                reg1 = getReg(quadruples[i].arg1);
                fprintf(fassemble, "MOV %s, R%d\n", symbol1, reg1);
            } else {
                reg1 = getReg(quadruples[i].arg1);
            }
            symbol2 = getSymbol(quadruples[i].arg2);
            if (findReg(quadruples[i].arg2) != -1) {
                reg2 = getReg(quadruples[i].arg2);
                if (quadruples[i].op == '-') {
                    fprintf(fassemble, "SUB R%d, R%d\n", reg2, reg1);
                } else if (quadruples[i].op == '+') {
                    fprintf(fassemble, "ADD R%d, R%d\n", reg2, reg1);
                } else if (quadruples[i].op == '*') {
                    fprintf(fassemble, "MUL R%d, R%d\n", reg2, reg1);
                }
            } else {
                if (quadruples[i].op == '-') {
                    fprintf(fassemble, "SUB %s, R%d\n", symbol2, reg1);
                } else if (quadruples[i].op == '+') {
                    fprintf(fassemble, "ADD %s, R%d\n", symbol2, reg1);
                } else if (quadruples[i].op == '*') {
                    fprintf(fassemble, "MUL %s, R%d\n", symbol2, reg1);
                }
            }
            regs[reg1] = quadruples[i].arg3;
        } else {
            symbol1 = getSymbol(quadruples[i].arg3);
            reg1 = findReg(quadruples[i].arg1);
            fprintf(fassemble, "MOV R%d, %s\n", reg1, symbol1);
        }
    }
    fclose(fassemble);
}

void target(FILE *fsymbols, FILE *fquadruples) {
    readExtendedSymbols(fsymbols);
    readQuad(fquadruples);
    assemble();
}

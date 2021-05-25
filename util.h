#define TRUE 1
#define FALSE 0
#define SYMBOL_LEG 10
#define SYMBOL_CNT 32
#define TYPE_LEG 10
#define QUADRUPLE_CNT 10
#define BUF_SIZE 256
#define STR_LEG 10
#define WORD_LEG 10

struct {
    char symbol[SYMBOL_LEG];
    char type[TYPE_LEG];
    int addr;
} aSymbol, symbol_table[SYMBOL_CNT];
int symbol_count;

struct {
    char op;
    int arg1;
    int arg2;
    int arg3;
} aQuadruple, quadruples[QUADRUPLE_CNT];
int quadruple_count;

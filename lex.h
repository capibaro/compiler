#define RESERVED_WORD_CNT 6

typedef enum {
    INT, VOID, IF, ELSE, WHILE, RETURN, ID, NUM, LBRK, RBRK, 
    LBRACE, RBRACE, LPAREN, RPAREN, GTE, LTE, NEQ, EQ, GT, LT, 
    ASSIGN, PLUS, MINUS, TIMES, DIV, COMMA, SEMICOLON, END, ERROR
} Word;

typedef enum {
    START, ISRCOM, ISLCOM, ISCOMMENT, ISNUM, 
    ISID, DONE, ISLTE, ISGTE, ISEEQ, ISNEQ
} State;

static struct {
    Word word;
    char *string;
} reservedWords[RESERVED_WORD_CNT] = {
    { INT, "int" }, 
    { VOID, "void" }, 
    { IF, "if" }, 
    { ELSE, "else" }, 
    { WHILE, "while" },
    { RETURN, "return" }
};

void lex(FILE* fsource);
#define ACTION_TABLE_ROW 33
#define ACTION_TABLE_COL 10
#define ACTION_CNT 330
#define ACTION_LEG 20
#define GOTO_TABLE_ROW 33
#define GOTO_TABLE_COL 5
#define GOTO_CNT 165
#define WORD_CNT 100
#define WORD_LEN 10
#define STR_LEG 10
#define DATA_LEG 32

typedef enum {
    TPLUS, TASSIGN, TMINUS, TTIMES, TLPAREN,
    TRPAREN, TID, TNUM, TINT, TSEMICOLON,
} Terminal;

typedef enum {
    S, A, B, C, SQUOTE,
} Variable;

struct {
    char word[WORD_LEN];
    char str[STR_LEG];
} words[WORD_CNT];
int word_count;

void parse(FILE *fwords, FILE *fsymbols);
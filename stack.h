#define LEN 10

typedef struct snode {
    int state;
    char str[LEN];
    struct snode *prev;
} StackNode;

typedef struct {
    StackNode *top;
    int size;
} Stack;

Stack *InitStack();

int isEmpty(Stack *sta);

void Push(Stack *sta, int state, char str[]);

void Pop(Stack *sta);
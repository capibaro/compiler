# Compiler

A naive C-like language compiler consisting of lexical scanner, LR(1) parser and target code generator.

## Grammer

```
[Variable]
S A B C S'
[Terminal]
+ = - * ( ) ID NUM int
[Start]
E'
[Production]
E' -> S';
S' -> int ID;
S' -> ID = S;
S -> S + A;
S -> S - A;
S -> A;
A -> A * B;
A -> B;
B -> ( S );
B -> ID;
B -> NUM;
```

## Build and Run

### build

```
gcc lex.c parse.c stack.c target.c main.c -o main
```

### run

```
.\main
```

## Example

### source

```
int a;
int b;
int c;
int d;
d = (a + b) - (b * c);
```

### word

```
(INT, int)
(ID, a)
(SEMICOLON, ;)
(INT, int)
(ID, b)
(SEMICOLON, ;)
(INT, int)
(ID, c)
(SEMICOLON, ;)
(INT, int)
(ID, d)
(SEMICOLON, ;)
(ID, d)
(ASSIGN, =)
(LPAREN, ()
(ID, a)
(PLUS, +)
(ID, b)
(RPAREN, ))
(MINUS, -)
(LPAREN, ()
(ID, b)
(TIMES, *)
(ID, c)
(RPAREN, ))
(SEMICOLON, ;)
```

### symbol

```
(a, int, 0)
(b, int, 4)
(c, int, 8)
(d, int, 12)
```

### production

```
S' -> int ID
S' -> int ID
S' -> int ID
S' -> int ID
B -> ID
A -> B
S -> A
B -> ID
A -> B
S -> S + A
B -> ( S )
A -> B
S -> A
B -> ID
A -> B
B -> ID
A -> A * B
S -> A
B -> ( S )
A -> B
S -> S - A
S' -> ID = S
```

### quadruple

```
t0=a+b
t1=b*c
t2=t0-t1
d=t2
```

### assemble

```
MOV a, R0
ADD b, R0
MOV b, R1
MUL c, R1
SUB R1, R0
MOV R0, d
```
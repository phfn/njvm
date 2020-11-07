#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define VERSION 0
#define DEBUG

#define TRUE 1
#define FALSE 0

#define ERROR 1
#define STACKOVERFLOW_ERROR 2
#define STACKUNDERFLOW_ERROR 3
#define DIVISIONBYZERO_ERROR 4

#define STACK_SIZE 100
#define SIZE_OF_INT 4

#define HALT 0
#define PUSHC 1

#define ADD 2
#define SUB 3
#define MUL 4
#define DIV 5
#define MOD 6

#define RDINT 7
#define WRINT 8
#define RDCHR 9
#define WRCHR 10

#define IMMEDIATE(x) ((x) & 0x00FFFFFF)
//immediate belegt nur die letzten 24 bit, die ersten 8 werden auf 0 gesetzt.

#define SIGN_EXTEND(i) ((i) & 0x00800000 ? (i) | 0xFF000000 : (i))
//if (i&0x00800000){i= i | 0xFF000000} else{i=i}
//wenn erstes immediate gesetz (also negative zahl im immediate) -> setzen auch die ersten 8 bit damit die Zahl auch als negativ betrachtet wird


unsigned int *prog_mem;

unsigned int pc=0;//Programm Counter

int stack[STACK_SIZE];
int *sp=stack;//Stack Pointer



void push(int value){
    if(sp > &stack[STACK_SIZE]){
        printf("STACKOVERFLOW_ERROR");
        exit(STACKOVERFLOW_ERROR);
    }

    *sp = value;
    sp = sp + 1;//rechnet plus vier, ich weiß nich genau warum, muss aber auch 4 rechnen um zu funzen...
}


int pull(){
    if(sp <= stack){
        printf("STACKOVERFLOW_ERROR");
        exit(STACKOVERFLOW_ERROR);
    }
    sp = sp - 1;//siehe ln 57
    int res = *sp;

    return res;
}

void add(){
    int y = pull();
    int x = pull();

    push(x+y);
}

void sub(){
    int y = pull();
    int x = pull();

    push(x-y);
}

void mul(){
    int y = pull();
    int x = pull();

    push(x*y);
}

void division(){
    int y = pull();
    int x = pull();

    if (y == 0){
        printf("Error: Division by 0\n");
        exit(DIVISIONBYZERO_ERROR);
    }

    push(x / y);
}

void mod(){
    int y = pull();
    int x = pull();

    if (y == 0){
        printf("Error: Division by 0\n");
        exit(DIVISIONBYZERO_ERROR);
    }
    push(x % y);
}

void rdint(){
    int input = 0;
    scanf("%d", &input);

    push(input);
}

void wrint(){
    int output = pull();
    printf("%d", output);
}

void rdchr(){
    char input = 0;
    scanf("%c", &input);

    push(input);
}

void wrchr(){
    int output = pull();
    printf("%c", (char)output);
}


unsigned int halt_bool=FALSE;
void exec(int IR){
    int code=IR>>24;
    int imm=SIGN_EXTEND(IMMEDIATE(IR));
    switch (code) {
        case HALT:
            halt_bool=TRUE;
            break;

        case PUSHC:
            push(imm);
            break;

        case ADD:
            add();
            break;

        case SUB:
            sub();
            break;

        case MUL:
            mul();
            break;

        case DIV:
            division();
            break;

        case MOD:
            mod();
            break;

        case RDINT:
            rdint();
            break;

        case WRINT:
            wrint();
            break;

        case RDCHR:
            rdchr();
            break;

        case WRCHR:
            wrchr();
            break;

        default:
            printf("NOT IMPLEMENTED YET");
    }
}



void run(){
    int IR;
    while(!halt_bool){
        IR=prog_mem[pc];
        pc=pc+1;
        exec(IR);
    }
}

unsigned int prog_1[] = {
        (PUSHC << 24) | IMMEDIATE(3),
        (PUSHC << 24) | IMMEDIATE(4),
        (ADD << 24),
        (PUSHC << 24) | IMMEDIATE(10),
        (PUSHC << 24) | IMMEDIATE(6),
        (SUB << 24),
        (MUL << 24),
        (WRINT << 24),
        (PUSHC << 24) | IMMEDIATE(10),
        (WRCHR << 24),
        (HALT << 24)
};

unsigned int prog_2[] = {
        (PUSHC << 24) | IMMEDIATE(-2),
        (RDINT << 24),
        (MUL << 24),
        (PUSHC << 24) | IMMEDIATE(3),
        (ADD << 24),
        (WRINT << 24),
        (PUSHC << 24) | IMMEDIATE(10),
        (WRCHR << 24),
        (HALT << 24)
};

unsigned int prog_3[] = {
        (RDCHR << 24),
        (WRINT << 24),
        (PUSHC << 24) | IMMEDIATE(10),
        (WRCHR << 24),
        (HALT << 24)
};

unsigned int prog_halt[]={(HALT << 24)};

int main(int argc, char* argv[]){

    if (argc > 1){
        if (strcmp(argv[1], "--help") == 0){
            printf("usage: ../njvm [option] [option] ...\n"
                   "  --version        show version and exit\n"
                   "  --help           show this help and exit\n");
            exit(0);
        }else if (strcmp(argv[1], "--version") == 0) {
            printf("Ninja Virtual Machine version 0\n");
            exit(0);
        }else if (strcmp(argv[1], "--prog1") == 0) {
            prog_mem = prog_1;
        }else if (strcmp(argv[1], "test") == 0) {
            printf("RESERVED FOR TESTING PURPOSE\n");
            printf("%x", SIZE_OF_INT);
            exit(0);
        }else{
            printf("unknown command line argument '%s', try '%s --help'\n",argv[1], argv[0]);
            exit(0);
        }
    }else{

        prog_mem=prog_halt;
    }
    printf("Ninja Virtual Machine started\n");
    run();
    printf("Ninja Virtual Machine stopped\n");
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VERSION 2
#define DEBUG

#define TRUE 1
#define FALSE 0

#define ERROR 1
#define STACKOVERFLOW_ERROR 2
#define STACKUNDERFLOW_ERROR 3
#define DIVISION_BY_ZERO_ERROR 4
#define FRAME_POINTER_INVALID 5

#define STACK_SIZE 100

#define HALT   0
#define PUSHC  1

#define ADD    2
#define SUB    3
#define MUL    4
#define DIV    5
#define MOD    6

#define RDINT  7
#define WRINT  8
#define RDCHR  9
#define WRCHR 10

#define PUSHG 11
#define POPG  12
#define ASF   13
#define RSF   14
#define PUSHL 15
#define POPL  16

#define IMMEDIATE(x) ((x) & 0x00FFFFFF)
//immediate belegt nur die letzten 24 bit, die ersten 8 werden auf 0 gesetzt.

#define SIGN_EXTEND(i) ((i) & 0x00800000 ? (i) | 0xFF000000 : (i))
//if (i&0x00800000){i= i | 0xFF000000} else{i=i}
//wenn erstes immediate gesetz (also negative zahl im immediate) -> setzen auch die ersten 8 bit damit die Zahl auch als negativ betrachtet wird

char* opCodes[]={"HALT ", "PUSHC", "ADD  ", "SUB  ", "MUL  ", "DIV  ", "MOD  ", "RDINT", "WRINT", "RDCHR", "WRCHR", "PUSHG", "POPG", "ASF", "RSF", "PSUHL", "POPL"};

int sda[];

unsigned int *prog_mem;

unsigned int pc=0;//Programm Counter

int stack[STACK_SIZE];
int *sp=stack;//Stack Pointer
int *fp=stack;//frame pointer


void push(int value){
    if(sp > &stack[STACK_SIZE]){
        printf("STACKOVERFLOW_ERROR");
        exit(STACKOVERFLOW_ERROR);
    }

    *sp = value;
    sp = sp + 1;//rechnet plus vier, ich weiß nich genau warum, muss aber auch 4 rechnen um zu funzen...
}


int pop(){
    if(sp <= stack){
        printf("STACKUNDERFLOW_ERROR");
        exit(STACKUNDERFLOW_ERROR);
    }
    sp = sp - 1;//siehe ln 57
    int res = *sp;

    return res;
}



unsigned int halt_bool=FALSE;
void exec(unsigned int IR){
    int opcode=IR >> 24;
    int imm=SIGN_EXTEND(IMMEDIATE(IR));
    int y;
    int x;

    switch (opcode) {
        case HALT:
            halt_bool=TRUE;
            break;

        case PUSHC:
            push(imm);
            break;

        case ADD:
            y = pop();
            x = pop();

            push(x+y);
            break;

        case SUB:
            y = pop();
            x = pop();

            push(x-y);
            break;

        case MUL:
            y = pop();
            x = pop();

            push(x*y);
            break;

        case DIV:
            y = pop();
            x = pop();

            if (y == 0){
                printf("Error: Division by 0\n");
                exit(DIVISION_BY_ZERO_ERROR);
            }

            push(x / y);
            break;

        case MOD:
            y = pop();
            x = pop();

            if (y == 0){
                printf("Error: Division by 0\n");
                exit(DIVISION_BY_ZERO_ERROR);
            }
            push(x % y);
            break;

        case RDINT:
            scanf("%d", &x);

            push(x);
            break;

        case WRINT:
            x = pop();
            printf("%d", x);
            break;

        case RDCHR:
            x=getchar();
            push(x);
            break;

        case WRCHR:
            x = pop();
            printf("%c", x);
            break;
        case PUSHG :
            push(sda[imm]);
            break;

        case POPG:
            sda[imm]=pop();
            break;

        case ASF:
            if(fp>=sp) exit(FRAME_POINTER_INVALID);
            if(sp+imm>&stack[STACK_SIZE]) exit(STACKOVERFLOW_ERROR);
            if(sp+imm<stack) exit(STACKUNDERFLOW_ERROR);
            push(fp);
            fp = sp;
            sp = sp + imm;
            break;

        case RSF:
            sp = fp;
            fp = pop();
            break;

        case PUSHL:
            *(fp +imm) = pop();
            break;

        case POPL:
            push(*(fp + imm));
            break;


        default:
            printf("ERROR NOT IMPLEMENTED YET");
    }
}



void run(){
    unsigned int IR;
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

void print_prog_mem(){
    unsigned int *p=&prog_mem[0];
    int opcode;
    unsigned int IR;
    int imm;
    do{
        IR=*p;
        opcode=IR >> 24;
        imm=SIGN_EXTEND(IMMEDIATE(IR));
        printf("%s %d\n", opCodes[opcode], imm);
        p++;
    }while(opcode!=HALT);
}

unsigned int prog_halt[]={(HALT << 24)};

int main(int argc, char* argv[]){

    if (argc > 1){
        for(int i=1; i< argc; i++){
            if (strcmp(argv[i], "--help") == 0){
                printf("usage: ../njvm [option] [option] ...\n"
                       "  --version        show version and exit\n"
                       "  --help           show this help and exit\n");
                exit(0);
            }else if (strcmp(argv[i], "--version") == 0) {
                printf("Ninja Virtual Machine version %d\n", VERSION);
                exit(0);
            }else if (strcmp(argv[i], "--prog1") == 0) {
                prog_mem = prog_1;
                break;
            }else if (strcmp(argv[i], "--prog2") == 0) {
                prog_mem = prog_2;
                break;
            }else if (strcmp(argv[i], "--prog3") == 0) {
                prog_mem = prog_3;
                break;
            }else if (strcmp(argv[i], "--test") == 0) {
                printf("RESERVED FOR TESTING PURPOSE\n");
                exit(0);
            }else if (strcmp(argv[i], "--compile") == 0) {
                char* imput_file = argv[i+1];
                char* output_file = argv[i+2];
                system("chmod +x nja");
                char command[100];
                sprintf(command, "./nja %s %s", imput_file, output_file);
                printf(command);
                system(command);
                i++;
                exit(0);
            }else{
                printf("unknown command line argument '%s', try '%s --help'\n",argv[1], argv[0]);
                exit(0);
            }
        }
    }else{

        prog_mem=prog_halt;
    }
    print_prog_mem();
    printf("Ninja Virtual Machine started\n");
    run();
    printf("Ninja Virtual Machine stopped\n");
    return 0;
}

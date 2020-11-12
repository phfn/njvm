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

#define FILE_NOT_FOUND_ERROR 5
#define WRONG_FILE_SIZE_ERROR 6
#define NJBF_ERROR 7
#define WRONG_VERSION_ERROR 8
#define MEMORY_FULL_ERROR 9

#define NO_CODE_FILE_ARGUMENT_ERROR 10
#define UNKNOWN_ARGUMENT_ERROR 11

#define STACK_SIZE 100

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

char* opCodes[]={"HALT ", "PUSHC", "ADD  ", "SUB  ", "MUL  ", "DIV  ", "MOD  ", "RDINT", "WRINT", "RDCHR", "WRCHR"};
//char* opCodes[]={"halt ", "pushc", "add  ", "sub  ", "mul  ", "div  ", "mov  ", "rdint", "wrint", "rdchr", "wrchr"};

int stack[STACK_SIZE];
int *sp=stack;//Stack Pointer
unsigned int pc=0;//Programm Counter

unsigned int *prog_mem;
unsigned int prog_mem_size;

int *sda;
unsigned int sda_size;


void push(int value){
    if(sp > &stack[STACK_SIZE]){
        printf("STACKOVERFLOW_ERROR");
        exit(STACKOVERFLOW_ERROR);
    }

    *sp = value;
    sp = sp + 1;
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

void load_memory(char *filepath){
    FILE *file;
    file = fopen(filepath, "r");

    if (!file){
        printf("Error: cannot open code file '%s'\n", filepath);
        exit(FILE_NOT_FOUND_ERROR);
    }


    unsigned int njbf;
    if (fread(&njbf, sizeof(unsigned int), 1, file) != 1){
        printf("Error: cannot read code file '%s'\n", filepath);
        exit(WRONG_FILE_SIZE_ERROR);
    }
    if (njbf != 0x46424a4e){
        printf("Error: file '%s' is not a Ninja binary\n", filepath);
        exit(NJBF_ERROR);
    }


    unsigned version;
    if (fread(&version, sizeof(unsigned int), 1, file) != 1){
        printf("Error: cannot read code file '%s'\n", filepath);
        exit(WRONG_FILE_SIZE_ERROR);
    }
    if (version != VERSION){
        printf("Error: file '%s' has wrong version number\n", filepath);
        exit(WRONG_VERSION_ERROR);
    }


    if (fread(&prog_mem_size, sizeof(unsigned int), 1, file) != 1){
        printf("Error: cannot read code file '%s'\n", filepath);
        exit(WRONG_FILE_SIZE_ERROR);
    }

    prog_mem = malloc(sizeof(unsigned int) * prog_mem_size);
    if (!prog_mem){
        printf("Error: No free heap memory\n");
        exit(MEMORY_FULL_ERROR);
    }


    if (fread(&sda_size, sizeof(unsigned int), 1, file) != 1){
        printf("Error: cannot read code file '%s'\n", filepath);
        exit(WRONG_FILE_SIZE_ERROR);
    }

    sda = malloc(sizeof(unsigned int) * sda_size);
    if (!sda){
        printf("Error: No free heap memory\n");
        exit(MEMORY_FULL_ERROR);
    }


    for (int i=0; i<prog_mem_size; i++){
        if (fread(&prog_mem[i], sizeof(unsigned int), 1, file) != 1) {
            printf("Error: cannot read code file '%s'\n", filepath);
            exit(WRONG_FILE_SIZE_ERROR);
        }
    }

    fclose(file);
}

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


int main(int argc, char* argv[]){

    if (argc < 2) {
        printf("Error: no code file specified\n");
        exit(NO_CODE_FILE_ARGUMENT_ERROR);
    }
    if (strcmp(argv[1], "--help") == 0){
        printf("usage: %s [options] <code file>\n"
               "  --version        show version and exit\n"
               "  --help           show this help and exit", argv[0]);
        exit(0);
    }
    else if (strcmp(argv[1], "--version") == 0){
        printf("Ninja Virtual Machine version %d (compiled %s, %s)\n", VERSION, __DATE__, __TIME__);
        exit(0);
    }
    else if (strcmp(argv[1], "--test") == 0){
        printf("RESERVED FOR TESTING PURPOSE\n");
        exit(0);
    }
    else if (argv[1][0] == '-'){
        printf("unknown command line argument '%s', try '%s --help'\n", argv[1], argv[0]);
        exit(UNKNOWN_ARGUMENT_ERROR);
    }
    else{
        load_memory(argv[1]);
    }

    printf("Ninja Virtual Machine started\n");

    print_prog_mem();
    run();

    printf("Ninja Virtual Machine stopped\n");
    return 0;
}

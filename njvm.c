#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VERSION 3
//#define DEBUG

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
#define INVALID_ARGUMENTS_ERROR 10

#define INVALID_JUMP_ERROR 11

#define FRAME_POINTER_INVALID 12
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

#define EQ    17
#define NE    18
#define LT    19
#define LE    20
#define GT    21
#define GE    22
#define JMP   23
#define BRF   24
#define BRT   25

#define IMMEDIATE(x) ((x) & 0x00FFFFFF)
//immediate belegt nur die letzten 24 bit, die ersten 8 werden auf 0 gesetzt.

#define SIGN_EXTEND(i) ((i) & 0x00800000 ? (i) | 0xFF000000 : (i))
//if (i&0x00800000){i= i | 0xFF000000} else{i=i}
//wenn erstes immediate gesetz (also negative zahl im immediate) -> setzen auch die ersten 8 bit damit die Zahl auch als negativ betrachtet wird

char* opCodes[]={"HALT ", "PUSHC", "ADD  ", "SUB  ", "MUL  ", "DIV  ", "MOD  ", "RDINT", "WRINT", "RDCHR", "WRCHR", "PUSHG", "POPG ", "ASF  ", "RSF  ", "PUSHL", "POPL ",
                 "EQ   ", "NE   ", "LT   ", "LE   ", "GT   ", "GE   ", "JMP  ", "BRF  ", "BRT  "};

int stack[STACK_SIZE];
int sp=0;//Stack Pointer
int fp=0;//frame pointer

unsigned int pc=0;//Programm Counter
unsigned int *prog_mem;
unsigned int prog_mem_size;

int *sda;
unsigned int sda_size;


void push(int value){
    if(sp > STACK_SIZE){
        printf("STACKOVERFLOW_ERROR\n");
        exit(STACKOVERFLOW_ERROR);
    }

    stack[sp] = value;
    sp++;
}


int pop(){
    if(sp <= 0){
        printf("STACKUNDERFLOW_ERROR\n");
        exit(STACKUNDERFLOW_ERROR);
    }
    sp--;

    return stack[sp];
}

void print_stack(void) {
    printf("\n Stack\n");
    printf(".-------+--------.\n");
    for (int i=sp; i>=0; i--) {
        if (i==sp)
            printf("|sp->%3d| <empty>|\n", i);
        else if(i==fp){
            printf("|fp->%3d| %6d |\n", i,stack[i]);
        }else{
            printf("|%7d| %6d |\n", i, stack[i]);
        }

    }
    printf("'-------+--------'\n\n");
}


unsigned int halt_bool=FALSE;
void exec(unsigned int IR){
    int opcode=IR >> 24;
    int imm=SIGN_EXTEND(IMMEDIATE(IR));
    int y;
    int x;

#ifdef DEBUG

    print_stack();
    printf("opCode: %s\n"
           "imm:    %d\n", opCodes[opcode], imm);
#endif
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
            if(fp>sp) exit(FRAME_POINTER_INVALID);
            if(sp+imm>STACK_SIZE) exit(STACKOVERFLOW_ERROR);
            if(sp+imm<0) exit(STACKUNDERFLOW_ERROR);
            push(fp);
            fp = sp;
            sp = sp + imm;
            break;

        case RSF:
            sp = fp;
            fp = pop();
            break;

        case PUSHL:
            push(stack[fp + imm]);
            break;

        case POPL:

            stack[fp +imm] = pop();
            break;

        case EQ:
            y = pop();
            x = pop();
            push(x == y);
            break;

        case NE:
            y = pop();
            x = pop();
            push(x != y);
            break;

        case LT:
            y = pop();
            x = pop();
            push(x < y);
            break;

        case LE:
            y = pop();
            x = pop();
            push(x <= y);
            break;

        case GT:
            y = pop();
            x = pop();
            push(x > y);
            break;

        case GE:
            y = pop();
            x = pop();
            push(x >= y);
            break;

        case JMP:
            if ((imm > 0) && (imm < prog_mem_size))
                pc = imm;
            else {
                printf("Error: Jump target out of bounds\n");
                exit(INVALID_JUMP_ERROR);
            }
            break;

        case BRF:
            if (pop() == FALSE){
                if ((imm > 0) && (imm < prog_mem_size))
                    pc = imm;
                else {
                    printf("Error: Jump target out of bounds\n");
                    exit(INVALID_JUMP_ERROR);
                }
            }
            break;

        case BRT:
            if (pop() == TRUE){
                if ((imm > 0) && (imm < prog_mem_size))
                    pc = imm;
                else {
                    printf("Error: Jump target out of bounds\n");
                    exit(INVALID_JUMP_ERROR);
                }
            }
            break;

        default:
            printf("ERROR NOT IMPLEMENTED YET");
    }
}


void load_prog_memory(char *filepath){
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


    unsigned version_file;
    if (fread(&version_file, sizeof(unsigned int), 1, file) != 1){
        printf("Error: cannot read code file '%s'\n", filepath);
        exit(WRONG_FILE_SIZE_ERROR);
    }
    if (version_file != VERSION){
        printf("Error: file '%s' has wrong version_file number\n", filepath);
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
    int opcode;
    unsigned int IR;
    int imm;
    for(int i=0; i < prog_mem_size; i++) {
        IR = prog_mem[i];
        opcode = IR >> 24;
        imm = SIGN_EXTEND(IMMEDIATE(IR));
        if(i==pc){
            printf("[%2d]: %s %3d <---pc\n", i , opCodes[opcode], imm);
        }else{
            printf("[%2d]: %s %3d\n", i , opCodes[opcode], imm);
        }
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

void print_data(){
    for(int i=0; i< sda_size; i++){
        printf("|%2d| %6d |\n", i, sda[i]);
    }
}

void debug(){
    int opcode;
    unsigned int IR;
    int imm;
    int breakpoint=-1;
    printf("Welcome to the njvm Debugger\n Please use only the first character of any command. i for inspect, b17r for breakpoint at 17 und run\n");
    while(!halt_bool){
        IR = prog_mem[pc];
        opcode = IR >> 24;
        imm = SIGN_EXTEND(IMMEDIATE(IR));
        printf("[%2d]: %s %3d\n", pc , opCodes[opcode], imm);
        printf("(i)nspect, (l)ist, (b)reakpoint, (s)tep, (r)un, (q)uit?\n");
        char c;
        scanf(" %c",&c);
        switch (c){
            case 'i'://inspect
                printf("(s)tack or (d)ata?\n");
                scanf(" %c",&c);
                switch (c) {
                    case 's'://stack
                        print_stack();
                        break;
                    case 'd'://data
                        print_data();
                        break;
                };
                break;
            case 'l'://list
                print_prog_mem();
                break;
            case 'b'://breakpoint
                printf("address to set, -1 to clear, now its %d\n", breakpoint);
                scanf(" %d", &breakpoint);
                printf("set to %d\n", breakpoint);
                break;
            case 's'://step
                pc=pc+1;
                exec(IR);
                break;
            case 'r'://run
                while(!halt_bool && pc!=breakpoint){

                    IR=prog_mem[pc];
                    pc=pc+1;
                    exec(IR);

                }
                break;

            case  'q'://quit
                halt_bool=TRUE;
                break;

        };

    }
}
int main(int argc, char* argv[]){

    if (argc > 1){
        for(int i=1; i< argc; i++){
            if (strcmp(argv[i], "--help") == 0){
                printf("usage: ../njvm [option] [option] file ...\n"
                       "  --version                            show version and exit\n"
                       "  --help                               show this help and exit\n"
                       "  --assemble <inputfile> <outputfile>   compile an asm file to bin and run the bin\n"
                       "  file                                 run program");
                exit(0);
            }else if (strcmp(argv[i], "--version") == 0) {
                printf("Ninja Virtual Machine version %d\n", VERSION);
                exit(0);
            }else if (strcmp(argv[i], "--assemble") == 0) {
                char* input_file = argv[i + 1];
                char* output_file = argv[i+2];
                char nja_path[256];
                char chmod_cmd[256];
                char nja_cmd[256];
                sprintf(nja_path, "aufgaben/a%d/nja", VERSION);
                sprintf(chmod_cmd, "chmod +x %s", nja_path);
                system(chmod_cmd);
                sprintf(nja_cmd, "./%s %s %s", nja_path, input_file, output_file);
                system(nja_cmd);
                i++;
            }else if (strcmp(argv[i], "--debug") == 0) {
                printf("Ninja Virtual Machine started\n");
                debug();
                printf("Ninja Virtual Machine stopped\n");
                return 0;
            }else if (argv[i][0] == '-'){
                printf("unknown command line argument '%s', try '%s --help'\n", argv[1], argv[0]);
                exit(INVALID_ARGUMENTS_ERROR);
            }
            else{
                load_prog_memory(argv[i]);
            }
        }
    }else{
        printf("try --help");
        exit(INVALID_ARGUMENTS_ERROR);
    }
    printf("Ninja Virtual Machine started\n");
    //print_prog_mem();
    run();
    printf("Ninja Virtual Machine stopped\n");
    return 0;
}

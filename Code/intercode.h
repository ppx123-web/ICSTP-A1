//#define INTERCODE_DEBUG



#ifndef INTERCODE_H
#define INTERCODE_H


#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "datastructure.h"
#include <stdint.h>

#define new(A) ((A*)malloc(sizeof(A)))


typedef struct CodeList_t CodeList_t;
typedef struct Operand Operand;
typedef struct InterCode InterCode;


static char * temp_var_prefix = "t", * label_name = "label";

struct Operand {
    enum {
        NONTYPE, VARIABLE, CONSTANT, ADDRESS, FUNCTION, GOTO, ORIGIN, RELOP, DEC, INT_CONST,
    } kind;
    union {
        struct {
            int var_no,use_info;
        };
        char * value;
        char * f_name;
        int goto_id;
        char * id_name;
        char * relop;
        int dec;
        int int_const;
    } var;
};

struct InterCode {
    enum {
        T_EMPTY, T_LABEL, T_FUNCTION, T_ASSIGN, T_ADD, T_MINUS, T_MUL, T_DIV, T_ADDR,
        T_A_STAR, T_STAR_A, T_GO, T_IF, T_RETURN, T_DEC, T_ARG, T_CALL,
        T_PARAM, T_READ, T_WRITE,
    } kind;
    int line,block;
    union {
        struct { Operand id; } label;
        struct { Operand arg1; } go;
        struct { Operand func; } function;
        struct { Operand arg1; } ret;
        struct { Operand arg1; } arg;
        struct { Operand arg1; } param;
        struct { Operand arg1; } read;
        struct { Operand arg1; } write;
        struct { Operand arg1,arg2; } assign;
        struct { Operand arg1,arg2; } a_star;
        struct { Operand arg1,arg2; } star_a;
        struct { Operand arg1,arg2; } addr;
        struct { Operand arg1,arg2; } dec;
        struct { Operand arg1,arg2; } call;
        struct { Operand arg1,arg2,arg3; } add;
        struct { Operand arg1,arg2,arg3; } minus;
        struct { Operand arg1,arg2,arg3; } mul;
        struct { Operand arg1,arg2,arg3; } div;
        struct { Operand arg1,arg2,arg3,op; } ifgoto;
        struct { Operand arg1,arg2,arg3,arg4; };
        struct { Operand args[4]; };
    } op;
    InterCode * next, * prev;
};

struct CodeList_t {
    InterCode head,tail;
};

int genvar();
void codelist_init(CodeList_t * this);
void codelist_insert(CodeList_t * this,InterCode * pos, InterCode * cur);
void codelist_display(CodeList_t * this);
void intercode_display(InterCode * cur);
void operand_display(Operand * op);

CodeList_t * code_optimizer(int size);

Operand genoperand(int kind,...);
InterCode * u_gencode(int kind,va_list ap);



#endif

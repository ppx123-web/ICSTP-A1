//#define INTERCODE_DEBUG



#ifndef INTERCODE_H
#define INTERCODE_H

#include <stdarg.h>

typedef struct CodeList_t CodeList_t;
typedef struct Operand Operand;
typedef struct InterCode InterCode;


static char * temp_var_prefix = "t", * label_name = "label";

struct Operand {
    enum {
        NONTYPE, VARIABLE, CONSTANT, ADDRESS, FUNCTION, GOTO, ORIGIN, RELOP, DEC, INT_CONST,
    } kind;
    union {
        int var_no;
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


typedef struct vector {
    int size;
    int capacity;
    void * data;
    int ele_size;

}vector;

void vector_init(vector * vec,int ele_size,int capacity);
void vector_resize(vector * vec,int size);
void vector_push_back(vector * vec,void * udata);
void vector_delete(vector * vec);
void * vector_id(vector * vec,int id);
int vector_size(vector *vec);


typedef struct hashmap_node_t {
    void * keydata, * valuedata;
    struct hashmap_node_t * next;
}hashmap_node_t;

typedef struct hashmap {
    int size;
    int bucket_capacity;
    hashmap_node_t * bucket;
    int key_size;
    int value_size;
    int unit_size;


    int (*hash)(struct hashmap * map,void * keydata,int size);
    int (*compare)(void * ka,void * kb);
}hashmap;

void hashmap_init(hashmap * map,int bucket_capacity,int key_size,int value_size,
                  int (*hash)(struct hashmap * map,void * keydata,int size),int (*compare)(void * ka,void * kb));
void hashmap_delete(hashmap * map,void * keydata);
void hashmap_set(hashmap * map,void * keydata,void * valuedata);
void * hashmap_find(hashmap * map,void * keydata);

void hashmap_deconstruct(hashmap * map);
void hashmap_traverse(hashmap * map,void (* func)(void *,void *));


#endif

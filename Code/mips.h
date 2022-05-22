#ifndef MIPS_H
#define MIPS_H

#define MIN(A,B) ((A) < (B)?(A):(B))
#define MAX(A,B) ((A) > (B)?(A):(B))



#include <stdio.h>
#include <stdlib.h>

typedef struct reg_info_t {
    char * name;
    int var,use_info;
}reg_info_t;

typedef struct regs_t {
    union {
        reg_info_t regs[40];
        struct {
            reg_info_t zero;
            reg_info_t at;
            reg_info_t v[2];
            reg_info_t a[4];
            reg_info_t t[8];
            reg_info_t s[8];
            reg_info_t t8,t9;
            reg_info_t k[2];
            reg_info_t gp;
            reg_info_t sp;
            reg_info_t fp;
            reg_info_t ra;
        };
    };
    int use_start, use_end;
}regs_t;

typedef struct mips_op_t {
    enum {
        M_EMPTY, M_LABEL, M_FUNCTION, M_ASSIGN_VAR, M_ASSIGN_CONST, M_ADD, M_ADDI, M_MINUS, M_MUL, M_DIV, M_ADDR,
        M_A_STAR, M_STAR_A, M_GO, M_RETURN, M_DEC, M_ARG, M_CALL,
        M_PARAM, M_READ, M_WRITE,
        M_IF_EQ, M_IF_NEQ, M_IF_GR, M_IF_LE, M_IF_GEQ, M_IF_LEQ,
    }kind;
}mips_op_t;

typedef struct stack_node_t {
    int offset;
}stack_node_t;

static regs_t regs = {
        .zero = {"$zero",0},
        .at = { "$at", 0},
        .v = {
                {"$v0",0},
                {"$v1",0},
        },
        .a = {
                {"$a0",0},
                {"$a1",0},
                {"$a2",0},
                {"$a3",0},
        },
        .t = {
                {"$t0",0},
                {"$t1",0},
                {"$t2",0},
                {"$t3",0},
                {"$t4",0},
                {"$t5",0},
                {"$t6",0},
                {"$t7",0},
        },
        .s = {
                {"$s0",0},
                {"$s1",0},
                {"$s2",0},
                {"$s3",0},
                {"$s4",0},
                {"$s5",0},
                {"$s6",0},
                {"$s7",0},
        },
        .t8 = {"$t8",0},
        .t9 = {"$t9",0},
        .k = {{"$k0",0}, {"$k1",0}},
        .gp = {"$gp",0},
        .sp = {"$sp",0},
        .fp = {"$fp",0},
        .ra = {"$ra",0},
        .use_start = 8,
        .use_end = 25,
};

#endif
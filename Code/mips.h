#ifndef MIPS_H
#define MIPS_H

#include <stdio.h>
#include <stdlib.h>

typedef struct reg_info_t {

}reg_info_t;

typedef struct regs_t {

}regs_t;

typedef struct mips_op_t {
    enum {
        M_EMPTY, M_LABEL, M_FUNCTION, M_ASSIGN_VAR, M_ASSIGN_CONST, M_ADD, M_MINUS, M_MUL, M_DIV, M_ADDR,
        M_A_STAR, M_STAR_A, M_GO, M_RETURN, M_DEC, M_ARG, M_CALL,
        M_PARAM, M_READ, M_WRITE,
        M_IF_EQ, M_IF_NEQ, M_IF_GR, M_IF_LE, M_IF_GEQ, M_IF_LEQ,
    };
}mips_op_t;


#endif
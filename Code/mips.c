#include "mips.h"
#include "intercode.h"
#include "debug.h"
//选择局部寄存器分配算法

/*
阶段：
 123不考虑函数调用的问题
 1. 先假设寄存器有无限多个（编号$t0、$t1、…、$t99、$t100、…），试着完成指令选择，然后将经过指令
选择之后的代码打印出来看一下是否正确。
 2. 完成寄存器分配算法
 3. 完成寄存器分配算法
 4. 完成函数调用
 */


typedef struct Code_Block_t {
    int first_code,end_code;
    InterCode * head,* tail;
}Code_Block_t;

static void show_block(int size,Code_Block_t * code_block) {
    freopen("block.txt","w",stdout);
    for(int i = 0;i < size;i++) {
        printf("block:%d, begin line:%d, end line :%d\n",i,code_block[i].first_code,code_block[i].end_code);
    }
}
static void assembler_init() {}
static int partition_code_block(CodeList_t * cl,Code_Block_t * code_block);
static void block_gen_code(Code_Block_t * cb);
static void emit_code(int kind,...);
static reg_info_t * ensure(Operand op);
static reg_info_t * allocate(Operand op);

void mips_code(CodeList_t * cl) {
    assembler_init();
    int intercode_size = cl->tail.prev->line + 5;
    Code_Block_t * code_block = malloc(sizeof(Code_Block_t) * intercode_size);
    int block_size = partition_code_block(cl,code_block);
    for(int i = 0;i < block_size;i++) {
        block_gen_code(&code_block[i]);
    }
//    show_block(block_size);
    free(code_block);
}


static int partition_code_block(CodeList_t * cl,Code_Block_t * code_block) {
    InterCode * cur = cl->head.next;
    int id = -1;
    code_block[++id].first_code = cur->line;
    code_block[id].head = cur;
    cur = cur->next;
    while (cur != &cl->tail) {
        cur->block = id;
        switch (cur->kind) {
            case T_GO:
            case T_IF:
            case T_RETURN:
                code_block[id].end_code = cur->line;
                code_block[id].tail = cur;
                id++;
                code_block[id].first_code = cur->line + 1;
                code_block[id].head = cur->next;
                break;
            case T_LABEL:
            case T_FUNCTION:
                if (code_block[id].first_code != cur->line) {
                    code_block[id].end_code = cur->line - 1;
                    code_block[id].tail = cur->prev;
                    id++;
                    code_block[id].first_code = cur->line;
                    code_block[id].head = cur;
                }
            default:;
        }
        cur = cur->next;
    }
    return id;
}

static void block_gen_code(Code_Block_t * cb) {

}

static void op_select(Code_Block_t * cb) {
    for(InterCode * cur = cb->head;cur != cb->tail->next; cur = cur->next) {
        switch (cur->kind) {
            case T_LABEL:
                emit_code(M_LABEL,cur->op.label.id);
                break;
            case T_ASSIGN:
                switch (cur->op.arg2.kind) {
                    case VARIABLE:
                        emit_code(M_ASSIGN_VAR, ensure(cur->op.arg1),ensure(cur->op.arg2));
                        break;
                    case INT_CONST:
                        emit_code(M_ASSIGN_CONST, ensure(cur->op.arg1),cur->op.arg2.var.int_const);
                        break;
                    default:
                        assert(0);
                }
                break;
            case T_ADD:
                emit_code(M_ADD, ensure(cur->op.arg1),ensure(cur->op.assign.arg2), ensure(cur->op.arg3));
                break;
            case T_MINUS:
                emit_code(M_MINUS, ensure(cur->op.arg1),ensure(cur->op.assign.arg2), ensure(cur->op.arg3));
                break;
            case T_MUL:
                emit_code(M_MUL, ensure(cur->op.arg1),ensure(cur->op.assign.arg2), ensure(cur->op.arg3));
                break;
            case T_DIV:
                emit_code(M_DIV, ensure(cur->op.arg1),ensure(cur->op.assign.arg2), ensure(cur->op.arg3));
                break;
            case T_A_STAR:
                emit_code(M_A_STAR, ensure(cur->op.arg1),ensure(cur->op.arg2));
                break;
            case T_STAR_A:
                emit_code(M_STAR_A, ensure(cur->op.arg1), ensure(cur->op.arg2));
                break;
            case T_GO:
                emit_code(M_GO, cur->op.arg1.var.goto_id);
                break;
            case T_CALL:
                emit_code(M_GO, ensure(cur->op.arg1), cur->op.arg2.var.f_name);
                break;
            case T_RETURN:
                emit_code(M_RETURN, ensure(cur->op.arg1));
                break;
            case T_READ:
                emit_code(M_READ, ensure(cur->op.arg1));
                break;
            case T_WRITE:
                emit_code(M_WRITE, ensure(cur->op.arg1));
                break;
            case T_IF:
                if(strcmp(cur->op.arg4.var.relop,"==") == 0) {
                    emit_code(M_IF_EQ, ensure(cur->op.arg1), ensure(cur->op.arg2),cur->op.arg3.var.goto_id);
                } else if(strcmp(cur->op.arg4.var.relop,"!=") == 0) {
                    emit_code(M_IF_NEQ, ensure(cur->op.arg1), ensure(cur->op.arg2),cur->op.arg3.var.goto_id);
                } else if(strcmp(cur->op.arg4.var.relop,"<") == 0) {
                    emit_code(M_IF_LE, ensure(cur->op.arg1), ensure(cur->op.arg2),cur->op.arg3.var.goto_id);
                } else if(strcmp(cur->op.arg4.var.relop,">") == 0) {
                    emit_code(M_IF_GR, ensure(cur->op.arg1), ensure(cur->op.arg2),cur->op.arg3.var.goto_id);
                } else if(strcmp(cur->op.arg4.var.relop,"<=") == 0) {
                    emit_code(M_IF_LEQ, ensure(cur->op.arg1), ensure(cur->op.arg2),cur->op.arg3.var.goto_id);
                } else if(strcmp(cur->op.arg4.var.relop,">=") == 0) {
                    emit_code(M_IF_GEQ, ensure(cur->op.arg1), ensure(cur->op.arg2),cur->op.arg3.var.goto_id);
                } else {
                    assert(0);
                }
                break;
            case T_DEC:
                break;
            case T_FUNCTION:
                break;
            case T_PARAM:
                break;
            case T_ARG:
                break;
            case T_ADDR:
                break;
            default:
                panic("Wrong");
        }
    }
}


static void emit_code(int kind,...) {
    va_list ap;
    va_start(ap,kind);
    char * temp;
    switch (kind) {
        case M_LABEL:
            printf("label%d:", va_arg(ap,int));
            break;
        case M_ASSIGN_VAR:
            printf("move %s, %s",va_arg(ap,char*),va_arg(ap,char*));
        case M_ASSIGN_CONST:
            printf("li %s, %d", va_arg(ap,char*), va_arg(ap,int));
            break;
        case M_ADD:
            printf("add %s, %s, %s",va_arg(ap,char*),va_arg(ap,char*),va_arg(ap,char*));
            break;
        case M_MINUS:
            printf("sub %s, %s, %s",va_arg(ap,char*),va_arg(ap,char*),va_arg(ap,char*));
            break;
        case M_MUL:
            printf("mul %s, %s, %s",va_arg(ap,char*),va_arg(ap,char*),va_arg(ap,char*));
            break;
        case M_DIV:
            temp = va_arg(ap,char*);
            printf("div %s, %s\n",va_arg(ap,char*),va_arg(ap,char*));
            printf("mflo %s",va_arg(ap,char*));
            break;
        case M_A_STAR:
            printf("lw %s, 0(%s)",va_arg(ap,char*),va_arg(ap,char*));
            break;
        case M_STAR_A:
            temp = va_arg(ap,char*);
            printf("sw %s, 0(%s)",va_arg(ap,char*),temp);
            break;
        case M_GO:
            printf("j label%d",va_arg(ap,int));
            break;
        case M_CALL:
            printf("jal %s\n", va_arg(ap,char *));
            printf("move %s, $v0", va_arg(ap,char *));
            break;
        case M_RETURN:
            printf("move $v0, %s\n", va_arg(ap,char *));
            printf("jr $ra");
            break;
        case M_IF_EQ:
            printf("beq %s, %s, label%d", va_arg(ap,char *), va_arg(ap,char *), va_arg(ap,int));
            break;
        case M_IF_NEQ:
            printf("bne %s, %s, label%d", va_arg(ap,char *), va_arg(ap,char *), va_arg(ap,int));
            break;
        case M_IF_GR:
            printf("bgt %s, %s, label%d", va_arg(ap,char *), va_arg(ap,char *), va_arg(ap,int));
            break;
        case M_IF_LE:
            printf("blt %s, %s, label%d", va_arg(ap,char *), va_arg(ap,char *), va_arg(ap,int));
            break;
        case M_IF_GEQ:
            printf("bge %s, %s, label%d", va_arg(ap,char *), va_arg(ap,char *), va_arg(ap,int));
            break;
        case M_IF_LEQ:
            printf("ble %s, %s, label%d", va_arg(ap,char *), va_arg(ap,char *), va_arg(ap,int));
            break;
            break;
        case M_READ:
            printf("jal read\n");
            printf("move %s, $v0", va_arg(ap,char *));
            break;
        case M_WRITE:
            printf("jal write\n");
            break;
    }


    va_end(ap);
    printf("\n");
}

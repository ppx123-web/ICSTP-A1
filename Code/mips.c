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
    int stack_size;
}Code_Block_t;

static void show_block(int size,Code_Block_t * code_block) {
    freopen("block.txt","w",stdout);
    for(int i = 0;i < size;i++) {
        printf("block:%d, begin line:%d, end line :%d\n",i,code_block[i].first_code,code_block[i].end_code);
    }
}
static void assembler_init() {
    printf(".data\n"
           "_prompt: .asciiz \"Enter an integer:\"\n"
           "_ret: .asciiz \"\\n\"\n"
           ".globl main\n"
           ".text\n"
           "read:\n"
           "  li $v0, 4\n"
           "  la $a0, _prompt\n"
           "  syscall\n"
           "  li $v0, 5\n"
           "  syscall\n"
           "  jr $ra\n"
           "\n"
           "write:\n"
           "  li $v0, 1\n"
           "  syscall\n"
           "  li $v0, 4\n"
           "  la $a0, _ret\n"
           "  syscall\n"
           "  move $v0, $0\n"
           "  jr $ra\n");
}
static int partition_code_block(CodeList_t * cl,Code_Block_t * code_block);
static void block_gen_code(Code_Block_t * cb);
static void emit_code(int kind,...);
static char * ensure(Operand op);
static int allocate(Operand op);
static void op_select(Code_Block_t * cb);
static void block_scan_info(Code_Block_t * cb,int *);
static void block_write_back();
static stack_node_t * var_offset;

void mips_code(CodeList_t * cl) {
    assembler_init();
    int intercode_size = cl->tail.prev->line + 5;
    int var_size = 2 * genvar();
    var_offset = malloc(var_size * sizeof(stack_node_t));
    memset(var_offset,-1,var_size * sizeof(stack_node_t));
    Code_Block_t * code_block = malloc(sizeof(Code_Block_t) * intercode_size);
    int block_size = partition_code_block(cl,code_block);

    int * var_use_info = malloc(var_size * sizeof(int));
    for(int i = 0;i < block_size;i++) {
        block_scan_info(&code_block[i],var_use_info);
        block_gen_code(&code_block[i]);
        block_write_back();
    }
    free(var_use_info);
//    show_block(block_size);
    free(code_block);
    free(var_offset);
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

static void block_scan_info(Code_Block_t * cb,int * var_use_info) {
    cb->stack_size = 0;
    for(InterCode * cur = cb->head;cur != cb->tail->next; cur = cur->next) {
        if(cur->kind != T_DEC && cur->kind != T_ADDR) {
            for(int i = 0;i < 3;i++) {
                if(cur->op.args[i].kind == VARIABLE &&
                var_offset[cur->op.args[i].var.var_no].offset == -1) {
                    var_offset[cur->op.args[i].var.var_no].offset = cb->stack_size;
                    cb->stack_size += 4;
                }
            }
        } else if(cur->kind == T_DEC){
            int array_size = cur->op.dec.arg2.var.dec;
            var_offset[cur->op.dec.arg1.var.var_no].offset = cb->stack_size;
            cb->stack_size += array_size;
        } else if(cur->kind == T_ADDR) {
            var_offset[cur->op.addr.arg1.var.var_no].offset = cb->stack_size;
            cb->stack_size += 4;
        }
    }

#define ININTERVAL(A) ((A) <= cb->end_code && (A) >= cb->first_code)
    for(InterCode * cur = cb->tail;cur != cb->head->prev; cur = cur->prev) {
        for(int i = 0;i < 3;i++) {
            int var_no = cur->op.args[i].var.var_no;
            if(cur->op.args[i].kind == VARIABLE) {
                if(ININTERVAL(var_use_info[var_no])) {
                    cur->op.args[i].var.use_info = var_use_info[var_no];
                } else {
                    cur->op.args[i].var.use_info = INT32_MAX;
                    var_use_info[var_no] = cur->line;
                }
            }
        }
    }
}

void block_write_back() {

}

static int select_code_ops[10] = {0};
static int select_code_ops_size;
static InterCode * cur_code;

static char * ensure(Operand op) {
    assert(op.kind == VARIABLE);
    for(int i = regs.use_start;i <= regs.use_end;i++) {
        if(op.var.var_no == regs.regs[i].var) {
            if(op.var.use_info == INT32_MAX) {
                select_code_ops[select_code_ops_size++] = i;
            }
            regs.regs[i].use_info = op.var.use_info;
            return regs.regs[i].name;
        }
    }
    int ret = allocate(op);
    if(op.var.use_info == INT32_MAX) {
        select_code_ops[select_code_ops_size++] = ret;
    }
    if(regs.regs[ret].var != op.var.var_no) {
        printf("  lw %s, %d($fp)\n",regs.regs[ret].name, -var_offset[op.var.var_no].offset);
    }
    return regs.regs[ret].name;
}

static int allocate(Operand op) {
    assert(op.kind == VARIABLE);
    for(int i = regs.use_start;i <= regs.use_end;i++) {
        if(regs.regs[i].var == 0 || regs.regs[i].var == op.var.var_no) {
            regs.regs[i].var = op.var.var_no;
            regs.regs[i].use_info = op.var.use_info;
            return i;
        }
    }
    int id = 0, max = 0;
    for(int i = regs.use_start;i <= regs.use_end;i++) {
        if(regs.regs[i].use_info < cur_code->line) {
            id = i;
            break;
        } else if(regs.regs[i].use_info > max){
            max = regs.regs[i].use_info;
            id = i;
        }
    }
    printf("  sw %s, %d($fp)\n", regs.regs[id].name,-var_offset[op.var.var_no].offset);
    regs.regs[id].var = op.var.var_no;
    return id;
}


static void block_gen_code(Code_Block_t * cb) {
    op_select(cb);
}

static void op_select(Code_Block_t * cb) {
#define REGNAME(A) (regs.regs[A].name)
    for(InterCode * cur = cb->head;cur != cb->tail->next; cur = cur->next) {
        cur_code = cur;
        switch (cur->kind) {
            case T_LABEL:
                emit_code(M_LABEL,cur->op.label.id);
                break;
            case T_ASSIGN:
                switch (cur->op.arg2.kind) {
                    case VARIABLE:
                        emit_code(M_ASSIGN_VAR, REGNAME(allocate(cur->op.arg1)),ensure(cur->op.arg2));
                        break;
                    case INT_CONST:
                        emit_code(M_ASSIGN_CONST, REGNAME(allocate(cur->op.arg1)),cur->op.arg2.var.int_const);
                        break;
                    default:
                        assert(0);
                }
                break;
            case T_ADD:
                emit_code(M_ADD, REGNAME(allocate(cur->op.arg1)),ensure(cur->op.assign.arg2), ensure(cur->op.arg3));
                break;
            case T_MINUS:
                emit_code(M_MINUS, REGNAME(allocate(cur->op.arg1)),ensure(cur->op.assign.arg2), ensure(cur->op.arg3));
                break;
            case T_MUL:
                emit_code(M_MUL, REGNAME(allocate(cur->op.arg1)),ensure(cur->op.assign.arg2), ensure(cur->op.arg3));
                break;
            case T_DIV:
                emit_code(M_DIV, REGNAME(allocate(cur->op.arg1)),ensure(cur->op.assign.arg2), ensure(cur->op.arg3));
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
            case T_RETURN:
                emit_code(M_RETURN, ensure(cur->op.arg1));
                break;
            case T_CALL:
                emit_code(M_CALL, REGNAME(allocate(cur->op.arg1)), cur->op.arg2.var.f_name);
                break;
            case T_READ:
                emit_code(M_READ, REGNAME(allocate(cur->op.arg1)));
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
            case T_FUNCTION:
                emit_code(M_FUNCTION, cur->op.arg1.var.f_name);
                if(strcmp(cur->op.arg1.var.f_name,"main") == 0) {
                    printf("  move $fp, $sp\n");
                    printf("  sub $sp, $sp, %d\n",cb->stack_size);
                }
                break;
            case T_DEC:
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
    for(int i = 0;i < select_code_ops_size;i++) {
        regs.regs[select_code_ops[i]].var = 0;
    }
    select_code_ops_size = 0;
    va_list ap;
    va_start(ap,kind);
    char * temp[4];
    int a[4];
    switch (kind) {
        case M_LABEL:
            a[0] = va_arg(ap,int);
            printf("label%d:", a[0]);
            break;
        case M_ASSIGN_VAR:
            temp[0] = va_arg(ap,char*);
            temp[1] = va_arg(ap,char*);
            printf("  move %s, %s",temp[0],temp[1]);
            break;
        case M_ASSIGN_CONST:
            temp[0] = va_arg(ap,char*);
            a[0] = va_arg(ap,int);
            printf("  li %s, %d", temp[0],a[0]);
            break;
        case M_ADD:
            temp[0] = va_arg(ap,char*);
            temp[1] = va_arg(ap,char*);
            temp[2] = va_arg(ap,char*);
            printf("  add %s, %s, %s",temp[0],temp[1],temp[2]);
            break;
        case M_MINUS:
            temp[0] = va_arg(ap,char*);
            temp[1] = va_arg(ap,char*);
            temp[2] = va_arg(ap,char*);
            printf("  sub %s, %s, %s",temp[0],temp[1],temp[2]);
            break;
        case M_MUL:
            temp[0] = va_arg(ap,char*);
            temp[1] = va_arg(ap,char*);
            temp[2] = va_arg(ap,char*);
            printf("  mul %s, %s, %s",temp[0],temp[1],temp[2]);
            break;
        case M_DIV:
            temp[0] = va_arg(ap,char*);
            temp[1] = va_arg(ap,char*);
            temp[2] = va_arg(ap,char*);
            printf("  div %s, %s\n",temp[1],temp[2]);
            printf("  mflo %s",temp[0]);
            break;
        case M_A_STAR:
            temp[0] = va_arg(ap,char*);
            temp[1] = va_arg(ap,char*);
            printf("  lw %s, 0(%s)",temp[0],temp[1]);
            break;
        case M_STAR_A:
            temp[0] = va_arg(ap,char*);
            temp[1] = va_arg(ap,char*);
            printf("  sw %s, 0(%s)",temp[1],temp[0]);
            break;
        case M_GO:
            printf("  j label%d",va_arg(ap,int));
            break;
        case M_RETURN:
            printf("  move $v0, %s\n", va_arg(ap,char *));
            printf("  jr $ra");
            break;
        case M_IF_EQ:
            temp[0] = va_arg(ap,char*);
            temp[1] = va_arg(ap,char*);
            a[2] = va_arg(ap,int);
            printf("  beq %s, %s, label%d", temp[0],temp[1],a[2]);
            break;
        case M_IF_NEQ:
            temp[0] = va_arg(ap,char*);
            temp[1] = va_arg(ap,char*);
            a[2] = va_arg(ap,int);
            printf("  bne %s, %s, label%d", temp[0],temp[1],a[2]);
            break;
        case M_IF_GR:
            temp[0] = va_arg(ap,char*);
            temp[1] = va_arg(ap,char*);
            a[2] = va_arg(ap,int);
            printf("  bgt %s, %s, label%d", temp[0],temp[1],a[2]);
            break;
        case M_IF_LE:
            temp[0] = va_arg(ap,char*);
            temp[1] = va_arg(ap,char*);
            a[2] = va_arg(ap,int);
            printf("  blt %s, %s, label%d", temp[0],temp[1],a[2]);
            break;
        case M_IF_GEQ:
            temp[0] = va_arg(ap,char*);
            temp[1] = va_arg(ap,char*);
            a[2] = va_arg(ap,int);
            printf("  bge %s, %s, label%d", temp[0],temp[1],a[2]);
            break;
        case M_IF_LEQ:
            temp[0] = va_arg(ap,char*);
            temp[1] = va_arg(ap,char*);
            a[2] = va_arg(ap,int);
            printf("  ble %s, %s, label%d", temp[0],temp[1],a[2]);
            break;
        case M_CALL:
            //TODO ARGS
            temp[0] = va_arg(ap,char *);
            temp[1] = va_arg(ap,char *);
            printf("  addi $sp, $sp, -4\n");
            printf("  sw $ra, 0($sp)\n");
            printf("  jal %s\n", temp[1]);
            printf("  lw $ra, 0($sp)\n");
            printf("  addi $sp, $sp, 4\n");
            printf("  move %s, $v0", temp[0]);
            break;
        case M_READ:
            temp[0] = va_arg(ap,char *);
            printf("  addi $sp, $sp, -4\n");
            printf("  sw $ra, 0($sp)\n");
            printf("  jal read\n");
            printf("  lw $ra, 0($sp)\n");
            printf("  addi $sp, $sp, 4\n");
            printf("  move %s, $v0", temp[0]);
            break;
        case M_WRITE:
            temp[0] = va_arg(ap,char *);
            printf("  addi $sp, $sp, -8\n");
            printf("  sw $ra, 0($sp)\n");
            printf("  sw $a0, 4($sp)\n");
            printf("  move $a0, %s\n",temp[0]);
            printf("  jal write\n");
            printf("  lw $ra, 0($sp)\n");
            printf("  lw $a0, 4($sp)\n");
            printf("  addi $sp, $sp, 8\n");
            break;
        case M_FUNCTION:
            temp[0] = va_arg(ap,char *);
            printf("\n%s:", temp[0]);
            break;
        default:
            assert(0);
    }
    va_end(ap);
    printf("\n");
}

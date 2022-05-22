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

extern int * variable_map;

typedef struct Code_Block_t {
    int first_code,end_code;
    InterCode * head,* tail;
    int stack_size,offset;
    int func_stack;
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
static int ensure(Operand op);
static int allocate(Operand op);
static void op_select(Code_Block_t * cb);
static void block_scan_info(Code_Block_t * cb,int *);
static void block_write_back();
static stack_node_t * var_offset;
static Code_Block_t * func_block;
static int block_var_offset_base = 0;

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
    }
    func_block->func_stack = block_var_offset_base;
    for(int i = 0;i < block_size;i++) {
        block_gen_code(&code_block[i]);
    }
//    free(var_use_info);
//    free(code_block);
//    free(var_offset);
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
                break;
            case T_CALL:
                code_block[id].end_code = cur->line - 1;
                code_block[id].tail = cur->prev;
                id++;
                code_block[id].first_code = cur->line;
                code_block[id].head = cur;
                code_block[id].end_code = cur->line;
                code_block[id].tail = cur;
                id++;
                code_block[id].first_code = cur->line + 1;
                code_block[id].head = cur->next;
            default:;
        }
        cur = cur->next;
    }
    return id;
}


static void block_scan_info(Code_Block_t * cb,int * var_use_info) {
    if(cb->head->kind == T_FUNCTION) {
        if(func_block) {
            func_block->func_stack = block_var_offset_base;
        }
        block_var_offset_base = 0;
        func_block = cb;
    }

    cb->stack_size = 0;
    cb->offset = block_var_offset_base;
    for(InterCode * cur = cb->head;cur != cb->tail->next; cur = cur->next) {
        if(cur->kind != T_DEC && cur->kind != T_ADDR) {
            for(int i = 0;i < 3;i++) {
                if(cur->op.args[i].kind == VARIABLE &&
                var_offset[cur->op.args[i].var.var_no].offset == -1) {
                    cb->stack_size += 4;
                    var_offset[cur->op.args[i].var.var_no].offset = cb->stack_size + cb->offset;
                }
            }
        } else if(cur->kind == T_DEC){
//            int array_size = cur->op.dec.arg2.var.dec;
//            cb->stack_size += array_size;
//            var_offset[cur->op.dec.arg1.var.var_no].offset = cb->stack_size + cb->offset;
        } else if(cur->kind == T_ADDR) {
            cb->stack_size += 4;
            var_offset[cur->op.addr.arg1.var.var_no].offset = cb->stack_size + cb->offset;
        } else {
            assert(0);
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

    block_var_offset_base += cb->stack_size;
}

static void reg_free(int id) {
    if(regs.regs[id].use_info == INT32_MAX && regs.regs[id].var > 0) {
        printf("  sw %s, %d($fp)\n",regs.regs[id].name, -var_offset[regs.regs[id].var].offset);
        regs.regs[id].var = 0;
        regs.regs[id].use_info = 0;
    }
}

static void block_write_back() {
    for(int i = regs.use_start;i <= regs.use_end;i++) {
        int var_no = regs.regs[i].var;
        if(regs.regs[i].var > 0) {
            printf("  sw %s, %d($fp)\n",regs.regs[i].name,-var_offset[var_no].offset);
        }
        regs.regs[i].var = 0;
        regs.regs[i].use_info = 0;
    }
}

static InterCode * cur_code;

static int ensure(Operand op) {
    assert(op.kind == VARIABLE);
    for(int i = regs.use_start;i <= regs.use_end;i++) {
        if(op.var.var_no == regs.regs[i].var) {
            regs.regs[i].use_info = op.var.use_info;
            return i;
        }
    }
    int ret = allocate(op);
    printf("  lw %s, %d($fp)\n",regs.regs[ret].name, -var_offset[op.var.var_no].offset);
    return ret;
}

static int allocate(Operand op) {
    assert(op.kind == VARIABLE);
    for(int i = regs.use_start;i <= regs.use_end;i++) {
        if(regs.regs[i].var == op.var.var_no) {
            regs.regs[i].var = op.var.var_no;
            regs.regs[i].use_info = op.var.use_info;
            return i;
        }
    }
    for(int i = regs.use_start;i <= regs.use_end;i++) {
        if(regs.regs[i].var == 0) {
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
    regs.regs[id].use_info = op.var.use_info;
    return id;
}


static void block_gen_code(Code_Block_t * cb) {
    op_select(cb);
    InterCode * cur = cb->tail;
    if((cur->kind != T_IF && cur->kind != T_GO && cur->kind != T_RETURN)) {
        block_write_back();
    }
    printf("\n");
}

static Operand call_args[256] = {0};
static int call_args_size = 0;
static int param_arg_size = 0;

static void op_select(Code_Block_t * cb) {
#define REGNAME(A) (regs.regs[A].name)
    for(InterCode * cur = cb->head;cur != cb->tail->next; cur = cur->next) {
        cur_code = cur;
        if(cur == cb->tail && (cur->kind == T_IF || cur->kind == T_GO || cur->kind == T_RETURN)) {
            block_write_back();
        }
        int ensure_id[3];
        switch (cur->kind) {
            case T_LABEL:
                emit_code(M_LABEL,cur->op.label.id.var.goto_id);
                break;
            case T_ASSIGN:
                switch (cur->op.arg2.kind) {
                    case VARIABLE:
                        ensure_id[0] = ensure(cur->op.arg2);
                        reg_free(ensure_id[0]);
                        emit_code(M_ASSIGN_VAR, REGNAME(allocate(cur->op.arg1)), REGNAME(ensure_id[0]));
                        break;
                    case INT_CONST:
                        emit_code(M_ASSIGN_CONST, REGNAME(allocate(cur->op.arg1)),cur->op.arg2.var.int_const);
                        break;
                    default:
                        assert(0);
                }
                break;
            case T_ADD:
                switch (cur->op.arg3.kind) {
                    case VARIABLE:
                        ensure_id[0] = ensure(cur->op.arg2);
                        ensure_id[1] = ensure(cur->op.arg3);
                        reg_free(ensure_id[0]);
                        reg_free(ensure_id[1]);
                        emit_code(M_ADD, REGNAME(allocate(cur->op.arg1)), REGNAME(ensure_id[0]), REGNAME(ensure_id[1]));
                        break;
                    case INT_CONST:
                        ensure_id[0] = ensure(cur->op.arg2);
                        reg_free(ensure_id[0]);
                        emit_code(M_ADDI, REGNAME(allocate(cur->op.arg1)),REGNAME(ensure_id[0]), cur->op.arg3.var.int_const);
                        break;
                    default:
                        assert(0);
                }
                break;
            case T_MINUS:
                ensure_id[0] = ensure(cur->op.arg2);
                ensure_id[1] = ensure(cur->op.arg3);
                reg_free(ensure_id[0]);
                reg_free(ensure_id[1]);
                emit_code(M_MINUS, REGNAME(allocate(cur->op.arg1)),REGNAME(ensure_id[0]), REGNAME(ensure_id[1]));
                break;
            case T_MUL:
                ensure_id[0] = ensure(cur->op.arg2);
                ensure_id[1] = ensure(cur->op.arg3);
                reg_free(ensure_id[0]);
                reg_free(ensure_id[1]);
                emit_code(M_MUL, REGNAME(allocate(cur->op.arg1)),REGNAME(ensure_id[0]), REGNAME(ensure_id[1]));
                break;
            case T_DIV:
                ensure_id[0] = ensure(cur->op.arg2);
                ensure_id[1] = ensure(cur->op.arg3);
                reg_free(ensure_id[0]);
                reg_free(ensure_id[1]);
                emit_code(M_DIV, REGNAME(allocate(cur->op.arg1)),REGNAME(ensure_id[0]), REGNAME(ensure_id[1]));
                break;
            case T_A_STAR:
                ensure_id[0] = ensure(cur->op.arg2);
                reg_free(ensure_id[0]);
                emit_code(M_A_STAR, REGNAME(allocate(cur->op.arg1)),REGNAME(ensure_id[0]));
                break;
            case T_STAR_A:
                switch (cur->op.arg2.kind) {
                    case VARIABLE:
                        ensure_id[0] = ensure(cur->op.arg2);
                        reg_free(ensure_id[0]);
                        emit_code(M_STAR_A, REGNAME(allocate(cur->op.arg1)),REGNAME(ensure_id[0]));
                        break;
                    case INT_CONST:
                        printf("  li $v1, %d\n",cur->op.arg2.var.int_const);
                        emit_code(M_STAR_A, REGNAME(allocate(cur->op.arg1)), "$v1");
                        break;
                    default:
                        assert(0);
                }
                break;
            case T_GO:
                emit_code(M_GO, cur->op.arg1.var.goto_id);
                break;
            case T_RETURN:
                emit_code(M_RETURN, REGNAME(ensure(cur->op.arg1)));
                break;
            case T_CALL:
                block_write_back();
                if(call_args_size > 4) {
                    int left_size = call_args_size - 4;
                    printf("  addi $sp, $sp, -%d\n",left_size * 4);
                    for(int i = left_size - 1;i >= 0;i--) {
                        printf("  lw $v1, %d($fp)\n",-var_offset[call_args[i].var.var_no].offset);
                        printf("  sw $v1, %d($sp)\n",(left_size - 1 - i) * 4);
                    }
                }
                int less_cnt = MIN(call_args_size,4) - 1;
                int arg_begin = call_args_size > 4? call_args_size - 1:less_cnt;
                int arg_end = call_args_size > 4? call_args_size - 4:0;
                for(int i = arg_begin;i >= arg_end;i--) {
                    ensure_id[0] = ensure(call_args[i]);
                    reg_free(ensure_id[0]);
                    emit_code(M_ASSIGN_VAR, regs.a[arg_begin - i].name, REGNAME(ensure_id[0]));
                    regs.a[arg_begin - i].var = call_args[i].var.var_no;
                    regs.a[arg_begin - i].use_info = regs.a[arg_begin - i].use_info;
                }
                emit_code(M_CALL, REGNAME(allocate(cur->op.arg1)), cur->op.arg2.var.f_name);
                if(call_args_size > 4) {
                    printf("  addi $sp, $sp, %d\n", (call_args_size - 4) * 4);
                }
                call_args_size = 0;
                break;
            case T_READ:
                emit_code(M_READ, REGNAME(allocate(cur->op.arg1)));
                block_write_back();
                break;
            case T_WRITE:
                ensure_id[0] = ensure(cur->op.arg1);
                reg_free(ensure_id[0]);
                emit_code(M_WRITE, REGNAME(ensure_id[0]));
                break;
            case T_IF:
                ensure_id[0] = ensure(cur->op.arg1);
                if(cur->op.arg2.kind == VARIABLE) {
                    ensure_id[1] = ensure(cur->op.arg2);
                } else {
                    ensure_id[1] = 0;
                }
                reg_free(ensure_id[0]);
                reg_free(ensure_id[1]);
                if(strcmp(cur->op.arg4.var.relop,"==") == 0) {
                    emit_code(M_IF_EQ, REGNAME(ensure_id[0]), REGNAME(ensure_id[1]),cur->op.arg3.var.goto_id);
                } else if(strcmp(cur->op.arg4.var.relop,"!=") == 0) {
                    emit_code(M_IF_NEQ, REGNAME(ensure_id[0]), REGNAME(ensure_id[1]),cur->op.arg3.var.goto_id);
                } else if(strcmp(cur->op.arg4.var.relop,"<") == 0) {
                    emit_code(M_IF_LE, REGNAME(ensure_id[0]), REGNAME(ensure_id[1]),cur->op.arg3.var.goto_id);
                } else if(strcmp(cur->op.arg4.var.relop,">") == 0) {
                    emit_code(M_IF_GR, REGNAME(ensure_id[0]), REGNAME(ensure_id[1]),cur->op.arg3.var.goto_id);
                } else if(strcmp(cur->op.arg4.var.relop,"<=") == 0) {
                    emit_code(M_IF_LEQ, REGNAME(ensure_id[0]), REGNAME(ensure_id[1]),cur->op.arg3.var.goto_id);
                } else if(strcmp(cur->op.arg4.var.relop,">=") == 0) {
                    emit_code(M_IF_GEQ, REGNAME(ensure_id[0]), REGNAME(ensure_id[1]),cur->op.arg3.var.goto_id);
                } else {
                    assert(0);
                }
                break;
            case T_FUNCTION:
                param_arg_size = 0;
                emit_code(M_FUNCTION, cur->op.arg1.var.f_name);
                printf("  move $fp, $sp\n");
                printf("  sub $sp, $sp, %d\n",cb->func_stack);
                for(int i = regs.use_start;i <= regs.use_end;i++) {
                    regs.regs[i].var = 0;
                }
                break;
            case T_PARAM:
                if(param_arg_size < 4) {
                    regs.a[param_arg_size].var = cur->op.param.arg1.var.var_no;
                    regs.a[param_arg_size].use_info = cur->op.param.arg1.var.use_info;
                    printf("  sw %s, %d($fp)\n",regs.a[param_arg_size].name,-var_offset[cur->op.param.arg1.var.var_no].offset);
                } else {
                    var_offset[cur->op.param.arg1.var.var_no].offset = -(8 + (param_arg_size - 4) * 4);
                }
                param_arg_size++;
                break;
            case T_ARG:
                call_args[call_args_size++] = cur->op.arg1;
                break;
            case T_DEC:
                printf("  addi $sp, $sp, %d\n",-cur->op.dec.arg2.var.dec);
                break;
            case T_ADDR:
                printf("  sw $sp, %d($fp)\n",-var_offset[cur->op.addr.arg1.var.var_no].offset);
                break;
            default:
                assert(0);
        }
    }
}


static void emit_code(int kind,...) {
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
        case M_ADDI:
            temp[0] = va_arg(ap,char*);
            temp[1] = va_arg(ap,char*);
            printf("  addi %s, %s, %d",temp[0],temp[1], va_arg(ap,int));
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
            printf("  move $sp, $fp\n");
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
            temp[0] = va_arg(ap,char *);
            temp[1] = va_arg(ap,char *);
            printf("  addi $sp, $sp, -8\n");
            printf("  sw $fp, 4($sp)\n");
            printf("  sw $ra, 0($sp)\n");
            printf("  jal %s\n", temp[1]);
            printf("  lw $ra, 0($sp)\n");
            printf("  lw $fp, 4($sp)\n");
            printf("  addi $sp, $sp, 8\n");
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
            printf("  addi $sp, $sp, 8");
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

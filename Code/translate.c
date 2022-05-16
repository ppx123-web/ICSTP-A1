#include "data.h"
#include "debug.h"
#include "intercode.h"
#include <stdio.h>

int * variable_map;

static Type Is_Top_Addr = {
        .kind = REMAINED,
};

CodeList_t code_list;

Operand genoperand(int kind,...) {
    va_list ap;
    Operand ret;
    memset(&ret,0, sizeof(Operand));
    ret.kind = kind;
    va_start(ap,kind);
    switch (kind) {
        case VARIABLE:
            ret.var.var_no = va_arg(ap,int);
            break;
        case CONSTANT:
            ret.var.value = malloc(NAME_LENGTH);
            sprintf(ret.var.value,"%s",va_arg(ap,char *));
            break;
        case INT_CONST:
            ret.var.int_const = va_arg(ap,int);
            break;
        case ADDRESS:
            ret.var.var_no = va_arg(ap,int);
        case FUNCTION:
            ret.var.f_name = malloc(NAME_LENGTH);
            sprintf(ret.var.f_name,"%s",va_arg(ap,char *));
            break;
        case GOTO:
            ret.var.goto_id = va_arg(ap,int);
            break;
        case ORIGIN:
            ret.var.id_name = malloc(NAME_LENGTH);
            sprintf(ret.var.id_name,"%s",va_arg(ap,char *));
            break;
        case RELOP:
            ret.var.relop = malloc(NAME_LENGTH);
            sprintf(ret.var.relop,"%s",va_arg(ap,char *));
            break;
        case DEC:
            ret.var.dec = va_arg(ap,int);
            break;
        default:
            panic("Wrong!");
    }
    va_end(ap);
    return ret;
}

InterCode * u_gencode(int kind,va_list ap) {
    InterCode * code = new(InterCode);
    memset(code,0, sizeof(InterCode));
    code->kind = kind;
    switch (kind) {
        case T_LABEL:
        case T_FUNCTION:
        case T_GO:
        case T_RETURN:
        case T_ARG:
        case T_PARAM:
        case T_READ:
        case T_WRITE:
            code->op.arg1 = va_arg(ap,Operand);
            break;
        case T_ASSIGN:
            if(code->op.arg1.var.var_no == -1) {
                free(code);
                va_end(ap);
                return NULL;
            }
            code->op.arg1 = va_arg(ap,Operand);
            code->op.arg2 = va_arg(ap,Operand);
            break;
        case T_ADDR:
        case T_A_STAR:
        case T_STAR_A:
        case T_DEC:
        case T_CALL:
            code->op.arg1 = va_arg(ap,Operand);
            code->op.arg2 = va_arg(ap,Operand);
            break;
        case T_MINUS:
        case T_MUL:
        case T_DIV:
        case T_ADD:
            code->op.arg1 = va_arg(ap,Operand);
            code->op.arg2 = va_arg(ap,Operand);
            code->op.arg3 = va_arg(ap,Operand);
            if(code->op.arg1.var.var_no == -1) {
                free(code);
                va_end(ap);
                return NULL;
            }
            break;
        case T_IF:
            code->op.arg1 = va_arg(ap,Operand);
            code->op.arg2 = va_arg(ap,Operand);
            code->op.arg3 = va_arg(ap,Operand);
            code->op.arg4 = va_arg(ap,Operand);
            break;
        default:
            panic("Wrong");
    }

    return code;
}

static void gencode(int kind,...) {
    va_list ap;
    va_start(ap,kind);
    InterCode * code = u_gencode(kind,ap);
    va_end(ap);
    if(code) {
        codelist_insert(&code_list,code_list.tail.prev,code);
    }
}

static int is_if = 0;
static int set_on_if() {
    is_if = 1;
}

static int set_on_off() {
    is_if = 0;
}

int genvar() {
    static int var_idx = 1;
    variable_map[var_idx] = is_if;
    return var_idx++;
}

static int genlable() {
    static int label_idx = 1;
    return label_idx++;
}

void codelist_init(CodeList_t * this) {
    this->head.next = &this->tail;
    this->tail.prev = &this->head;
}

static int inter_code_line = 0;

void codelist_insert(CodeList_t * this,InterCode * pos, InterCode * cur) {
    cur->line = ++inter_code_line;
    InterCode * next = pos->next;
    pos->next = cur;
    next->prev = cur;
    cur->next = next;
    cur->prev = pos;
}

void intercode_display(InterCode * cur) {
    switch (cur->kind) {
        case T_LABEL:
            printf("LABEL ");
            operand_display(&cur->op.label.id);
            printf(" :");
            break;
        case T_FUNCTION:
            printf("FUNCTION ");
            operand_display(&cur->op.function.func);
            printf(" :");
            break;
        case T_GO:
            printf("GOTO ");
            operand_display(&cur->op.go.arg1);
            break;
        case T_RETURN:
            printf("RETURN ");
            operand_display(&cur->op.ret.arg1);
            break;
        case T_ARG:
            printf("ARG ");
            operand_display(&cur->op.arg.arg1);
            break;
        case T_PARAM:
            printf("PARAM ");
            operand_display(&cur->op.param.arg1);
            break;
        case T_READ:
            printf("READ ");
            operand_display(&cur->op.read.arg1);
            break;
        case T_WRITE:
            printf("WRITE ");
            operand_display(&cur->op.write.arg1);
            break;
        case T_ASSIGN:
            operand_display(&cur->op.assign.arg1);
            printf(" := ");
            operand_display(&cur->op.assign.arg2);
            break;
        case T_ADDR:
            operand_display(&cur->op.addr.arg1);
            printf(" := &");
            operand_display(&cur->op.addr.arg2);
            break;
        case T_A_STAR:
            operand_display(&cur->op.a_star.arg1);
            printf(" := *");
            operand_display(&cur->op.a_star.arg2);
            break;
        case T_STAR_A:
            printf("*");
            operand_display(&cur->op.star_a.arg1);
            printf(" := ");
            operand_display(&cur->op.star_a.arg2);
            break;
        case T_DEC:
            printf("DEC ");
            operand_display(&cur->op.dec.arg1);
            printf(" ");
            operand_display(&cur->op.dec.arg2);
            break;
        case T_CALL:
            operand_display(&cur->op.call.arg1);
            printf(" := CALL ");
            operand_display(&cur->op.call.arg2);
            break;
        case T_MINUS:
            operand_display(&cur->op.minus.arg1);
            printf(" := ");
            operand_display(&cur->op.minus.arg2);
            printf(" - ");
            operand_display(&cur->op.minus.arg3);
            break;
        case T_MUL:
            operand_display(&cur->op.mul.arg1);
            printf(" := ");
            operand_display(&cur->op.mul.arg2);
            printf(" * ");
            operand_display(&cur->op.mul.arg3);
            break;
        case T_DIV:
            operand_display(&cur->op.div.arg1);
            printf(" := ");
            operand_display(&cur->op.div.arg2);
            printf(" / ");
            operand_display(&cur->op.div.arg3);
            break;
        case T_ADD:
            operand_display(&cur->op.add.arg1);
            printf(" := ");
            operand_display(&cur->op.add.arg2);
            printf(" + ");
            operand_display(&cur->op.add.arg3);
            break;
        case T_IF:
            printf("IF ");
            operand_display(&cur->op.ifgoto.arg1);
            printf(" ");
            operand_display(&cur->op.ifgoto.op);
            printf(" ");
            operand_display(&cur->op.ifgoto.arg2);
            printf(" GOTO ");
            operand_display(&cur->op.ifgoto.arg3);
            break;
        default:
            panic("Wrong");
    }
    printf("\n");
}

void codelist_display(CodeList_t * this) {
    InterCode * cur = this->head.next, * prev;
    while (cur != &this->tail && cur) {
        intercode_display(cur);
        prev = cur;
        cur = cur->next;
    }
}

void operand_display(Operand * op) {
    switch (op->kind) {
        case VARIABLE:
            if(op->var.var_no == -1) {
                op->var.var_no = 0;
            }
            printf("%s%d",temp_var_prefix,op->var.var_no);
            break;
        case CONSTANT:
            printf("#%s", op->var.value);
            break;
        case INT_CONST:
            printf("#%d", op->var.int_const);
            break;
        case FUNCTION:
            printf("%s", op->var.f_name);
            break;
        case ORIGIN:
            printf("%s", op->var.value);
            break;
        case ADDRESS:
            printf("t%d", op->var.var_no);
            break;
        case GOTO:
            printf("%s%d",label_name,op->var.goto_id);
            break;
        case RELOP:
            printf("%s",op->var.relop);
            break;
        case DEC:
            printf("%d",op->var.dec);
            break;
        default:
            panic("Wrong!");
    }
}

static CodeList_t * code_list_optimizer(CodeList_t * this) {
    int size = inter_code_line;
    inter_code_line = 0;
    CodeList_t * ret = code_optimizer(size);
    return ret;
}

//======================================================================
//======================================================================

static void translate_Program(Node_t * root);

static Type * translate_Specifier(Node_t * root);
static unit_t * translate_Creat_Node(char *,Type *,int);
static int translate_Insert_Node(unit_t *);

static void translate_ExtDefList(Node_t * root);
static void translate_ExtDef(Node_t * root);
static void translate_ExtDecList(Node_t * root);

static unit_t * translate_FunDec(Node_t *);
static void translate_VarList(Node_t * root);
static void translate_ParamDec(Node_t * root);

static void translate_DefList(Node_t * root);
static void translate_Def(Node_t * root);
static void translate_DecList(Node_t * root);
static void translate_Dec(Node_t * root);
static unit_t * translate_VarDec(Node_t * root);
static Type * translate_StructSpecifier(Node_t * root);

static void translate_CompSt(Node_t * root);
static void translate_StmtList(Node_t * root);
static void translate_Stmt(Node_t * root);
static void translate_Exp(Node_t * root,int * place);
static int translate_Args(Node_t * root);

static void translate_Cond(Node_t * root,int label_true,int label_false);

void translate() {
    variable_map = malloc(sizeof(int) * 102400);
    memset(variable_map,0, sizeof(int) * 102400);

    codelist_init(&code_list);
    Type * read_type = type_ops->type_alloc_init(FUNC_IMPL);
    Type * write_type = type_ops->type_alloc_init(FUNC_IMPL);
    read_type->u.func.var_list = NULL;
    read_type->u.func.ret_type = &Int_Type;
    write_type->u.func.var_list = NULL;
    write_type->u.func.ret_type = NULL;
    translate_Insert_Node(translate_Creat_Node("read",read_type,-1));
    translate_Insert_Node(translate_Creat_Node("write",write_type,-1));
    translate_Program(tree->root);


//    CodeList_t * opt_code = code_list_optimizer(&code_list);
//    codelist_display(opt_code);
    codelist_display(&code_list);


    free(variable_map);
}

static int translate_getsize(const Type * type) {
    if(type->kind == BASIC) {
        return 4;
    } else if(type->kind == ARRAY) {
        return type->u.array.size * translate_getsize(type->u.array.elem);
    } else if(type->kind == STRUCTURE) {
        int ans = 0;
        FieldList * cur = type->u.structure->type->u.structure;
        while (cur) {
            ans += translate_getsize(cur->type);
            cur = cur->tail;
        }
        return ans;
    } else {
        panic("Get wrong type size!");
    }
}

static int translate_getstructbias(Type * type,char * name) {
    FieldList * cur = type->u.structure->type->u.structure;
    int ans = 0;
    while (strcmp(cur->name,name) != 0) {
        ans += translate_getsize(cur->type);
        cur = cur->tail;
    }
    return ans;
}

static unit_t * translate_Creat_Node(char * name,Type * type,int line) {//不会复制type
    unit_t * node = symbol_table->node_alloc();
    symbol_table->node_init(node,name);
    node->line = line;
    node->type = type;
}

static int translate_Insert_Node(unit_t * cur) {
    symbol_table->insert(cur);
    return 1;
}

static Type * translate_Specifier(Node_t * root) {
    Type * ret = &Wrong_Type;
    if(type(root->lchild,"TYPE")) {
        if(strcmp(root->lchild->text,"int") == 0) {
            ret = &Int_Type;
        } else if(strcmp(root->lchild->text,"float") == 0) {
            ret = &Float_Type;
        }
    } else if(type(root->lchild->lchild,"STRUCT")) {
        if(type(root->lchild->rchild,"RC")) {//是结构体的定义
            ret = translate_StructSpecifier(root->lchild);
        } else {
            ret = symbol_table->find(root->lchild->lchild->right->lchild->text)->type;
        }
    }
    return ret;
}

static void translate_Program(Node_t * root) {
    panic_on("Wrong",!type(root,"Program"));
    if(type(root->lchild,"ExtDefList")) {
        translate_ExtDefList(root->lchild);
    }
}

static void translate_ExtDefList(Node_t * root) {
    panic_on("Wrong",!type(root,"ExtDefList"));
    translate_ExtDef(root->lchild);
    if(root->lchild->right) {
        translate_ExtDefList(root->rchild);
    }
}

static void translate_ExtDef(Node_t * root) {
    panic_on("Wrong",!type(root,"ExtDef"));
    Node_t * cur = root->lchild->right;
    Node_t * specifier = root->lchild;
    Type * type = translate_Specifier(specifier);
    cur->inh = type;
    if(type(cur,"ExtDecList")) {//Ext Var Dec list
        translate_ExtDecList(cur);
    } else if(type(cur,"SEMI")) {//struct Definition
        return;
    } else if(type(cur,"FunDec")) {
        unit_t * func;
        char name[32];
        func = translate_FunDec(cur);
        strcpy(name,func->name);
        func->type->kind = FUNC_IMPL;
        translate_Insert_Node(func);//将函数插入符号表,注意失败的情况
        symbol_stack->push(FUNC_FIELD);//进入函数体
        FieldList * temp = func->type->u.func.var_list;
        gencode(T_FUNCTION, genoperand(FUNCTION,func->name));
        while (temp) {
            unit_t * var = translate_Creat_Node(temp->name,type_ops->type_copy(temp->type),temp->line);
            translate_Insert_Node(var);
            var->var_id.var_no = genvar();
            gencode(T_PARAM, genoperand(VARIABLE,var->var_id.var_no));
            variable_map[var->var_id.var_no] = 1;
            if(temp->type->kind == STRUCTURE) {
                unit_t * find_struct = symbol_table->find(temp->type->u.structure->name);
                if(!find_struct) {
                    unit_t * type_struct = translate_Creat_Node(temp->type->u.structure->name,type_ops->type_copy(temp->type),temp->line);
                    translate_Insert_Node(type_struct);
                }
            }
            temp = temp->tail;
        }//将函数参数加入符号表
        translate_CompSt(root->rchild);
        symbol_stack->pop();//处理结束退栈
    }
}

static void translate_ExtDecList(Node_t * root) {
    panic_on("Wrong",!type(root,"ExtDecList"));
    root->lchild->inh = root->rchild->inh = root->inh;
    translate_Insert_Node(translate_VarDec(root->lchild));
    if(root->rchild != root->lchild) {
        translate_ExtDecList(root->rchild);
    }
}

static unit_t * translate_FunDec(Node_t * root) {
    panic_on("Wrong",!type(root,"FunDec"));
    Type * type = type_ops->type_alloc_init(REMAINED);
    type->u.func.ret_type = type_ops->type_copy(root->inh);
    if(type(root->rchild->left,"VarList")) {
        symbol_stack->push(FUNC_FIELD);
        translate_VarList(root->rchild->left);
        type->u.func.var_list = symbol_stack->pop_var();
        symbol_stack->pop();
    } else {
        type->u.func.var_list = NULL;
    }
    unit_t * ret = translate_Creat_Node(root->lchild->text,type,root->lchild->line);
    return ret;
}

static void translate_VarList(Node_t * root) {
    panic_on("Wrong",!type(root,"VarList"));
    translate_ParamDec(root->lchild);
    if(root->lchild != root->rchild) {
        translate_VarList(root->rchild);
    }
}

static void translate_ParamDec(Node_t * root) {
    panic_on("Wrong",!type(root,"ParamDec"));
    root->rchild->inh = translate_Specifier(root->lchild);
    unit_t * node = translate_VarDec(root->rchild);
    translate_Insert_Node(node);
}

static void translate_DefList(Node_t * root) {
    panic_on("Wrong",!type(root,"DefList"));
    translate_Def(root->lchild);
    if(root->rchild != root->lchild) {
        translate_DefList(root->rchild);
    }
}

static void translate_Def(Node_t * root) {
    panic_on("Wrong",!type(root,"Def"));
    root->lchild->right->inh = translate_Specifier(root->lchild);
    translate_DecList(root->lchild->right);
}

static void translate_DecList(Node_t * root) {
    panic_on("Wrong",!type(root,"DecList"));
    root->lchild->inh = root->rchild->inh = root->inh;
    translate_Dec(root->lchild);
    if(root->lchild != root->rchild) {
        translate_DecList(root->rchild);
    }
}

static void translate_Dec(Node_t * root) {
    panic_on("Wrong",!type(root,"Dec"));
    root->lchild->inh = root->inh;
    unit_t * var = translate_VarDec(root->lchild);
    if(symbol_stack->top()->field_type != STRUCT_FIELD ){
        if((var->type->kind == STRUCTURE || var->type->kind == ARRAY)) {
            var->var_id.var_addr = genvar();
            int t1 = genvar();
            gencode(T_DEC, genoperand(VARIABLE,t1), genoperand(DEC,translate_getsize(var->type)));
            gencode(T_ADDR, genoperand(VARIABLE,var->var_id.var_addr), genoperand(VARIABLE,t1));
        } else {
            var->var_id.var_no = genvar();
            variable_map[var->var_id.var_no] = 1;
        }
    }
    translate_Insert_Node(var);
    if(root->lchild != root->rchild) {
        int t1 = genvar();
        root->rchild->inh = &Is_Top_Addr;
        translate_Exp(root->rchild,&t1);
        gencode(T_ASSIGN, genoperand(VARIABLE,var->var_id.var_addr), genoperand(VARIABLE,t1));
    }
}

static unit_t * translate_VarDec(Node_t * root) {
    panic_on("Wrong",!type(root,"VarDec"));
    int cnt = 0,nums[100] = {0};
    char * s;
    Node_t * cur;
    for(cur = root->lchild;cur->lchild != NULL;cur = cur->lchild) {
        nums[cnt++] = (int)strtol(cur->right->right->text,&s,0);
    }
    Type * var_type = new(Type), * type_head = var_type;
    int i = 0;
    do {
        if(i == cnt) {
            if(root->inh->kind == BASIC) {
                var_type->kind = BASIC;
                var_type->u.basic = root->inh->u.basic;
            } else {
                var_type->kind = root->inh->kind;
                var_type->u.structure = type_ops->field_copy(root->inh->u.structure);
            }
        } else {
            var_type->kind = ARRAY;
            var_type->u.array.size = nums[cnt - i - 1];
            var_type->u.array.elem = new(Type);
            var_type = var_type->u.array.elem;
        }
        i++;
    } while (i <= cnt);
    unit_t * ret = translate_Creat_Node(cur->text,type_head,cur->line);
    return ret;
}

static Type * translate_StructSpecifier(Node_t * root) {
    static int anonymous = 1;
    panic_on("Wrong",!type(root,"StructSpecifier"));
    assert(type(root->lchild,"STRUCT") && type(root->rchild,"RC"));
    symbol_stack->push(STRUCT_FIELD);
    Type * type = type_ops->type_alloc_init(STRUCTURE);
    if(type(root->rchild->left,"DefList")) {
        translate_DefList(root->rchild->left);
    }
    type->u.structure = type_ops->field_alloc_init("struct",root->lchild->line,NULL);
    type->u.structure->type = type_ops->type_alloc_init(STRUCTURE);
    type->u.structure->type->u.structure = symbol_stack->pop_var();
    symbol_stack->pop();
    unit_t * node = translate_Creat_Node("struct",type,root->lchild->line);

    if(type(root->lchild->right,"LC")) {
        sprintf(node->name,"-%d-",anonymous);
        sprintf(node->type->u.structure->name,"-%d-",anonymous++);
    } else {
        strcpy(node->name,root->lchild->right->lchild->text);
        strcpy(node->type->u.structure->name,root->lchild->right->lchild->text);
    }
    return translate_Insert_Node(node)?type:symbol_table->find(root->lchild->right->lchild->text)->type;
}
//definition finished

static void translate_CompSt(Node_t * root) {//在调用前，需要先push stack
    panic_on("Wrong",!type(root,"CompSt"));
    if(type(root->lchild->right,"DefList")) {
        translate_DefList(root->lchild->right);
    }
    if(type(root->rchild->left,"StmtList")) {
        translate_StmtList(root->rchild->left);
    }
}

static void translate_StmtList(Node_t * root) {
    panic_on("Wrong",!type(root,"StmtList"));
    translate_Stmt(root->lchild);
    if(root->lchild != root->rchild) {
        translate_StmtList(root->rchild);
    }
}

static void translate_Stmt(Node_t * root) {
    panic_on("Wrong",!type(root,"Stmt"));
    if(type(root->lchild,"Exp")) {
        root->lchild->inh = &Is_Top_Addr;
        int t = -1;
        translate_Exp(root->lchild,&t);
    } else if(type(root->lchild,"CompSt")) {
        translate_CompSt(root->lchild);
    } else if(type(root->lchild,"RETURN")) {
        int t1 = genvar();
        root->lchild->right->inh = &Is_Top_Addr;
        translate_Exp(root->lchild->right,&t1);
        gencode(T_RETURN, genoperand(VARIABLE,t1));
    } else if(type(root->lchild,"IF")) {
        if(type(root->rchild->left,"ELSE")) {
            int l2 = genlable(),l3 = genlable();
            translate_Cond(root->lchild->right->right,-1,l2);
            translate_Stmt(root->rchild->left->left);
            gencode(T_GO, genoperand(GOTO,l3));
            gencode(T_LABEL, genoperand(GOTO,l2));
            translate_Stmt(root->rchild);
            gencode(T_LABEL, genoperand(GOTO,l3));
        } else {
            int l1 = genlable();
            translate_Cond(root->lchild->right->right,-1,l1);
            translate_Stmt(root->rchild);
            gencode(T_LABEL, genoperand(GOTO,l1));
        }
    } if(type(root->lchild,"WHILE")) {
        int l1 = genlable(),l2 = genlable();
        gencode(T_LABEL, genoperand(GOTO,l1));
        translate_Cond(root->lchild->right->right,-1,l2);
        translate_Stmt(root->rchild);
        gencode(T_GO, genoperand(GOTO,l1));
        gencode(T_LABEL, genoperand(GOTO,l2));
    }
}

static char * reverse_relop(char * op) {
    if(strcmp(op,"<") == 0) {
        return ">=";
    } else if(strcmp(op,"<=") == 0) {
        return ">";
    } else if(strcmp(op,">") == 0) {
        return "<=";
    } else if(strcmp(op,">=") == 0) {
        return "<";
    } else if(strcmp(op,"==") == 0) {
        return "!=";
    } else if(strcmp(op,"!=") == 0) {
        return "==";
    } else {
        panic("Wrong Op");
    }
}

static void translate_Cond(Node_t * root,int label_true,int label_false) {
    panic_on("Wrong!",!type(root,"Exp"));
    Node_t * mid = root->lchild->right;
    set_on_if();
    if(type(mid,"RELOP")) {
        int t1 = genvar(),t2 = genvar();
        root->lchild->inh = root->rchild->inh = &Is_Top_Addr;
        translate_Exp(root->lchild,&t1);
        translate_Exp(root->rchild,&t2);
        if(label_true != -1 && label_false != -1) {
            gencode(T_IF, genoperand(VARIABLE,t1), genoperand(VARIABLE,t2), genoperand(GOTO,label_true), genoperand(RELOP,mid->text));
            gencode(T_GO, genoperand(GOTO,label_false));
        } else if(label_true != -1) {
            gencode(T_IF, genoperand(VARIABLE,t1), genoperand(VARIABLE,t2), genoperand(GOTO,label_true), genoperand(RELOP,mid->text));
        } else if(label_false != -1){
            gencode(T_IF, genoperand(VARIABLE,t1), genoperand(VARIABLE,t2), genoperand(GOTO,label_false), genoperand(RELOP, reverse_relop(mid->text)));
        }
    } else if(type(mid,"AND")) {
        if(label_false != -1) {
            translate_Cond(root->lchild,-1,label_false);
            translate_Cond(root->rchild,label_true,label_false);
        } else {
            int l1 = genlable();
            translate_Cond(root->lchild,-1,l1);
            translate_Cond(root->rchild,label_true,label_false);
            gencode(T_LABEL, genoperand(GOTO,l1));
        }
    } else if(type(mid,"OR")) {
        int b1t,b1f,b2t,b2f;
        if(label_true != -1) {
            b1t = label_true;
        } else {
            b1t = genlable();
        }
        b1f = -1;
        b2t = label_true;
        b2f = label_false;
        if(label_true != -1) {
            translate_Cond(root->lchild,b1t,b1f);
            translate_Cond(root->rchild,b2t,b2f);
        } else {
            translate_Cond(root->lchild,b1t,b1f);
            translate_Cond(root->rchild,b2t,b2f);
            gencode(T_LABEL, genoperand(GOTO,b1t));
        }
    } else if(type(root->lchild,"NOT")) {
        translate_Cond(mid,label_false,label_true);
    } else {
        int t1 = genvar();
        root->inh = &Is_Top_Addr;
        translate_Exp(root,&t1);
        if(label_true != -1 && label_false != -1) {
            gencode(T_IF, genoperand(VARIABLE,t1), genoperand(INT_CONST,0), genoperand(GOTO,label_true),genoperand(RELOP,"!="));
            gencode(T_GO, genoperand(GOTO,label_false));
        } else if(label_true != -1) {
            gencode(T_IF, genoperand(VARIABLE,t1), genoperand(INT_CONST,0), genoperand(GOTO,label_true),genoperand(RELOP,"!="));
        } else if(label_false != -1){
            gencode(T_IF, genoperand(VARIABLE,t1), genoperand(INT_CONST,0), genoperand(GOTO,label_false),genoperand(RELOP,"=="));
        } else {
            fprintf(stderr,"Error\n");
            assert(0);
        }
    }
    root->syn = &Int_Type;
    set_on_off();
}

static int args_stack[1024] = {0};
static int args_stack_size = 0;

static void translate_push_args(int k) {
    while (k--) {
        gencode(T_ARG, genoperand(VARIABLE,args_stack[--args_stack_size]));
    }
}

static void translate_Exp(Node_t * root,int * place) {
    Node_t * mid = root->lchild->right, * left = root->lchild,* right = root->rchild;
    const Type * left_type = NULL, * right_type = NULL, * mid_type = NULL;
    const Type * ret = NULL;
    if(type(left,"ID") && mid == NULL) {
        unit_t * find = symbol_table->find(left->text);
        ret = find->type;
        if(ret->kind == BASIC) {
            if(find->var_id.var_no == 0) {
                find->var_id.var_no = genvar();
            }
            *place = find->var_id.var_no;
        } else {
            if(find->var_id.var_addr == 0) {
                find->var_id.var_addr = genvar();
            }
            gencode(T_ASSIGN,genoperand(VARIABLE,*place),genoperand(VARIABLE,find->var_id.var_addr));
        }
    } else if(type(left,"INT") && mid == NULL) {
        char * s = NULL;
        int num = (int)strtol(left->text,&s,0);
        gencode(T_ASSIGN,genoperand(VARIABLE,*place),genoperand(INT_CONST,num));
        ret = &Int_Type;
    } else if(type(left,"FLOAT") && mid == NULL) {
        gencode(T_ASSIGN,genoperand(VARIABLE,*place),genoperand(CONSTANT,left->text));
        ret = &Float_Type;
    } else if(type(mid,"ASSIGNOP")) {
        int t1 = genvar(),t2 = genvar();
        left->inh = NULL;
        right->inh = root->inh;
        int is_const = 0;
        if(type(right->lchild,"INT")) {
            is_const = 1;
        }
        if(!is_const) {
            translate_Exp(right,&t2);
        }
        translate_Exp(left,&t1);
        //left->inh = &Is_Top_Addr说明是读取值，在LB DOT中，的最顶层需要读取*VAR
        //left->inh = NULL说明是赋值，在LB DOT中需要的是地址
        if(!is_const) {
            if(type(left->lchild,"ID") && left->syn->kind == BASIC) {
                gencode(T_ASSIGN,genoperand(VARIABLE,t1),genoperand(VARIABLE,t2));
            } else if(left->syn->kind == BASIC) {
                gencode(T_STAR_A,genoperand(VARIABLE,t1), genoperand(VARIABLE,t2));
            } else {
                int t3 = genvar(),t4 = genvar(),t5 = genvar();
                for(int i = 0;i < translate_getsize(left->syn);i += 4) {
                    gencode(T_ADD, genoperand(VARIABLE,t3), genoperand(VARIABLE,t1), genoperand(INT_CONST,i));
                    gencode(T_ADD, genoperand(VARIABLE,t4), genoperand(VARIABLE,t2), genoperand(INT_CONST,i));
                    gencode(T_A_STAR,genoperand(VARIABLE,t5),genoperand(VARIABLE,t4));
                    gencode(T_STAR_A,genoperand(VARIABLE,t3), genoperand(VARIABLE,t5));
                }
            }
            if(*place != -1) {
                gencode(T_ASSIGN, genoperand(VARIABLE,*place), genoperand(VARIABLE,t2));
            }
            left_type = left->syn;
            ret = left_type;
        } else {
            char * s = NULL;
            int num = (int)strtol(right->lchild->text,&s,0);
            if(type(left->lchild,"ID") && left->syn->kind == BASIC) {
                gencode(T_ASSIGN,genoperand(VARIABLE,t1),genoperand(INT_CONST,num));
            } else if(left->syn->kind == BASIC) {
                gencode(T_STAR_A,genoperand(VARIABLE,t1), genoperand(INT_CONST,num));
            }
            if(*place != -1)
                gencode(T_ASSIGN,genoperand(VARIABLE,*place),genoperand(INT_CONST,num));
        }
    }  else if(type(left,"MINUS")) {
        int t1 = genvar();
        mid->inh = root->inh;
        translate_Exp(mid,&t1);
        gencode(T_MINUS,genoperand(VARIABLE,*place),genoperand(INT_CONST,0),genoperand(VARIABLE,t1));
        ret = mid->syn;
    } else if(type(mid,"PLUS")) {
        int t1 = genvar(),t2 = genvar();
        left->inh = right->inh = root->inh;
        translate_Exp(left,&t1);
        translate_Exp(right,&t2);
        gencode(T_ADD,genoperand(VARIABLE,*place),genoperand(VARIABLE,t1),genoperand(VARIABLE,t2));
        left_type = left->syn;
        ret = left_type;
    } else if(type(mid,"MINUS")) {
        int t1 = genvar(),t2 = genvar();
        left->inh = right->inh = root->inh;
        translate_Exp(left,&t1);
        translate_Exp(right,&t2);
        gencode(T_MINUS,genoperand(VARIABLE,*place),genoperand(VARIABLE,t1),genoperand(VARIABLE,t2));
        left_type = left->syn;
        ret = left_type;
    } else if(type(mid,"STAR")) {
        int t1 = genvar(),t2 = genvar();
        left->inh = right->inh = root->inh;
        translate_Exp(left,&t1);
        translate_Exp(right,&t2);
        if(*place != -1) {
            gencode(T_MUL,genoperand(VARIABLE,*place),genoperand(VARIABLE,t1),genoperand(VARIABLE,t2));
        }
        left_type = left->syn;
        ret = left_type;
    } else if(type(mid,"DIV")) {
        int t1 = genvar(),t2 = genvar();
        left->inh = right->inh = root->inh;
        translate_Exp(left,&t1);
        translate_Exp(right,&t2);

        gencode(T_DIV,genoperand(VARIABLE,*place),genoperand(VARIABLE,t1),genoperand(VARIABLE,t2));
        left_type = left->syn;
        ret = left_type;
    } else if(type(mid,"RELOP") || type(left,"NOT") || type(mid,"AND") || type(mid,"OR")) {
        int l1 = genlable(),l2 = genlable();
        gencode(T_ASSIGN,genoperand(VARIABLE,*place),genoperand(INT_CONST,0));
        translate_Cond(root,l1,l2);
        gencode(T_LABEL, genoperand(GOTO,l1));
        gencode(T_ASSIGN,genoperand(VARIABLE,*place),genoperand(INT_CONST,1));
        gencode(T_LABEL,genoperand(GOTO,l2));
        ret = &Int_Type;
    } else if(type(left,"LP")) {
        mid->inh = root->inh;
        translate_Exp(mid,place);
        ret = mid->syn;
    } else if(type(mid,"LP")) {
        if(type(mid->right,"Args")) {
            int arg_no = translate_Args(mid->right);
            if(strcmp(left->text,"write") == 0) {
                gencode(T_WRITE, genoperand(VARIABLE,args_stack[--args_stack_size]));
            } else {
                translate_push_args(arg_no);
                if(*place != -1) {
                    gencode(T_CALL, genoperand(VARIABLE,*place) ,genoperand(FUNCTION,left->text));
                } else {
                    gencode(T_CALL, genoperand(VARIABLE,genvar()) ,genoperand(FUNCTION,left->text));
                }
            }
        } else {
            if(strcmp(left->text,"read") == 0) {
                gencode(T_READ, genoperand(VARIABLE,*place));
            } else {
                if(*place == -1) {
                    gencode(T_CALL, genoperand(VARIABLE,genvar()) ,genoperand(FUNCTION,left->text));
                } else {
                    gencode(T_CALL, genoperand(VARIABLE,*place) ,genoperand(FUNCTION,left->text));
                }
            }
        }
        ret = symbol_table->find(left->text)->type->u.func.ret_type;
    } else if(type(mid,"LB")) {
        int t1 = genvar(),t2 = genvar();
        left->inh = NULL;
        mid->right->inh = &Is_Top_Addr;
        translate_Exp(left,&t1);
        translate_Exp(mid->right,&t2);

        left_type = left->syn;
        ret = left_type->u.array.elem;

        int t3 = genvar(),t4 = genvar();
        gencode(T_MUL, genoperand(VARIABLE,t3), genoperand(VARIABLE,t2), genoperand(INT_CONST,  translate_getsize((Type*)ret)));
        gencode(T_ADD, genoperand(VARIABLE,t4), genoperand(VARIABLE,t1), genoperand(VARIABLE,t3));

        if(root->inh == &Is_Top_Addr && ret->kind == BASIC) {
            gencode(T_A_STAR, genoperand(VARIABLE,*place), genoperand(VARIABLE,t4));
        } else {
            gencode(T_ASSIGN, genoperand(VARIABLE,*place), genoperand(VARIABLE,t4));
        }

    } else if(type(mid,"DOT")) {
        int t1 = genvar(),t2 = genvar();
        left->inh = NULL;
        translate_Exp(left, &t1);
        gencode(T_ADD, genoperand(VARIABLE,t2), genoperand(VARIABLE,t1), genoperand(INT_CONST,
                                                                                       translate_getstructbias((Type*)left->syn,right->text)));
        const FieldList * temp = left->syn->u.structure->type->u.structure;
        while (temp) {
            if(strcmp(temp->name,right->text) == 0) {
                break;
            }
            temp = temp->tail;
        }
        ret = temp->type;

        if(root->inh == &Is_Top_Addr && ret->kind == BASIC) {
            gencode(T_A_STAR, genoperand(VARIABLE,*place), genoperand(VARIABLE,t2));
        } else {
            gencode(T_ASSIGN, genoperand(VARIABLE,*place), genoperand(VARIABLE,t2));
        }
    }
    root->syn = ret;
}

static int translate_Args(Node_t * root) {
    panic_on("Wrong",!type(root,"Args"));
    int t1 = genvar();
    root->lchild->inh = &Is_Top_Addr;
    translate_Exp(root->lchild,&t1);
    args_stack[args_stack_size++] = t1;
    if(type(root->rchild,"Args")) {
        return translate_Args(root->rchild) + 1;
    } else {
        return 1;
    }
}












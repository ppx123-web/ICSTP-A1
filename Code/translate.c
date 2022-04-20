#include "data.h"
#include "debug.h"
#include <stdio.h>

typedef struct CodeList_t CodeList_t;
typedef struct CodeNode_t CodeNode_t;
typedef struct Operand Operand;
typedef struct InterCode InterCode;

struct Operand {
    enum {
        VARIABLE, CONSTANT, ADDRESS, FUNCTION, GOTO,
    } kind;
    union {
        int var_no;
        int value;
        char * f_name;
        int goto_id;
    } var;
};

struct InterCode {
    enum {
        T_LABEL,T_FUNCTION,T_ASSIGN,T_ADD, T_MINUS, T_MUL, T_DIV, T_ADDR,
        T_A_STAR, T_STAR_A, T_GOTO, T_IF, T_RETURN, T_DEC, T_ARG, T_CALL,
        T_PARAM, T_READ, T_WRITE,
    } kind;
    union {
        struct { Operand id; } label;
        struct { Operand func; } function;
        struct { Operand arg1,arg2; } assign;
        struct { Operand arg1,arg2,arg3; } add;
        struct { Operand arg1,arg2,arg3; } minus;
        struct { Operand arg1,arg2,arg3; } mul;
        struct { Operand arg1,arg2,arg3; } div;
        struct { Operand arg1,arg2; } addr;
        struct { Operand arg1,arg2; } a_star;
        struct { Operand arg1,arg2; } star_a;
        struct { Operand arg1; } go;
        struct { Operand arg1; } ret;
        struct { Operand arg1,arg2; } dec;
        struct { Operand arg1; } arg;
        struct { Operand arg1,arg2; } call;
        struct { Operand arg1; } param;
        struct { Operand arg1; } read;
        struct { Operand arg1; } write;
    } op;
    InterCode * next, * prev;
};

static Operand genoperand(int kind,...) {
    va_list ap;
    Operand ret;
    ret.kind = kind;
    va_start(ap,kind);
    switch (kind) {
        case VARIABLE:
            ret.var.var_no = va_arg(ap,int);
            break;
        case CONSTANT:
            ret.var.value = va_arg(ap,int);
            break;
        case ADDRESS:
            ret.var.var_no = va_arg(ap,int);
        case FUNCTION:
            ret.var.f_name = va_arg(ap,char*);
            break;
        case GOTO:
            ret.var.goto_id = va_arg(ap,int);
            break;
        default:
            panic("Wrong!");
    }
    va_end(ap);
    return ret;
}

static void gencode(int kind,...) {

}

static int genvar() {
    static int var_idx = 1;
    return var_idx++;
}

static int genlable() {
    static int label_idx = 1;
    return label_idx++;
}


struct CodeList_t {
    InterCode head,tail;
};

void codelist_init(CodeList_t * this) {
    this->head.next = &this->tail;
    this->tail.prev = &this->head;
}

void codelist_insert(CodeList_t * this,InterCode * pos, InterCode * cur) {

}

void codelist_remove(CodeList_t * this,InterCode * cur) {

}

void codelist_merge(CodeList_t * l1,CodeList_t * l2) {}

//======================================================================

//======================================================================


static void translate_Program(Node_t * root);

static Type * translate_Specifier(Node_t * root);
static unit_t * translate_Creat_Node(char *,Type *,int);
static int translate_Insert_Node(unit_t *);
static void translate_Func_Implement();

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
static void translate_Exp(Node_t * root);
static void translate_Args(Node_t * root);


void translation_init() {

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
    if(!type) type = &Wrong_Type;
    cur->inh = type;
    if(type(cur,"ExtDecList")) {//Ext Var Dec list
        return;
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
        while (temp) {
            unit_t * var = translate_Creat_Node(temp->name,type_ops->type_copy(temp->type),temp->line);
            translate_Insert_Node(var);
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
    } else {
        panic("Wrong");
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
    translate_Insert_Node(translate_VarDec(root->rchild));
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
    translate_Insert_Node(translate_VarDec(root->lchild));
    if(root->lchild != root->rchild) {
        translate_Exp(root->rchild);
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
        translate_Exp(root->lchild);
    } else if(type(root->lchild,"CompSt")) {
        
    } else if(type(root->lchild,"RETURN")) {
        
    } else if(type(root->lchild,"IF")) {
        
    } if(type(root->lchild,"WHILE")) {
        
    }
}

static void translate_Exp(Node_t * root) {
    panic_on("Wrong Exp", !type(root,"Exp"));
    Node_t * mid = root->lchild->right, * left = root->lchild,* right = root->rchild;
    const Type * ret = NULL;
    const Type * left_type = NULL, * right_type = NULL, * mid_type = NULL;
    if(type(left,"ID") && mid == NULL) {
        
    } else if(type(left,"INT") && mid == NULL) {

    } else if(type(left,"FLOAT") && mid == NULL) {

    } else if(type(mid,"ASSIGNOP")) {
        
    }  else if(type(mid,"RELOP")) {
        
    } else if(type(mid,"PLUS") || type(mid,"MINUS") || type(mid,"STAR") || type(mid,"DIV")) {
        
    } else if(type(left,"LP") || type(left,"MINUS")) {
        
    } else if(type(left,"NOT")) {
        
    } else if(type(mid,"LP")) {

    } else if(type(mid,"LB")) {
        
    } else if(type(mid,"DOT")) {
        
    }
    panic_on("Wrong Exp",ret == NULL);
}

static void translate_Args(Node_t * root) {
    panic_on("Wrong",!type(root,"Args"));
    
}












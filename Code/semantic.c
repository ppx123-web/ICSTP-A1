#include "data.h"
#include "debug.h"

/*
 * 遍历语法分析树，遇到ExtDef和Def时构建类型，加入symbol table中
 *              遇到Exp检查是否有语义错误
 */

#define SemanticFunc(A,...) Semantic_Check_ ## A( __VA_ARGS__)
#define Assign(A) .A =  Semantic_Check_ ## A,
#define type(A,B) (strcmp(A->content,B) == 0)


static Type * Type_Copy(Type * t);
static void Type_delete(Type * t);


static struct Type_Ops_t {
    Type * (*copy)(Type *);
    void (*delete)(Type *);
    Type * (*creat_array)(Node_t *);
    Type * (*creat_structure)(Node_t *);
}typeops = {

};







void SemanticFunc(init);
void SemanticFunc(main,Node_t * root);
static void SemanticFunc(ExtDef,Node_t * root);
static void SemanticFunc(Def,Node_t * root);
static void SemanticFunc(Exp,Node_t * root);
static void SemanticFunc(CompSt,Node_t * root);
static void SemanticFunc(Struct,Node_t * root);
static unit_t * SemanticFunc(VarDec,Node_t * root,int type,char * specifier);
static void SemanticFunc(ExtDecList,Node_t * root,int type,char * specifier);

static struct Semantic_Check_t {
    void (*init)();
    void (*main)(Node_t * );
    void (*ExtDef)(Node_t * );
    void (*Def)(Node_t * );
    void (*Exp)(Node_t * );
    void (*CompSt)(Node_t * );
    void (*Struct)(Node_t * );
    unit_t *  (*VarDec)(Node_t * ,int,char *);
    void (*ExtDecList)(Node_t *,int,char *);
}Semantic_Check = {
        Assign(init)
        Assign(main)
        Assign(ExtDef)
        Assign(Def)
        Assign(Exp)
        Assign(CompSt)
        Assign(Struct)
        Assign(VarDec)
        Assign(ExtDecList)
};

void SemanticFunc(main,Node_t * root) {
    if(type(root,"ExtDef")) {
        Semantic_Check.ExtDef(root);
    } else if(type(root,"Def")) {
        Semantic_Check.Def(root);
    } else if(type(root,"Exp")) {
        Semantic_Check.Exp(root);
    } else if(type(root,"CompSt")) {
        Semantic_Check.CompSt(root);
    } else {
        for(Node_t * cur = root->lchild;cur != NULL;cur = cur->right) {
            Semantic_Check.main(cur);
        }
    }
}

void SemanticFunc(init) {
    symbol_table->init(0x3ff);
    symbol_stack->init();
    symbol_stack->push(symbol_stack->node_alloc());
}

static void SemanticFunc(ExtDef,Node_t * root) {
    Node_t * cur = root->lchild->right;
    Node_t * specifier = root->lchild;
    if(type(cur,"ExtDecList")) {
        //Var Dec
        int type;
        if(type(specifier,"TYPE")) type = BASIC;
        else if(type(specifier,"StructSpecifier")) type = STRUCTURE;
        Semantic_Check.ExtDecList(cur,type,specifier->right->text);
    } else if(type(cur,"SEMI")) {
        //struct Defination
    } else if(type(cur,"FunDec")) {
        if(type(cur->right,"CompSt")) {
            //Func implymentation
        } else if(type(cur->right,"SEMI")) {
            //Func Declaration
        }
    } else {
        panic("Wrong");
    }
}

static void SemanticFunc(Def,Node_t * root) {

}

static void SemanticFunc(Exp,Node_t * root) {

}

static void SemanticFunc(CompSt,Node_t * root) {

}

static void SemanticFunc(Struct,Node_t * root) {

}

static unit_t * SemanticFunc(VarDec,Node_t * root,int type,char * specifier) {
    unit_t * var = symbol_table->node_alloc();
    FieldList * var_field = var->field;

    symbol_table->node_init(var,root->text);
    strcpy(var_field->name,specifier);

    int cnt = 0,nums[10] = {0};
    char * s;
    for(Node_t * cur = root->lchild;cur->lchild != NULL;cur = cur->lchild) {
        nums[cnt] = (int)strtol(cur->rchild->left->text,&s,0);
        cnt++;
    }
    //to do
}

static void SemanticFunc(ExtDecList,Node_t * root,int type,char * specifier) {
    if(type == BASIC || type == STRUCTURE) {
        unit_t * var = Semantic_Check.VarDec(root->lchild,type,specifier);
        if(root->lchild->right != NULL) {
            Semantic_Check.ExtDecList(root->rchild,type,specifier);
        }
        symbol_table->insert(var);
    } else {
        panic("Wrong");
    }
}
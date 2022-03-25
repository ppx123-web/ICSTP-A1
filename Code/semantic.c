#include "data.h"
#include "debug.h"

/*
 * 遍历语法分析树，遇到ExtDef和Def时构建类型，加入symbol table中
 *              遇到Exp检查是否有语义错误
 */

#define SemanticFunc(A,...) Semantic_Check_ ## A( __VA_ARGS__)
#define Assign(A) .A =  Semantic_Check_ ## A,


//extern
FieldList * TypeTable_DefList(Node_t * cur);

//extern



void SemanticFunc(init);
void SemanticFunc(main,Node_t * root);
static void SemanticFunc(ExtDef,Node_t * root);
static void SemanticFunc(Exp,Node_t * root);
static void SemanticFunc(CompSt,Node_t * root);
static void SemanticFunc(Struct,Node_t * root);
static unit_t * SemanticFunc(VarDec,Node_t * root,int type,char * specifier);
static void SemanticFunc(ExtDecList,Node_t * root,int type,char * specifier);

static struct Semantic_Check_t {
    void (*init)();
    void (*main)(Node_t * );
    void (*ExtDef)(Node_t * );
    void (*Exp)(Node_t * );
    void (*CompSt)(Node_t * );
    void (*Struct)(Node_t * );
    unit_t *  (*VarDec)(Node_t * ,int,char *);
    void (*ExtDecList)(Node_t *,int,char *);
    FieldList * (*DefList)(Node_t *);
}Semantic_Check = {
        Assign(init)
        Assign(main)
        Assign(ExtDef)
        Assign(Exp)
        Assign(CompSt)
        Assign(Struct)
        Assign(VarDec)
        Assign(ExtDecList)

        .DefList = TypeTable_DefList,
};

void SemanticFunc(main,Node_t * root) {
    if(type(root,"ExtDef")) {
        Semantic_Check.ExtDef(root);
    } else if(type(root,"DefList")) {
        FieldList * field = Semantic_Check.DefList(root);
        //TODO 解析field,与类型表共用同一个函数
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
    for(Node_t * cur = root->lchild;cur != NULL;cur = cur->lchild) {
        nums[cnt] = (int)strtol(cur->rchild->left->text,&s,0);
        cnt++;
    }
    var_field->type = new(Type);
    Type * var_type = var_field->type;
    int i = 0;
    do {
        if(i + 1 == cnt) {
            var_type->kind = type;
            if(type == BASIC) {
                if(strcmp(specifier,"INT") == 0) {
                    var_type->u.basic = 0;
                } else {
                    var_type->u.basic = 1;
                }
            } else {

            }
        } else {
            var_type->kind = ARRAY;
            var_type->u.array.size = nums[i];
            var_type->u.array.elem = new(Type);
            var_type = var_type->u.array.elem;

        }
        i++;
    } while (i < cnt);
    //TODO
}

//需要的接口
/*
 * 一个类型表
 * 类型表的复制、删除（删除暂时不用实现，struct的定义一定是全局定义）
 * 类型表的查询
 * 类型表的插入
 */


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
#include "data.h"
#include "debug.h"

/*
 * 遍历语法分析树，遇到ExtDef和Def时构建类型，加入symbol table中
 *              遇到Exp检查是否有语义错误
 */

#define Assign(A) .A =  Semantic_Check_ ## A,


//extern


//extern



static void Semantic_Check_init();
static void Semantic_Check_main(Node_t * root);
static FieldList * Semantic_Check_gettype(Node_t * cur);

static void Semantic_Check_Exp(Node_t * root);

static void Semantic_Check_CompSt(Node_t * root);

static void Semantic_Check_ExtDefList(Node_t * root);
static void Semantic_Check_ExtDef(Node_t * root);
static FieldList * Semantic_Check_ExtDecList(Node_t * root,const FieldList * );
static FieldList * Semantic_Check_ExtDec(Node_t *,const FieldList * );

static FieldList * Semantic_Check_DefList(Node_t * cur);
static FieldList * Semantic_Check_Def(Node_t * cur);
static FieldList * Semantic_Check_DecList(Node_t * cur,const FieldList * );
static FieldList * Semantic_Check_Dec(Node_t * cur,const FieldList * );
static FieldList * Semantic_Check_VarDec(Node_t * root,const FieldList * );
static FieldList * Semantic_Check_Struct(Node_t * root);


static Semantic_Check_t Semantic_Check = {
        Assign(init)
        Assign(main)
        Assign(gettype)

        Assign(Exp)
        Assign(CompSt)
        Assign(Struct)

        Assign(ExtDefList)
        Assign(ExtDef)
        Assign(ExtDecList)
        Assign(ExtDec)

        Assign(DefList)
        Assign(Def)
        Assign(DecList)
        Assign(Dec)
        Assign(VarDec)
};

Semantic_Check_t * semantic_check = &Semantic_Check;

static void Semantic_Check_init() {
    symbol_table->init(0x3ff);
    symbol_stack->init();
    symbol_stack->push(symbol_stack->node_alloc());

    type_table->init();
}

/*
Specifier : TYPE
     | StructSpecifier
     ;
StructSpecifier : STRUCT OptTag LC DefList RC
    | STRUCT Tag
    ;
OptTag : ID
    |
    ;
Tag : ID
*/
static FieldList *  Semantic_Check_gettype(Node_t * cur) {
    FieldList * ret;
    if(type(cur->lchild,"TYPE")) {
        ret = new(FieldList);
        ret->type = new(Type);
        ret->tail = NULL;
        ret->type->kind = BASIC;
        ret->type->u.basic = type(cur,"FLOAT");
    } else if(type(cur->lchild->lchild,"STRUCT")) {
        if(type(cur->lchild->rchild,"RC")) {
            ret = semantic_check->Struct(cur->lchild);
        } else {
            ret = type_table->find(cur->rchild->lchild->text);
        }
    } else {
        panic("Wrong type");
    }
    return ret;
}

static void Semantic_Check_main(Node_t * root) {
    if(type(root->lchild,"ExtDefList")) {
        Semantic_Check_ExtDefList(root->lchild);
    } else {
        panic("Wrong");
    }
}

static void Semantic_Check_Exp(Node_t * root) {
    panic("Not implemented");
}

static void Semantic_Check_CompSt(Node_t * root) {
    panic("Not implemented");
}


static void Semantic_Check_ExtDefList(Node_t * root) {
    Semantic_Check_ExtDef(root->lchild);
    if(root->lchild->right) {
        Semantic_Check_ExtDefList(root->rchild);
    }
}

static void Semantic_Check_ExtDef(Node_t * root) {
    Node_t * cur = root->lchild->right;
    Node_t * specifier = root->lchild;
    FieldList * field = Semantic_Check_gettype(specifier);
    if(type(cur,"ExtDecList")) {
        //Var Dec list
        FieldList * list = Semantic_Check_ExtDecList(cur,field);
        while (list) {
            unit_t * node = new(unit_t);
            symbol_table->node_init(node,list->name);
            node->field = list;
            symbol_table->insert(node);
            list = list->tail;
        }
    } else if(type(cur,"SEMI")) {
        //struct Defination
        FieldList * s = Semantic_Check_Struct(root->lchild);
        type_table->insert(s);
    } else if(type(cur,"FunDec")) {
        if(type(cur->right,"CompSt")) {
            Semantic_Check_CompSt(root->rchild);
        } else if(type(cur->right,"SEMI")) {
            //Func Declaration
            //TODO()
            panic("Not implemented");
        }
    } else {
        panic("Wrong");
    }
}

static FieldList * Semantic_Check_ExtDecList(Node_t * root,const FieldList * field) {
    FieldList * ret = semantic_check->VarDec(root->lchild,field);
    FieldList * temp = ret;
    while (temp->tail) {
        temp = temp->tail;
    }
    if(root->rchild != root->lchild) {
        temp->tail = semantic_check->ExtDecList(root->rchild,field);
    }
    return ret;
}

static FieldList * Semantic_Check_ExtDec(Node_t * root,const FieldList * field) {
    FieldList * ret = Semantic_Check_VarDec(root->lchild,field);
    if(root->lchild->right) {
        ret->tail = Semantic_Check_ExtDec(root->rchild,field);
    }
    return ret;
}


/*
 * 一个类型表
 * 类型表的复制、删除（删除暂时不用实现，struct的定义一定是全局定义）
 * 类型表的查询
 * 类型表的插入
 */
//需要的接口


/*
DefList : Def DefList
    |
    :
Def : Specifier DecList SEMI
    ;
DecList : Dec
    | Dec COMMA DecList
    ;
Dec : VarDec
    | VarDec ASSIGNOP Exp
    ;
 */

static FieldList * Semantic_Check_DefList(Node_t * cur) {
    if(cur == NULL) return NULL;
    FieldList * ret = Semantic_Check_Def(cur->lchild);
    FieldList * temp = ret;
    while (temp->tail) {
        temp = temp->tail;
    }
    temp->tail = Semantic_Check_DefList(cur->lchild->right);
    return ret;
}

static FieldList * Semantic_Check_Def(Node_t * cur) {
    FieldList * field = Semantic_Check_gettype(cur->lchild);
    FieldList * ret = Semantic_Check_DecList(cur->lchild->right,field);
    return ret;
}

static FieldList * Semantic_Check_DecList(Node_t * cur,const FieldList * field) {
    if(cur == NULL) return NULL;
    FieldList * ret = Semantic_Check_Dec(cur->lchild,field);
    if(cur->rchild != cur->lchild)
        ret->tail = Semantic_Check_DecList(cur->rchild,field);
    return ret;
}

static FieldList * Semantic_Check_Dec(Node_t * cur,const FieldList * field) {
    FieldList * ret = Semantic_Check_VarDec(cur->lchild,field);
    return ret;
}

/*
VarDec : ID
    | VarDec LB INT RB
*/
static FieldList * Semantic_Check_VarDec(Node_t * root,const FieldList * field) {
    FieldList * var_field = new(FieldList);
    var_field->type = new(Type);
    int cnt = 0,nums[10] = {0};
    char * s;
    Node_t * cur;
    for(cur = root->lchild;cur->lchild != NULL;cur = cur->lchild) {
        nums[cnt++] = (int)strtol(cur->right->right->text,&s,0);
    }
    strcpy(var_field->name,cur->text);
    Type * var_type = var_field->type;
    int i = 0;
    do {
        if(i == cnt) {
            if(field->type->kind == BASIC) {
                var_type->kind = BASIC;
                var_type->u.basic = field->type->u.basic;
            } else {
                var_type->kind = STRUCTURE;
                var_type->u.structure = type_ops->field_copy(field);
            }
        } else {
            var_type->kind = ARRAY;
            var_type->u.array.size = nums[cnt - i - 1];
            var_type->u.array.elem = new(Type);
            var_type = var_type->u.array.elem;
        }
        i++;
    } while (i <= cnt);
    return var_field;
}

/*
StructSpecifier : STRUCT OptTag LC DefList RC
*/
static FieldList * Semantic_Check_Struct(Node_t * root) {
    assert(type(root->lchild,"STRUCT") && type(root->rchild,"RC"));
    FieldList * field = new(FieldList);
    field->type = new(Type);
    field->tail = NULL;
    field->type->kind = STRUCTURE;
    field->type->u.structure = Semantic_Check_DefList(root->rchild->left);
    if(type(root->lchild->right,"LC")) {
        field->name[0] = '\0';
    } else {
        strcpy(field->name,root->lchild->right->lchild->text);
        type_table->insert(field);
    }
    return field;
}
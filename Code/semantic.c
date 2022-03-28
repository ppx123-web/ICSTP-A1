#include "data.h"
#include "debug.h"


/*
 * 代码要求：
 *      所有unit_t的实例在VarDec中创建，在stack.pop()中free，里面的type指针也同样
 *      所有的VarList在 xxxfList中创建，在VarListHandling中释放
 *      gettype中获得是符号表的指针，在VarDec赋值时需要复制
 *      所有Node_t节点中，inh是继承属性，syn是综合属性，都是指针，不是实例，通过gettype指向符号表中实例，赋值时需要复制
 *
 * Type_ops.field_alloc_init(char *name,int line,Type * type)
 * 传入field 的name，line，以及Type，type会复制一份
 *
 * /


/*
 * 遍历语法分析树，遇到ExtDef和Def时构建类型，加入symbol table中
 *              遇到Exp检查是否有语义错误
 */

typedef struct VarList_t {
    unit_t * node;
    struct VarList_t * next;
    int assign;
}VarList_t;



static void ErrorHandling(int type,int line) {
    switch (type) {
        case 1:
            break;
        case 2:
            break;
        case 3:
            Log("Error type 3 at Line %d: redefinition", line);
            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
            break;
        case 7:
            break;
        case 8:
            break;
        case 9:
            break;
        case 10:
            break;
        case 11:
            break;
        case 12:
            break;
        case 13:
            break;
        case 14:
            break;
        case 15:
            Log("Error type 15 at Line %d: struct field", line);
            break;
        case 16:
            break;
        case 17:
            break;
        default:
            panic("Wrong error type");
    }
}

static void Semantic_Check_init();
static void Semantic_Check_main(Node_t * root);

static Type * Semantic_Check_gettype(Node_t * root);
static unit_t * Semantic_Check_Creat_Node(char *,Type *,int,int);
static int Semantic_Check_Insert_Node(unit_t *);
static void * Semantic_Handle_VarList(VarList_t *,int );

static void Semantic_Check_Exp(Node_t * root);
static void Semantic_Check_CompSt(Node_t * root);

static void Semantic_Check_ExtDefList(Node_t * root);
static void Semantic_Check_ExtDef(Node_t * root);
static VarList_t * Semantic_Check_ExtDecList(Node_t * root);
static unit_t * Semantic_Check_FunDec(Node_t *);

static VarList_t * Semantic_Check_VarList(Node_t * root);
static VarList_t * Semantic_Check_ParamDec(Node_t * root);


static VarList_t * Semantic_Check_DefList(Node_t * root);
static VarList_t * Semantic_Check_Def(Node_t * root);
static VarList_t * Semantic_Check_DecList(Node_t * root);
static VarList_t * Semantic_Check_Dec(Node_t * root);
static unit_t * Semantic_Check_VarDec(Node_t * root);
static Type * Semantic_Check_StructSpecifier(Node_t * root);

//所有的field实例产生于VarDec，局部变量在Dec处插入，当是 struct的定义中，需要赋值FieldList插入

static Semantic_Check_t Semantic_Check = {
        .init = Semantic_Check_init,
        .main = Semantic_Check_main,

        .ErrorHandling = ErrorHandling,
};

Semantic_Check_t * semantic_check = &Semantic_Check;

static void Semantic_Check_init() {
    symbol_table->init(0x3ff);
    symbol_stack->init();
    symbol_stack->push(symbol_stack->node_alloc(GLOB_FIELD));
}

static unit_t * Semantic_Check_Creat_Node(char * name,Type * type,int deep,int line) {
    unit_t * node = symbol_table->node_alloc();
    symbol_table->node_init(node,name);
    if(deep != 0) {
        node->deep = deep;
    }
    node->line = line;
    node->type = type;
}


static int Semantic_Check_Insert_Node(unit_t * cur) {
    unit_t * find = symbol_table->find(cur->name);
    if(find && find->deep == symbol_stack->stack_size) {
        switch (symbol_stack->top()->field_type) {
            case GLOB_FIELD:
                ErrorHandling(3,cur->line);
                break;
            case STRUCT_FIELD:
                ErrorHandling(15,cur->line);
                break;
            case FUNC_FIELD:
                break;
            default:
                panic("Wrong Field");
        }
        nodeop->delete(cur,cur->node_type);
        return 0;
    } else {
        panic_on("Insert Fail",symbol_table->insert(cur) != 1);
        return 1;
    }
}

static void * Semantic_Handle_VarList(VarList_t * head,int kind) {
    void * ret = NULL;
    FieldList * field, * temp;
    VarList_t newlist = {
            .next = NULL,.node = NULL,.assign = 0,
    };
    VarList_t * cur = head, * prev = &newlist, * temp_node;
    switch (kind) {
        case STRUCT_FIELD://返回FieldList *
            while (cur) {
                if(!symbol_table->insert(cur->node)) {
                    ErrorHandling(15,cur->node->line);
                    temp_node = cur->next;
                    nodeop->delete(cur->node,INFONODE);
                    free(cur);
                    cur = temp_node;
                    continue;
                }
                if(cur->assign) {
                    ErrorHandling(15,cur->node->line);
                }
                prev->next = cur;
                prev= prev->next;
                cur = cur->next;
            }
            cur = newlist.next;
            field = type_ops->field_alloc_init("struct",0,NULL);
            field->type = type_ops->type_alloc_init(STRUCTURE);
            field->type->u.structure = type_ops->field_alloc_init(cur->node->name,cur->node->line,cur->node->type);
            temp = field->type->u.structure;
            cur = cur->next;
            while (cur) {
                temp->tail = type_ops->field_alloc_init(cur->node->name,cur->node->line,cur->node->type);
                temp = temp->tail;
                cur = cur->next;
            }
            ret = field;
            break;
        case GLOB_FIELD: //仅做检查和插入符号表，返回NULL
            while (cur) {
                if(!symbol_table->insert(cur->node)) {
                    ErrorHandling(3,cur->node->line);
                    temp_node = cur->next;
                    nodeop->delete(cur->node,INFONODE);
                    free(cur);
                    cur = temp_node;
                    continue;
                }
                prev->next = cur;
                prev= cur;
                cur = cur->next;
            }
            break;
        case FUNC_FIELD: //返回FieldList构成的函数的参数链表
            while (cur) {
                if(!symbol_table->insert(cur->node)) {
                    ErrorHandling(3,cur->node->line);
                    temp_node = cur->next;
                    nodeop->delete(cur->node,INFONODE);
                    free(cur);
                    cur = temp_node;
                    continue;
                }
                prev->next = cur;
                prev = cur;
                cur = cur->next;
            }
            cur = newlist.next;
            field = type_ops->field_alloc_init(cur->node->name,cur->node->line,cur->node->type);
            cur = cur->next;
            temp = field;
            while (cur) {
                temp->tail = type_ops->field_alloc_init(cur->node->name,cur->node->line,cur->node->type);
                temp = temp->tail;
                cur = cur->next;
            }
            ret = field;
            break;
        default:
            panic("Wrong Field");
    }
    cur = head;
    while (cur) {
        prev = cur->next;
        free(cur);
        cur = prev;
    }
    return ret;
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

//gettype返回的都是符号表中实例的一个指针，要再次使用，需要使用type copy
static Type * Semantic_Check_gettype(Node_t * root) {
    Type * ret;
    if(type(root->lchild,"TYPE")) {
        if(strcmp(root->lchild->text,"int") == 0) {
            ret = & Int_Type;
        } else if(strcmp(root->lchild->text,"float") == 0) {
            ret = & Float_Type;
        } else {
            panic("Wrong TYPE");
        }
    } else if(type(root->lchild->lchild,"STRUCT")) {
        if(type(root->lchild->rchild,"RC")) {
            //是结构体的定义
            ret = Semantic_Check_StructSpecifier(root->lchild);
            panic_on("Wrong struct",ret->kind != STRUCTURE);
        } else {
            unit_t * temp = symbol_table->find(root->lchild->lchild->right->lchild->text);
            if(temp != NULL && temp->type->kind == STRUCTURE && strcmp(temp->name,temp->type->u.structure->name) == 0 ) {
                ret = temp->type;
            } else {
                ErrorHandling(0,0);
                panic("Not Implemented");
            }
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
    Type * field = Semantic_Check_gettype(specifier);
    cur->inh = field;
    if(type(cur,"ExtDecList")) {//Ext Var Dec list
        VarList_t * head = Semantic_Check_ExtDecList(cur);
        Semantic_Handle_VarList(head,GLOB_FIELD);
    } else if(type(cur,"SEMI")) {//struct Definition
        return;
    } else if(type(cur,"FunDec")) {
        unit_t * func;
        if(type(cur->right,"SEMI")) {
            func = Semantic_Check_FunDec(cur);
            func->type->kind = FUNC_DECL;
            Semantic_Check_Insert_Node(func);
        } else {
            func = Semantic_Check_FunDec(cur);
            func->type->kind = FUNC_IMPL;
            Semantic_Check_Insert_Node(func);
            symbol_stack->push(symbol_stack->node_alloc(FUNC_FIELD));
            FieldList * temp = func->type->u.func.var_list;
            while (temp) {
                unit_t * var = Semantic_Check_Creat_Node(temp->name,type_ops->type_copy(temp->type),0,temp->line);
                Semantic_Check_Insert_Node(var);
                temp = temp->tail;
            }
            Semantic_Check_CompSt(root->rchild);
            symbol_stack->pop();
        }
    } else {
        panic("Wrong");
    }
}

static VarList_t * Semantic_Check_ExtDecList(Node_t * root) {
    VarList_t * ret = new(VarList_t);
    ret->assign = 0;
    ret->next = NULL;
    root->lchild->inh = root->rchild->inh = root->inh;
    ret->node = Semantic_Check_VarDec(root->lchild);
    if(root->rchild != root->lchild) {
        ret->next = Semantic_Check_ExtDecList(root->rchild);
    }
    return ret;
}

static unit_t * Semantic_Check_FunDec(Node_t * root) {
    Type * type = type_ops->type_alloc_init(REMAINED);;
    type->u.func.ret_type = type_ops->type_copy(root->inh);
    if(type(root->rchild->left,"VarList")) {
        symbol_stack->push(symbol_stack->node_alloc(FUNC_FIELD));
        VarList_t * head = Semantic_Check_VarList(root->rchild->left);
        type->u.func.var_list = Semantic_Handle_VarList(head,FUNC_FIELD);
        symbol_stack->pop();
    } else {
        type->u.func.var_list = NULL;
    }
    unit_t * ret = Semantic_Check_Creat_Node(root->lchild->text,type,0,root->lchild->line);
    return ret;
}

static VarList_t * Semantic_Check_VarList(Node_t * root) {
    VarList_t * ret = Semantic_Check_ParamDec(root->lchild);
    if(root->lchild != root->rchild) {
        ret->next = Semantic_Check_VarList(root->rchild);
    }
    return ret;
}

static VarList_t * Semantic_Check_ParamDec(Node_t * root) {
    VarList_t * ret = new(VarList_t);
    root->rchild->inh = Semantic_Check_gettype(root->lchild);
    ret->next = NULL;
    ret->assign = 0;
    ret->node = Semantic_Check_VarDec(root->rchild);
    return ret;
}

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

static VarList_t * Semantic_Check_DefList(Node_t * root) {
    VarList_t * ret = Semantic_Check_Def(root->lchild);
    VarList_t * temp = ret;
    while (temp->next) {
        temp = temp->next;
    }
    if(root->rchild != root->lchild) {
        temp->next = Semantic_Check_DefList(root->rchild);
    }
    return ret;
}

static VarList_t * Semantic_Check_Def(Node_t * root) {
    root->inh = Semantic_Check_gettype(root->lchild);
    root->lchild->right->inh = root->inh;
    return Semantic_Check_DecList(root->lchild->right);
}

static VarList_t * Semantic_Check_DecList(Node_t * root) {
    root->lchild->inh = root->rchild->inh = root->inh;
    VarList_t * ret = Semantic_Check_Dec(root->lchild);
    if(root->lchild != root->rchild) {
        ret->next = Semantic_Check_DecList(root->rchild);
    }
    return ret;
}

static VarList_t * Semantic_Check_Dec(Node_t * root) {
    root->lchild->inh = root->inh;
    VarList_t * ret = new(VarList_t);
    ret->next = NULL;
    ret->node = Semantic_Check_VarDec(root->lchild);
    ret->assign = (root->lchild != root->rchild);
    return ret;
}

/*
VarDec : ID
    | VarDec LB INT RB
*/
static unit_t * Semantic_Check_VarDec(Node_t * root) {
    int cnt = 0,nums[10] = {0};
    char * s;
    Node_t * cur;
    for(cur = root->lchild;cur->lchild != NULL;cur = cur->lchild) {
        nums[cnt++] = (int)strtol(cur->right->right->text,&s,0);
    }
    Type * var_type = new(Type);
    int i = 0;
    do {
        if(i == cnt) {
            if(root->inh->kind == BASIC) {
                var_type->kind = BASIC;
                var_type->u.basic = root->inh->u.basic;
            } else {
                var_type->kind = STRUCTURE;
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
    unit_t * ret = Semantic_Check_Creat_Node(cur->text,var_type,0,cur->line);
    return ret;
}

/*
StructSpecifier : STRUCT OptTag LC DefList RC
*/
static Type * Semantic_Check_StructSpecifier(Node_t * root) {
    root->rchild->left->inh = root->inh;
    static int anonymous = 1;
    assert(type(root->lchild,"STRUCT") && type(root->rchild,"RC"));
    symbol_stack->push(symbol_stack->node_alloc(STRUCT_FIELD));
    VarList_t * head = Semantic_Check_DefList(root->rchild->left);
    Type * type = type_ops->type_alloc_init(STRUCTURE);
    type->u.structure =  Semantic_Handle_VarList(head,STRUCT_FIELD);
    symbol_stack->pop();
    unit_t * node = Semantic_Check_Creat_Node("struct",type,1,root->lchild->line);

    if(type(root->lchild->right,"LC")) {
        sprintf(node->name,"-%d-",anonymous);
        sprintf(node->type->u.structure->name,"-%d-",anonymous++);
    } else {
        strcpy(node->name,root->lchild->right->lchild->text);
        strcpy(node->type->u.structure->name,root->lchild->right->lchild->text);
    }
    Semantic_Check_Insert_Node(node);
    return type;
}
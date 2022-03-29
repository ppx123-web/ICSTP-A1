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
            Log("Error type %d at Line %d: Undefined variable", type, line);
            break;
        case 2:
            Log("Error type %d at Line %d: Undefined function", type, line);
            break;
        case 3:
            Log("Error type %d at Line %d: Redefined variable", type, line);
            break;
        case 4:
            Log("Error type %d at Line %d: Redefined function", type, line);
            break;
        case 5:
            Log("Error type %d at Line %d: Type mismatched for assignment", type, line);
            break;
        case 6:
            Log("Error type %d at Line %d: The left-hand side of an assignment must be a variable", type, line);
            break;
        case 7:
            Log("Error type %d at Line %d: Type mismatched for operands", type, line);
            break;
        case 8:
            Log("Error type %d at Line %d: Type mismatched for return", type, line);
            break;
        case 9:
            Log("Error type %d at Line %d: Function is not applicable for arguments", type, line);
            break;
        case 10:
            Log("Error type %d at Line %d: Variable is not an array", type, line);
            break;
        case 11:
            Log("Error type %d at Line %d: Not a function", type, line);
            break;
        case 12:
            Log("Error type %d at Line %d: Not an integer", type, line);
            break;
        case 13:
            Log("Error type %d at Line %d: Illegal use of \".\"", type, line);
            break;
        case 14:
            Log("Error type %d at Line %d: Non-existent field", type, line);
            break;
        case 15:
            Log("Error type %d at Line %d: Struct redefined field", type, line);
            break;
        case 16:
            Log("Error type %d at Line %d: Duplicated name struct", type, line);
            break;
        case 17:
            Log("Error type %d at Line %d: None Defined Struct", type, line);
            break;
        case 18:
            Log("Error type %d at Line %d: Function declared not implemented", type, line);
            break;
        case 19:
            Log("Error type %d at Line %d: Function Definition Conflict", type, line);
            break;

        case 20://非要求
            Log("Error type %d at Line %d: IF WHILE EXP:AND OR  not int", type, line);
        default:
            panic("Wrong error type");
    }
}

static void Semantic_Check_init();
static void Semantic_Check_Program(Node_t * root);

static Type * Semantic_Check_Specifier(Node_t * root);
static unit_t * Semantic_Check_Creat_Node(char *,Type *,int,int);
static int Semantic_Check_Insert_Node(unit_t *);
static void * Semantic_Handle_VarList(VarList_t *,int );

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

static void Semantic_Check_CompSt(Node_t * root);
static void Semantic_Check_StmtList(Node_t * root);
static void Semantic_Check_Stmt(Node_t * root);
static void Semantic_Check_Exp(Node_t * root);
static void Semantic_Check_Args(Node_t * root);


static Semantic_Check_t Semantic_Check = {
        .init = Semantic_Check_init,
        .main = Semantic_Check_Program,

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
                if(cur->type->kind == FUNC_IMPL || cur->type->kind == FUNC_DECL) {
                    if(find->type->kind != FUNC_IMPL || cur->type->kind != FUNC_IMPL) {
                        if(nodeop->equal(find,cur)) {
                            find->type->kind = cur->type->kind;
                        } else {
                            ErrorHandling(19,cur->line);
                        }
                    } else {
                        ErrorHandling(4,cur->line);
                    }
                } else {
                    ErrorHandling(3,cur->line);
                }
                break;
            case STRUCT_FIELD:
                ErrorHandling(15,cur->line);
                break;
            case FUNC_FIELD:
                ErrorHandling(3,cur->line);
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
    if(!head) return NULL;
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
            prev->next = NULL;
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
            prev->next = NULL;
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
            prev->next = NULL;
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
        case COMPST_FIELD://仅做检查和插入符号表，返回NULL
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
            prev->next = NULL;
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
static Type * Semantic_Check_Specifier(Node_t * root) {
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
            panic_on("Wrong struct",ret != NULL && ret->kind != STRUCTURE);
        } else {
            unit_t * temp = symbol_table->find(root->lchild->lchild->right->lchild->text);
            if(temp != NULL && temp->type->kind == STRUCTURE && strcmp(temp->name,temp->type->u.structure->name) == 0 ) {
                ret = temp->type;
            } else {
                ret = NULL;
                if(temp == NULL || temp->type->kind != STRUCTURE) {
                    ErrorHandling(17,root->lchild->lchild->line);
                }  else {
                    panic("Wrong Error");
                }


            }
        }
    } else {
        panic("Wrong type");
    }
    return ret;
}

static void Semantic_Check_Program(Node_t * root) {
    panic_on("Wrong",!type(root,"Program"));
    if(root->lchild && type(root->lchild,"ExtDefList")) {
        Semantic_Check_ExtDefList(root->lchild);
    } else {
        return;
    }
}

static void Semantic_Check_ExtDefList(Node_t * root) {
    panic_on("Wrong",!type(root,"ExtDefList"));
    Semantic_Check_ExtDef(root->lchild);
    if(root->lchild->right) {
        Semantic_Check_ExtDefList(root->rchild);
    }
}

static void Semantic_Check_ExtDef(Node_t * root) {
    panic_on("Wrong",!type(root,"ExtDef"));
    Node_t * cur = root->lchild->right;
    Node_t * specifier = root->lchild;
    Type * type = Semantic_Check_Specifier(specifier);
    if(!type) type = &Wrong_Type;
    cur->inh = type;
    if(type(cur,"ExtDecList")) {//Ext Var Dec list
        VarList_t * head = Semantic_Check_ExtDecList(cur);
        Semantic_Handle_VarList(head,GLOB_FIELD);
    } else if(type(cur,"SEMI")) {//struct Definition
        return;
    } else if(type(cur,"FunDec")) {
        unit_t * func;
        char name[32];
        if(type(cur->right,"SEMI")) {
            func = Semantic_Check_FunDec(cur);
            func->type->kind = FUNC_DECL;
            Semantic_Check_Insert_Node(func);
        } else {
            func = Semantic_Check_FunDec(cur);
            strcpy(name,func->name);
            func->type->kind = FUNC_IMPL;
            Semantic_Check_Insert_Node(func);//将函数插入符号表,注意失败的情况
            func = symbol_table->find(name);
            symbol_stack->push(symbol_stack->node_alloc(FUNC_FIELD));//进入函数体
            symbol_stack->top()->head.type = func->type;//在函数中的stack节点中head的type指针指向一个，但是不会删除
            FieldList * temp = func->type->u.func.var_list;
            while (temp) {
                unit_t * var = Semantic_Check_Creat_Node(temp->name,type_ops->type_copy(temp->type),0,temp->line);
                Semantic_Check_Insert_Node(var);
                temp = temp->tail;
            }//将函数参数加入符号表
            Semantic_Check_CompSt(root->rchild);
            symbol_stack->pop();//处理结束退栈
        }
    } else {
        panic("Wrong");
    }
}

static VarList_t * Semantic_Check_ExtDecList(Node_t * root) {
    panic_on("Wrong",!type(root,"ExtDecList"));
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
    panic_on("Wrong",!type(root,"FunDec"));
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
    panic_on("Wrong",!type(root,"VarList"));
    VarList_t * ret = Semantic_Check_ParamDec(root->lchild);
    if(root->lchild != root->rchild) {
        ret->next = Semantic_Check_VarList(root->rchild);
    }
    return ret;
}

static VarList_t * Semantic_Check_ParamDec(Node_t * root) {
    panic_on("Wrong",!type(root,"ParamDec"));
    VarList_t * ret = new(VarList_t);
    root->rchild->inh = Semantic_Check_Specifier(root->lchild);
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
    panic_on("Wrong",!type(root,"DefList"));
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
    panic_on("Wrong",!type(root,"Def"));
    root->inh = Semantic_Check_Specifier(root->lchild);
    root->lchild->right->inh = root->inh;
    return Semantic_Check_DecList(root->lchild->right);
}

static VarList_t * Semantic_Check_DecList(Node_t * root) {
    panic_on("Wrong",!type(root,"DecList"));
    root->lchild->inh = root->rchild->inh = root->inh;
    VarList_t * ret = Semantic_Check_Dec(root->lchild);
    if(root->lchild != root->rchild) {
        ret->next = Semantic_Check_DecList(root->rchild);
    }
    return ret;
}

static VarList_t * Semantic_Check_Dec(Node_t * root) {
    panic_on("Wrong",!type(root,"Dec"));
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
    panic_on("Wrong",!type(root,"VarDec"));
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
    unit_t * ret = Semantic_Check_Creat_Node(cur->text,var_type,0,cur->line);
    return ret;
}

/*
StructSpecifier : STRUCT OptTag LC DefList RC
*/
static Type * Semantic_Check_StructSpecifier(Node_t * root) {
    panic_on("Wrong",!type(root,"StructSpecifier"));
    root->rchild->left->inh = root->inh;
    static int anonymous = 1;
    assert(type(root->lchild,"STRUCT") && type(root->rchild,"RC"));
    symbol_stack->push(symbol_stack->node_alloc(STRUCT_FIELD));
    VarList_t * head = NULL;
    Type * type = type_ops->type_alloc_init(STRUCTURE);
    if(type(root->rchild->left,"DefList")) {
        head = Semantic_Check_DefList(root->rchild->left);
    }
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
    if(Semantic_Check_Insert_Node(node)) {
        return type;
    } else {
        return NULL;
    }
}

//definition finished

static void Semantic_Check_CompSt(Node_t * root) {
    panic_on("Wrong",!type(root,"CompSt"));
    if(type(root->lchild->right,"DefList")) {
        VarList_t * head = Semantic_Check_DefList(root->lchild->right);
        Semantic_Handle_VarList(head,COMPST_FIELD);
    }
    if(type(root->rchild->left,"StmtList")) {
        Semantic_Check_StmtList(root->rchild->left);
    }
}

static void Semantic_Check_StmtList(Node_t * root) {
    panic_on("Wrong",!type(root,"StmtList"));
    Semantic_Check_Stmt(root->lchild);
    if(root->lchild != root->rchild) {
        Semantic_Check_StmtList(root->rchild);
    }
}

static void Semantic_Check_Stmt(Node_t * root) {
    panic_on("Wrong",!type(root,"Stmt"));
    if(type(root->lchild,"Exp")) {
        Semantic_Check_Exp(root->lchild);
    } else if(type(root->lchild,"CompSt")) {
        Semantic_Check_CompSt(root->lchild);
    } else if(type(root->lchild,"RETURN")) {
        Semantic_Check_Exp(root->lchild->right);
        const Type * ret_type = root->lchild->right->syn;
        if(!type_ops->type_equal(ret_type,symbol_stack->top()->head.type->u.func.ret_type)) {
            ErrorHandling(8,root->lchild->line);
        }
    } else if(type(root->lchild,"IF")) {
        Semantic_Check_Exp(root->lchild->right->right);
        const Type * exp_type = root->lchild->right->right->syn;
        if(!type_ops->type_equal(exp_type,&Int_Type)) {
            ErrorHandling(7,root->lchild->right->right->line);
        }
        Semantic_Check_Stmt(root->rchild->right->right->right->right);
        if(type(root->rchild->left,"ELSE")) {
            Semantic_Check_Stmt(root->rchild);
        }
    } if(type(root->lchild,"WHILE")) {
        Semantic_Check_Exp(root->lchild->right->right);
        const Type * exp_type = root->lchild->right->right->syn;
        if(!type_ops->type_equal(exp_type,&Int_Type)) {
            ErrorHandling(7,root->lchild->right->right->line);
        }
        Semantic_Check_Stmt(root->rchild->right->right->right->right);
    }
}

static void Semantic_Check_Exp(Node_t * root) {
    panic_on("Wrong Exp", !type(root,"Exp"));
    Node_t * mid = root->lchild->right, * left = root->lchild,* right = root->rchild;
    const Type * ret = NULL;
    const Type * left_type, * right_type, * mid_type;
    if(type(left,"ID") && mid == NULL) {
        unit_t * find = symbol_table->find(left->text);
        if(!find) {
            ErrorHandling(1,left->line);
            ret = &Wrong_Type;
        } else {
            ret = find->type;
        }
    } else if(type(left,"INT") && mid == NULL) {
        ret = &Int_Type;
    } else if(type(left,"FLOAT") && mid == NULL) {
        ret = &Float_Type;
    } else if(type(mid,"ASSIGNOP")) {
        Semantic_Check_Exp(left);
        Semantic_Check_Exp(right);
        left_type = left->syn;
        right_type = right->syn;
        if(!type_ops->type_equal(left_type,right_type)) {
            ErrorHandling(5,mid->line);
        }
        if(!type(left->lchild,"ID") && !type(left->rchild,"RB") && !type(left->lchild->right,"DOT")) {
            ErrorHandling(6,mid->line);
        }
        ret = left_type;
    } else if(type(mid,"AND") || type(mid,"OR")) {
        Semantic_Check_Exp(left);
        Semantic_Check_Exp(right);
        left_type = left->syn;
        right_type = right->syn;
        if(!type_ops->type_equal(left_type,&Int_Type) || !type_ops->type_equal(right_type,&Int_Type)) {
            ErrorHandling(7,mid->line);
        }
        ret = &Int_Type;
    }  else if(type(mid,"RELOP")) {
        Semantic_Check_Exp(left);
        Semantic_Check_Exp(right);
        left_type = left->syn;
        right_type = right->syn;
        if((!type_ops->type_equal(left_type,&Int_Type) && !type_ops->type_equal(left_type,&Float_Type))
            || (!type_ops->type_equal(right_type,&Int_Type) && !type_ops->type_equal(right_type,&Float_Type))) {
            ErrorHandling(7,mid->line);
        }
        if(!type_ops->type_equal(right_type,left_type)) {
            ErrorHandling(7,mid->line);
        }
        ret = &Int_Type;
    } else if(type(mid,"PLUS") || type(mid,"MINUS") || type(mid,"STAR") || type(mid,"DIV")) {
        Semantic_Check_Exp(left);
        Semantic_Check_Exp(right);
        left_type = left->syn;
        right_type = right->syn;
        if((!type_ops->type_equal(left_type,&Int_Type) && !type_ops->type_equal(left_type,&Float_Type))
           || (!type_ops->type_equal(right_type,&Int_Type) && !type_ops->type_equal(right_type,&Float_Type))) {
            ErrorHandling(7,mid->line);
        }
        if(!type_ops->type_equal(right_type,left_type)) {
            ErrorHandling(7,mid->line);
        }
        ret = left_type;
    } else if(type(left,"LP")) {
        Semantic_Check_Exp(mid);
        ret = mid->syn;
    } else if(type(left,"MINUS")) {
        Semantic_Check_Exp(mid);
        ret = mid->syn;
    } else if(type(left,"NOT")) {
        Semantic_Check_Exp(mid);
        mid_type = mid->syn;
        if(!type_ops->type_equal(mid_type,&Int_Type)) {
            ErrorHandling(7,mid->line);
        }
        mid_type = &Int_Type;
    } else if(type(mid,"LP")) {
        unit_t * find = symbol_table->find(left->text);
        if(!find) {
            ErrorHandling(2,left->line);
            ret = &Wrong_Type;
        } else if(find->type->kind != FUNC_DECL && find->type->kind != FUNC_IMPL) {
            ErrorHandling(11,left->line);
            ret = find->type;
        } else {
            if(type(mid->right,"RP") && find->type->u.func.var_list != NULL) {
                ErrorHandling(9,left->line);
            } else if(type(mid->right,"Args") && find->type->u.func.var_list == NULL) {
                ErrorHandling(9,left->line);
            } else {
                if(type(mid->right,"Args")) {
                    mid->right->inh = find->type;
                    Semantic_Check_Args(mid->right);
                }
            }
            ret = find->type->u.func.ret_type;
        }
    } else if(type(mid,"LB")) {
        Semantic_Check_Exp(left);
        Semantic_Check_Exp(mid->right);
        left_type = left->syn;
        mid_type = mid->right->syn;
        if(left_type->kind != ARRAY) {
            ErrorHandling(10,mid->line);
            ret = left_type;
        } else {
            if(!type_ops->type_equal(mid_type,&Int_Type)) {
                ErrorHandling(12,mid->line);
            }
            ret = left_type->u.array.elem;
        }
    } else if(type(mid,"DOT")) {
        Semantic_Check_Exp(left);
        left_type = left->syn;
        if(left_type->kind != STRUCTURE) {
            ErrorHandling(13,mid->line);
            ret = left_type;
        } else {
            const FieldList * temp = left_type->u.structure;
            while (temp) {
                if(strcmp(temp->name,right->text) == 0) {
                    break;
                }
                temp = temp->tail;
            }
            if(temp) {
                ret = temp->type;
            } else {
                ErrorHandling(14,mid->line);
                ret = left_type;
            }
        }
    }
    root->syn = ret;
    panic_on("Wrong Exp",ret == NULL);
}

static void Semantic_Check_Args(Node_t * root) {
    panic_on("Wrong",!type(root,"Args"));
    FieldList * list, * head;
    Node_t * cur = root;
    Semantic_Check_Exp(root->lchild);
    list = type_ops->field_alloc_init("Exp",0,root->lchild->syn);
    head = list;
    if(root->lchild != root->rchild) {
        cur = cur->rchild;
    }
    while (cur->lchild != cur->rchild) {
        Semantic_Check_Exp(root->lchild);
        list->tail = type_ops->field_alloc_init("Exp",0,root->lchild->syn);
        cur = cur->rchild;
    }
    if(!type_ops->field_equal(root->inh->u.func.var_list,head)) {
        ErrorHandling(9,root->lchild->right->line);
    }
    type_ops->field_delete(head);
}




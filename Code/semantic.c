#include "data.h"
#include "debug.h"

/*
 * 代码要求：
 *      定义的变量必须在VarDec上即刻加入符号表，获得当前变量FieldList链表（不含变量定义）使用stack API，API需要复制符号表中的Type指针，即返回的是一个实例
 *      所有Node_t节点中，inh是继承属性，syn是综合属性，都是指针，不是实例，指向符号表中实例，赋值时需要复制
 *
 * Type_ops.field_alloc_init(char *name,int line,Type * type)
 * 传入field 的name，line，以及Type，type会复制一份

 * 遍历语法分析树，遇到ExtDef和Def时构建类型，加入symbol table中
 *              遇到Exp检查是否有语义错误
 */

static void ErrorHandling(int type,int line,char * msg) {
    switch (type) {
        case 1:
            Log("Error type %d at Line %d: Undefined variable \"%s\"", type, line,msg);
            break;
        case 2:
            Log("Error type %d at Line %d: Undefined function \"%s\"", type, line,msg);
            break;
        case 3:
            Log("Error type %d at Line %d: Redefined variable \"%s\"", type, line,msg);
            break;
        case 4:
            Log("Error type %d at Line %d: Redefined function \"%s\"", type, line,msg);
            break;
        case 5:
            Log("Error type %d at Line %d: Type mismatched for assignment \"%s\"", type, line,msg);
            break;
        case 6:
            Log("Error type %d at Line %d: The left-hand side of an assignment must be a variable \"%s\"", type, line,msg);
            break;
        case 7:
            Log("Error type %d at Line %d: Type mismatched for operands \"%s\"", type, line,msg);
            break;
        case 8:
            Log("Error type %d at Line %d: Type mismatched for return \"%s\"", type, line,msg);
            break;
        case 9:
            Log("Error type %d at Line %d: Function is not applicable for arguments \"%s\"", type, line,msg);
            break;
        case 10:
            Log("Error type %d at Line %d: Variable is not an array \"%s\"", type, line,msg);
            break;
        case 11:
            Log("Error type %d at Line %d: Not a function \"%s\"", type, line,msg);
            break;
        case 12:
            Log("Error type %d at Line %d: Not an integer \"%s\"", type, line,msg);
            break;
        case 13:
            Log("Error type %d at Line %d: Illegal use of \"%s\"", type, line,msg);
            break;
        case 14:
            Log("Error type %d at Line %d: Non-existent field \"%s\"", type, line,msg);
            break;
        case 15:
            Log("Error type %d at Line %d: Struct redefined field or assign \"%s\"", type, line,msg);
            break;
        case 16:
            Log("Error type %d at Line %d: Duplicated name struct \"%s\"", type, line,msg);
            break;
        case 17:
            Log("Error type %d at Line %d: None Defined Struct \"%s\"", type, line,msg);
            break;
        case 18:
            Log("Error type %d at Line %d: Undefined function \"%s\"", type, line,msg);
            break;
        case 19:
            Log("Error type %d at Line %d: Inconsistent declaration of function \"%s\"", type, line,msg);
            break;
        default:
            panic("Wrong error type");
    }
}
static void Semantic_Check_init();
static void Semantic_Check_Program(Node_t * root);

static Type * Semantic_Check_Specifier(Node_t * root);
static unit_t * Semantic_Check_Creat_Node(char *,Type *,int);
static int Semantic_Check_Insert_Node(unit_t *);
static void Semantic_Check_Func_Implement();

static void Semantic_Check_ExtDefList(Node_t * root);
static void Semantic_Check_ExtDef(Node_t * root);
static void Semantic_Check_ExtDecList(Node_t * root);

static unit_t * Semantic_Check_FunDec(Node_t *);
static void Semantic_Check_VarList(Node_t * root);
static void Semantic_Check_ParamDec(Node_t * root);

static void Semantic_Check_DefList(Node_t * root);
static void Semantic_Check_Def(Node_t * root);
static void Semantic_Check_DecList(Node_t * root);
static void Semantic_Check_Dec(Node_t * root);
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
};
Semantic_Check_t * semantic_check = &Semantic_Check;

static void Semantic_Check_init() {
    symbol_table->init(0x3fff);
    symbol_stack->init();
    symbol_stack->push(GLOB_FIELD);
}

static unit_t * Semantic_Check_Creat_Node(char * name,Type * type,int line) {//不会复制type
    unit_t * node = symbol_table->node_alloc();
    symbol_table->node_init(node,name);
    node->line = line;
    node->type = type;
}

static int Semantic_Check_Insert_Node(unit_t * cur) {
    unit_t * find = symbol_table->find(cur->name);
    if(nodeop->IsStructDef(cur)) {
//        cur->deep = 1;
        if(find && find->deep == symbol_stack->stack_size) {
            ErrorHandling(16,cur->line,cur->name);
            nodeop->delete(cur,INFONODE);
            return 0;
        } else {
            symbol_table->insert(cur);
            return 1;
        }
    } else if(find && find->deep == symbol_stack->stack_size) {
        switch (symbol_stack->top()->field_type) {
            case GLOB_FIELD:
                if(cur->type->kind == FUNC_IMPL || cur->type->kind == FUNC_DECL) {
                    if(!(find->type->kind == FUNC_IMPL && cur->type->kind == FUNC_IMPL)) {
                        if(nodeop->equal(find,cur)) {
                            find->type->kind = cur->type->kind;
                        } else {
                            ErrorHandling(19,cur->line,cur->name);
                        }
                    } else {
                        ErrorHandling(4,cur->line,cur->name);
                    }
                } else {
                    ErrorHandling(3,cur->line,cur->name);
                }
                break;
            case STRUCT_FIELD:
                ErrorHandling(15,cur->line,cur->name);
                break;
            case FUNC_FIELD:
            case COMPST_FIELD:
                ErrorHandling(3,cur->line,cur->name);
                break;
            default:
                panic("Wrong Field");
        }
        nodeop->delete(cur,cur->node_type);
        return 0;
    } else {
        symbol_table->insert(cur);
        return 1;
    }
}

static void Semantic_Check_Func_Implement() {
    panic_on("Wrong check or stack",symbol_stack->stack_size != 1);
    panic_on("Not Glob field",symbol_stack->top()->field_type != GLOB_FIELD);
    unit_t * cur = symbol_stack->top()->head.scope_next;
    while (cur != &symbol_stack->top()->tail) {
        if(cur->type->kind == FUNC_DECL) {
            ErrorHandling(18,cur->line,cur->name);
        }
        cur = cur->scope_next;
    }
}

//返回的都是符号表中实例的一个指针，要再次使用，需要使用type copy
static Type * Semantic_Check_Specifier(Node_t * root) {
    Type * ret = &Wrong_Type;
    if(type(root->lchild,"TYPE")) {
        if(strcmp(root->lchild->text,"int") == 0) {
            ret = &Int_Type;
        } else if(strcmp(root->lchild->text,"float") == 0) {
            ret = &Float_Type;
        }
    } else if(type(root->lchild->lchild,"STRUCT")) {
        if(type(root->lchild->rchild,"RC")) {//是结构体的定义
            ret = Semantic_Check_StructSpecifier(root->lchild);
            //panic_on("Wrong struct",ret != NULL && ret->kind != STRUCTURE);
        } else {
            unit_t * temp = symbol_table->find(root->lchild->lchild->right->lchild->text);
            if(temp != NULL && nodeop->IsStructDef(temp)) {
                ret = temp->type;
            } else {
                ret = &Wrong_Type;
                ErrorHandling(17,root->lchild->lchild->line,root->lchild->lchild->right->lchild->text);
            }
        }
    }
    return ret;
}

static void Semantic_Check_Program(Node_t * root) {
    panic_on("Wrong",!type(root,"Program"));
    if(type(root->lchild,"ExtDefList")) {
        Semantic_Check_ExtDefList(root->lchild);
        Semantic_Check_Func_Implement();
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
        Semantic_Check_ExtDecList(cur);
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
            symbol_stack->push(FUNC_FIELD);//进入函数体
            symbol_stack->top()->head.type = func->type;//在函数中的stack节点中head的type指针指向一个，但是不会删除
            FieldList * temp = func->type->u.func.var_list;
            while (temp) {
                unit_t * var = Semantic_Check_Creat_Node(temp->name,type_ops->type_copy(temp->type),temp->line);
                Semantic_Check_Insert_Node(var);
                if(temp->type->kind == STRUCTURE) {
                    unit_t * find_struct = symbol_table->find(temp->type->u.structure->name);
                    if(!find_struct) {
                        unit_t * type_struct = Semantic_Check_Creat_Node(temp->type->u.structure->name,type_ops->type_copy(temp->type),temp->line);
                        Semantic_Check_Insert_Node(type_struct);
                    }
                }
                temp = temp->tail;
            }//将函数参数加入符号表
            Semantic_Check_CompSt(root->rchild);
            symbol_stack->pop();//处理结束退栈
        }
    } else {
        panic("Wrong");
    }
}

static void Semantic_Check_ExtDecList(Node_t * root) {
    panic_on("Wrong",!type(root,"ExtDecList"));
    root->lchild->inh = root->rchild->inh = root->inh;
    Semantic_Check_Insert_Node(Semantic_Check_VarDec(root->lchild));
    if(root->rchild != root->lchild) {
        Semantic_Check_ExtDecList(root->rchild);
    }
}

static unit_t * Semantic_Check_FunDec(Node_t * root) {
    panic_on("Wrong",!type(root,"FunDec"));
    Type * type = type_ops->type_alloc_init(REMAINED);
    type->u.func.ret_type = type_ops->type_copy(root->inh);
    if(type(root->rchild->left,"VarList")) {
        symbol_stack->push(FUNC_FIELD);
        Semantic_Check_VarList(root->rchild->left);
        type->u.func.var_list = symbol_stack->pop_var();
        symbol_stack->pop();
    } else {
        type->u.func.var_list = NULL;
    }
    unit_t * ret = Semantic_Check_Creat_Node(root->lchild->text,type,root->lchild->line);
    return ret;
}

static void Semantic_Check_VarList(Node_t * root) {
    panic_on("Wrong",!type(root,"VarList"));
    Semantic_Check_ParamDec(root->lchild);
    if(root->lchild != root->rchild) {
        Semantic_Check_VarList(root->rchild);
    }
}

static void Semantic_Check_ParamDec(Node_t * root) {
    panic_on("Wrong",!type(root,"ParamDec"));
    root->rchild->inh = Semantic_Check_Specifier(root->lchild);
    Semantic_Check_Insert_Node(Semantic_Check_VarDec(root->rchild));
}

static void Semantic_Check_DefList(Node_t * root) {
    panic_on("Wrong",!type(root,"DefList"));
    Semantic_Check_Def(root->lchild);
    if(root->rchild != root->lchild) {
        Semantic_Check_DefList(root->rchild);
    }
}

static void Semantic_Check_Def(Node_t * root) {
    panic_on("Wrong",!type(root,"Def"));
    root->lchild->right->inh = Semantic_Check_Specifier(root->lchild);
    Semantic_Check_DecList(root->lchild->right);
}

static void Semantic_Check_DecList(Node_t * root) {
    panic_on("Wrong",!type(root,"DecList"));
    root->lchild->inh = root->rchild->inh = root->inh;
    Semantic_Check_Dec(root->lchild);
    if(root->lchild != root->rchild) {
        Semantic_Check_DecList(root->rchild);
    }
}

static void Semantic_Check_Dec(Node_t * root) {
    panic_on("Wrong",!type(root,"Dec"));
    root->lchild->inh = root->inh;
    Semantic_Check_Insert_Node(Semantic_Check_VarDec(root->lchild));
    if(root->lchild != root->rchild) {
        if(symbol_stack->top()->field_type == STRUCT_FIELD) {
            ErrorHandling(15,root->lchild->right->line,"=");
        }
        Semantic_Check_Exp(root->rchild);
        if(!type_ops->type_equal(root->inh,root->rchild->syn)) {
            ErrorHandling(5,root->rchild->left->line,"=");
        }
    }
}

static unit_t * Semantic_Check_VarDec(Node_t * root) {
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
    unit_t * ret = Semantic_Check_Creat_Node(cur->text,type_head,cur->line);
    return ret;
}

static Type * Semantic_Check_StructSpecifier(Node_t * root) {
    static int anonymous = 1;
    panic_on("Wrong",!type(root,"StructSpecifier"));
    assert(type(root->lchild,"STRUCT") && type(root->rchild,"RC"));
    symbol_stack->push(STRUCT_FIELD);
    Type * type = type_ops->type_alloc_init(STRUCTURE);
    if(type(root->rchild->left,"DefList")) {
        Semantic_Check_DefList(root->rchild->left);
    }
    type->u.structure = type_ops->field_alloc_init("struct",root->lchild->line,NULL);
    type->u.structure->type = type_ops->type_alloc_init(STRUCTURE);
    type->u.structure->type->u.structure = symbol_stack->pop_var();
    symbol_stack->pop();
    unit_t * node = Semantic_Check_Creat_Node("struct",type,root->lchild->line);

    if(type(root->lchild->right,"LC")) {
        sprintf(node->name,"-%d-",anonymous);
        sprintf(node->type->u.structure->name,"-%d-",anonymous++);
    } else {
        strcpy(node->name,root->lchild->right->lchild->text);
        strcpy(node->type->u.structure->name,root->lchild->right->lchild->text);
    }
    return Semantic_Check_Insert_Node(node)?type:symbol_table->find(root->lchild->right->lchild->text)->type;
}
//definition finished

static void Semantic_Check_CompSt(Node_t * root) {//在调用前，需要先push stack
    panic_on("Wrong",!type(root,"CompSt"));
    if(type(root->lchild->right,"DefList")) {
        Semantic_Check_DefList(root->lchild->right);
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
        symbol_stack->push(COMPST_FIELD);
        Semantic_Check_CompSt(root->lchild);
        symbol_stack->pop();
    } else if(type(root->lchild,"RETURN")) {
        Semantic_Check_Exp(root->lchild->right);
        const Type * ret_type = root->lchild->right->syn;
        if(!type_ops->type_equal(ret_type,symbol_stack->top()->head.type->u.func.ret_type)) {
            ErrorHandling(8,root->lchild->line,root->lchild->text);
        }
    } else if(type(root->lchild,"IF")) {
        Semantic_Check_Exp(root->lchild->right->right);
        const Type * exp_type = root->lchild->right->right->syn;
        if(!type_ops->type_equal(exp_type,&Int_Type)) {
            ErrorHandling(7,root->lchild->right->right->line,root->lchild->right->right->text);
        }
        if(root->lchild->right->right->right->right) {
            Semantic_Check_Stmt(root->lchild->right->right->right->right);
        }
        if(type(root->rchild->left,"ELSE")) {
            Semantic_Check_Stmt(root->rchild);
        }
    } if(type(root->lchild,"WHILE")) {
        Semantic_Check_Exp(root->lchild->right->right);
        const Type * exp_type = root->lchild->right->right->syn;
        if(!type_ops->type_equal(exp_type,&Int_Type)) {
            ErrorHandling(7,root->lchild->right->right->line,root->lchild->text);
        }
        Semantic_Check_Stmt(root->lchild->right->right->right->right);
    }
}

static void Semantic_Check_Exp(Node_t * root) {
    panic_on("Wrong Exp", !type(root,"Exp"));
    Node_t * mid = root->lchild->right, * left = root->lchild,* right = root->rchild;
    const Type * ret = NULL;
    const Type * left_type = NULL, * right_type = NULL, * mid_type = NULL;
    if(type(left,"ID") && mid == NULL) {
        unit_t * find = symbol_table->find(left->text);
        if(!find) {
            ErrorHandling(1,left->line,left->text);
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
            ErrorHandling(5,mid->line,mid->text);
        }
        if(!type(left->lchild,"ID") && !type(left->rchild,"RB") && !(type(left->lchild->right,"DOT"))) {
            ErrorHandling(6,mid->line,mid->text);
        }
        ret = left_type;
    } else if(type(mid,"AND") || type(mid,"OR")) {
        Semantic_Check_Exp(left);
        Semantic_Check_Exp(right);
        left_type = left->syn;
        right_type = right->syn;
        if(!type_ops->type_equal(left_type,&Int_Type) || !type_ops->type_equal(right_type,&Int_Type)) {
            ErrorHandling(7,mid->line,mid->text);
        }
        ret = &Int_Type;
    }  else if(type(mid,"RELOP")) {
        Semantic_Check_Exp(left);
        Semantic_Check_Exp(right);
        left_type = left->syn;
        right_type = right->syn;
        if((!type_ops->type_equal(left_type,&Int_Type) && !type_ops->type_equal(left_type,&Float_Type))
        || (!type_ops->type_equal(right_type,&Int_Type) && !type_ops->type_equal(right_type,&Float_Type))
        || !type_ops->type_equal(right_type,left_type)) {
            ErrorHandling(7,mid->line,mid->text);
        }
        ret = &Int_Type;
    } else if(type(mid,"PLUS") || type(mid,"MINUS") || type(mid,"STAR") || type(mid,"DIV")) {
        Semantic_Check_Exp(left);
        Semantic_Check_Exp(right);
        left_type = left->syn;
        right_type = right->syn;
        if((!type_ops->type_equal(left_type,&Int_Type) && !type_ops->type_equal(left_type,&Float_Type))
           || (!type_ops->type_equal(right_type,&Int_Type) && !type_ops->type_equal(right_type,&Float_Type))) {
            ErrorHandling(7,mid->line,empty);
        } else if(!type_ops->type_equal(right_type,left_type)) {
            ErrorHandling(7,mid->line,empty);
        }
        ret = left_type;
    } else if(type(left,"LP") || type(left,"MINUS")) {
        Semantic_Check_Exp(mid);
        ret = mid->syn;
    } else if(type(left,"NOT")) {
        Semantic_Check_Exp(mid);
        mid_type = mid->syn;
        if(!type_ops->type_equal(mid_type,&Int_Type)) {
            ErrorHandling(7,mid->line,left->text);
        }
        ret = &Int_Type;
    } else if(type(mid,"LP")) {
        unit_t * find = symbol_table->find(left->text);
        if(!find) {
            ErrorHandling(2,left->line,left->text);
            ret = &Wrong_Type;
        } else if(find->type->kind != FUNC_DECL && find->type->kind != FUNC_IMPL) {
            ErrorHandling(11,left->line,left->text);
            ret = find->type;
        } else {
            if((type(mid->right,"RP") && find->type->u.func.var_list != NULL) || (type(mid->right,"Args") && find->type->u.func.var_list == NULL)) {
                ErrorHandling(9,left->line,left->text);
            }  else {
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
            ErrorHandling(10,mid->line,mid->text);
            ret = left_type;
        } else {
            if(!type_ops->type_equal(mid_type,&Int_Type)) {
                ErrorHandling(12,mid->line,mid->text);
            }
            ret = left_type->u.array.elem;
        }
    } else if(type(mid,"DOT")) {
        Semantic_Check_Exp(left);
        left_type = left->syn;
        if(left_type->kind != STRUCTURE) {
            ErrorHandling(13,mid->line,mid->text);
            ret = left_type;
        } else {
            const FieldList * temp = left_type->u.structure->type->u.structure;
            while (temp) {
                if(strcmp(temp->name,right->text) == 0) {
                    break;
                }
                temp = temp->tail;
            }
            if(temp) {
                ret = temp->type;
            } else {
                ErrorHandling(14,mid->line,right->text);
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
    cur = cur->rchild;
    while (type(cur,"Args")) {
        Semantic_Check_Exp(cur->lchild);
        list->tail = type_ops->field_alloc_init("Exp",0,cur->lchild->syn);
        cur = cur->rchild;
        list = list->tail;
    }
    if(!type_ops->field_equal(root->inh->u.func.var_list,head)) {
        ErrorHandling(9,root->lchild->line,root->lchild->text);
    }
    type_ops->field_delete(head);
}

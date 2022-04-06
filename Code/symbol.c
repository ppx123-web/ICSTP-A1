#include "data.h"
#include "debug.h"

typedef SymbolInfoList_t list_t;
typedef struct SymbolStack_ele_t stack_ele_t;

static void node_init(unit_t * cur,int argc,...);
static void node_delete(void * cur,int mode);
static bool struct_node_equal(unit_t*,unit_t*);
static bool IsStructDef(unit_t *);

static struct Info_Node_Ops InfoNodeOp = {
        .init = node_init,
        .delete = node_delete,
        .equal = struct_node_equal,
        .IsStructDef = IsStructDef,
};

struct Info_Node_Ops * nodeop = &InfoNodeOp;


static void SymbolInfoList_insert(list_t * list,unit_t * cur,unit_t * new);
static void SymbolInfoList_remove(list_t * list,unit_t * cur);
static void SymbolInfoList_init(list_t * list);
static unit_t * SymbolInfoList_find(list_t * list,char * name);
static list_t * SymbolInfoList_alloc();

static struct Hash_List_Ops {
    void (*insert)(list_t * list,unit_t * cur,unit_t * new); //在list链表节点cur之后插入新节点new         !!!注意插入节点前要先malloc一个节点
    void (*remove)(list_t * list,unit_t * cur);              //在list链表中删除cur节点,                 !!!注意：删除节点时候不free，在stack中pop的时候free;
    void (*init)(list_t * list);                             //初始化
    unit_t * (* find)(list_t * list,char * name);
    list_t * (*alloc)();
}listop = {
        .insert = SymbolInfoList_insert,
        .remove = SymbolInfoList_remove,
        .init = SymbolInfoList_init,

        .find = SymbolInfoList_find,
        .alloc = SymbolInfoList_alloc,
};

//hash table list 接口


static unit_t * SymbolTable_node_alloc();
static void SymbolTable_init(int size);
static int SymbolTable_insert(unit_t * cur);
static int SymbolTable_insert_struct(unit_t * cur);
static int SymbolTable_remove(unit_t * cur);
static unit_t * SymbolTable_find(char *);
static void SymbolTable_node_init(unit_t * cur,char * name);
static void SymbolTable_display_node(unit_t *);

//SymbolTable API
MODULE_DEF(SymbolTable_t,symbol_table) = {
        .node_alloc = SymbolTable_node_alloc,
        .node_init = SymbolTable_node_init,
        .init = SymbolTable_init,
        .insert = SymbolTable_insert,
        .insert_struct = SymbolTable_insert_struct,
        .remove = SymbolTable_remove,
        .find = SymbolTable_find,
        .display_node = SymbolTable_display_node,
};

static stack_ele_t * SymbolStack_node_alloc(int );   //分配栈中的节点，由于也是链表，需要分配两个head和tail
static void SymbolStack_init();                     //初始化栈
static void SymbolStack_push(int);     //在push前应调用stack的node_alloc来分配栈中的节点
static void SymbolStack_pop();                      //在pop时free掉所有这一层作用域申请的节点
static stack_ele_t * SymbolStack_top();
static bool SymbolStack_empty();
static FieldList * SymbolStack_pop_var();
static FieldList * SymbolStack_pop_type();

MODULE_DEF(SymbolStack_t,symbol_stack) = {
        .node_alloc = SymbolStack_node_alloc,
        .init = SymbolStack_init,
        .push = SymbolStack_push,
        .pop = SymbolStack_pop,
        .top = SymbolStack_top,
        .empty = SymbolStack_empty,
        .pop_var = SymbolStack_pop_var,
        .pop_type = SymbolStack_pop_type,
};

//Symbol Table
static void SymbolTable_display_node(unit_t * cur) {
    printf("Name: %s, deep: %d\n",cur->name,cur->deep);
    type_ops->print_type(cur->type,0);
}

static int SymbolTable_hashFun(char * name) {
    int val = 0,i;
    for(; *name; ++name) {
        val = (val << 2) + *name;
        if ((i = val & symbol_table->table_size)) {
            val = (val ^ (i >> 12)) & symbol_table->table_size;
        }
    }
    return val;
}

static void SymbolTable_init(int size) {
    symbol_table->table_size = size;
    symbol_table->hash = SymbolTable_hashFun;
    symbol_table->table = (list_t **)malloc(sizeof(list_t*) * size);
    for(int i = 0;i < size;i++) {
        symbol_table->table[i] = NULL;
    }
    symbol_table->cnt = 0;
}

static unit_t * SymbolTable_node_alloc() {
    unit_t * ans = (unit_t *) malloc(sizeof(unit_t));
    return ans;
}

static void SymbolTable_node_init(unit_t * cur,char * name) {
    nodeop->init(cur,3,name,symbol_stack->stack_size,INFONODE);
}

static int SymbolTable_insert(unit_t * cur) {
    int id = symbol_table->hash(cur->name);
    list_t * list = symbol_table->table[id];
    if(symbol_table->table[id] != NULL) {
        unit_t * find = symbol_table->find(cur->name);
        if(find && find->deep == cur->deep) {
            return 0;
        } else {
            listop.insert(list,&list->head,cur);
        }
    } else {
        list = listop.alloc();
        listop.init(list);
        symbol_table->table[id] = list;
        listop.insert(list,&list->head,cur);
    }
    symbol_table->cnt += 1;

    //还需在纵向的十字链表插入头部
    unit_t * scope = &symbol_stack->top()->head;
    unit_t * next = scope->scope_next;
    panic_on("Wrong Node Type",scope->node_type != STACKNODE);
    scope->scope_next = cur;
    cur->scope_next = next;
    next->scope_prev = cur;
    cur->scope_prev = scope;
    return 1;
}
//插入失败不会删除

static int SymbolTable_remove(unit_t * cur) {
    int id = symbol_table->hash(cur->name);
    list_t * list = symbol_table->table[id];
    assert(list != NULL);
    listop.remove(list,cur);
    symbol_table->cnt--;
    return 1;
}//说明见最后，这里不free
//Symbol Table

static unit_t * SymbolTable_find(char * name) {
    int id = symbol_table->hash(name);
    list_t * list = symbol_table->table[id];
    if(list == NULL) {
        return NULL;
    } else {
        return listop.find(list,name);
    }
}

static int SymbolTable_insert_struct(unit_t * cur) {
    panic_on("Not Struct Def", IsStructDef(cur) != 1);
    unit_t * find = symbol_table->find(cur->name);
    if(find) {
        return 0;
    } else {
        int id = symbol_table->hash(cur->name);
        list_t * list = symbol_table->table[id];
        if(symbol_table->table[id] != NULL) {
            listop.insert(list,list->tail.hash_prev,cur);
        } else {
            list = listop.alloc();
            listop.init(list);
            symbol_table->table[id] = list;
            listop.insert(list,list->tail.hash_prev,cur);
        }
        symbol_table->cnt += 1;
    }

    unit_t * scope = &symbol_stack->last.prev->head;
    unit_t * next = scope->scope_next;
    panic_on("Wrong Node Type",scope->node_type != STACKNODE);
    scope->scope_next = cur;
    cur->scope_next = next;
    next->scope_prev = cur;
    cur->scope_prev = scope;
    return 1;
}

/*
 * scope,用于纵向连接hash table中元素
 * hash，用于在stack中连接栈中的元素
*/
//Symbol Stack
static stack_ele_t * SymbolStack_node_alloc(int field) {
    stack_ele_t * node = (stack_ele_t *) malloc(sizeof(stack_ele_t));

    node->field_type = field;
    node->prev = node->next = NULL;

    unit_t * head = &node->head;
    unit_t * tail = &node->tail;

    nodeop->init(head,3,"Stack node head",symbol_stack->stack_size + 1,STACKNODE);
    nodeop->init(tail,3,"Stack node tail",symbol_stack->stack_size + 1,STACKNODE);

    head->hash_prev = head->hash_next = tail->hash_prev = tail->hash_next = NULL;
    head->scope_prev = NULL,tail->scope_next = NULL;

    head->scope_next = tail;
    tail->scope_prev = head;

    return node;
}//分配两个节点，连接两个节点

static void SymbolStack_init() {
    symbol_stack->stack_size = 0;
    stack_ele_t * first = &symbol_stack->first;
    stack_ele_t * last = &symbol_stack->last;

    first->prev = last->next = NULL;
    first->next = last;
    last->prev = first;
}//初始化栈中的两个节点

static void SymbolStack_push(int field) {
    SymbolStack_ele_t * item = symbol_stack->node_alloc(field);
    stack_ele_t * first = &symbol_stack->first, * next = first->next;
    first->next = item;
    item->next = next;
    next->prev = item;
    item->prev = first;
    symbol_stack->stack_size++;

    item->head.type = next->head.type;
}

static void SymbolStack_pop() {
    stack_ele_t * top = symbol_stack->top();
    unit_t * cur = top->head.scope_next, * temp;
    while (cur != &top->tail) {
        temp = cur->scope_next;
        assert(cur->deep == top->head.deep);
        symbol_table->remove(cur);
        nodeop->delete(cur,INFONODE);
        cur = temp;
    }
    stack_ele_t * head = &symbol_stack->first, * next = top->next;
    head->next = next;
    next->prev = head;
    nodeop->delete(top,STACKNODE);
    symbol_stack->stack_size--;
}

static stack_ele_t * SymbolStack_top() {
    if(symbol_stack->first.next != &symbol_stack->last) {
        return symbol_stack->first.next;
    } else {
        Log("The Symbol Stack is empty!");
        assert(0);
    }
}

static bool SymbolStack_empty() {
    if(symbol_stack->stack_size == 0) return true;
    else return false;
}

static FieldList * SymbolStack_pop_var() {
    stack_ele_t * top = symbol_stack->top();
    unit_t * cur = top->head.scope_next;
    FieldList * ret = NULL, * tail = ret;
    while (cur != &top->tail) {
        assert(cur->deep == top->head.deep);
        if(cur->type->kind == STRUCTURE && strcmp(cur->name,cur->type->u.structure->name) == 0) {
            cur = cur->scope_next;
            continue;
        }
        ret = type_ops->field_alloc_init(cur->name,cur->line,cur->type);
        ret->tail = tail;
        tail = ret;
        cur = cur->scope_next;
    }
    return ret;
}

static FieldList * SymbolStack_pop_type() {
    stack_ele_t * top = symbol_stack->top();
    unit_t * cur = top->head.scope_next;
    FieldList * ret = NULL, * tail = ret;
    while (cur != &top->tail) {
        assert(cur->deep == top->head.deep);
        if(cur->type->kind == STRUCTURE && strcmp(cur->name,cur->type->u.structure->name) == 0) {
            ret = type_ops->field_alloc_init(cur->name,cur->line,cur->type);
            ret->tail = tail;
            tail = ret;
        }
        cur = cur->scope_next;
    }
    return ret;
}
//Symbol Stack


//Symbol List
static void SymbolInfoList_insert(list_t * list,unit_t * cur,unit_t * new) {
    unit_t * next = cur->hash_next;
    cur->hash_next = new;
    new->hash_next = next;
    next->hash_prev = new;
    new->hash_prev = cur;
    list->list_cnt++;
}

static void SymbolInfoList_init(list_t * list) {
    nodeop->init(&list->head,1,"HASH LIST HEAD");
    nodeop->init(&list->tail,1,"HASH LIST TAIL");

    list->list_cnt = 0;
    list->head.hash_next = &list->tail;
    list->tail.hash_prev = &list->head;

    list->head.hash_prev = list->tail.hash_next = NULL;
    list->head.node_type = list->tail.node_type = HASHLIST;
}

static void SymbolInfoList_remove(list_t * list,unit_t * cur) {
    unit_t * prev = cur->hash_prev, * next = cur->hash_next;
    assert(prev != NULL && next != NULL);
    prev->hash_next = next;
    next->hash_prev = prev;
    list->list_cnt--;
//在这里不删除分配的内存，最后统一删除
}


static unit_t * SymbolInfoList_find(list_t * list,char * name) {
    unit_t * cur = list->head.hash_next;
    while (cur != &list->tail) {
        if(strcmp(name,cur->name) == 0) {
            return cur;
        }
        cur = cur->hash_next;
    }
    return NULL;
}

static list_t * SymbolInfoList_alloc() {
    list_t * list = (list_t *)malloc(sizeof(list_t));
    return list;
}
//Symbol List


//Symbol Info node ops
//argc,char * name,deep,Type
static void node_init(unit_t * cur,int argc,...) {
    //argc,char * name,deep,Type
    va_list ap;
    va_start(ap,argc);
    if(argc >= 1) {
        char * name = va_arg(ap,char *);
        assert(NAME_LENGTH > strlen(name));
        strcpy(cur->name,name);
    }
    if(argc >= 2) {
        int deep = va_arg(ap,int);
        cur->deep = deep;
    }
    if(argc >= 3) {
        cur->node_type = va_arg(ap,int );
    }
    va_end(ap);
    //初始化目前仅涉及了
    cur->hash_next = cur->hash_prev = cur->scope_next = cur->scope_prev = NULL;

}

static void node_delete(void * cur,int mode) {
    unit_t * info_node = (unit_t*) cur;
    SymbolStack_ele_t * stack_node = (SymbolStack_ele_t *) cur;
    switch (mode) {
        case INFONODE://存实际信息的节点
            type_ops->type_delete(info_node->type);
            free(info_node);
            break;
        case HASHLIST://不允许通过该API删除hash table中每个链表的头尾节点
            panic("Delete hash list node not allowed!");
        case STACKNODE:
            free(stack_node);
            break;
        case STACKLIST://不允许通过该API删除stack中的
            panic("Delete stack list node not allowed!");
        default:
            panic("Wrong mode");
    }
}

static bool struct_node_equal(unit_t* n1,unit_t* n2) {
    if(strcmp(n1->name,n2->name) != 0) return false;
    if(!type_ops->type_equal(n1->type,n2->type)) return false;
    return true;
}

static bool IsStructDef(unit_t * node) {
    if(!node) return false;
    if(node->type->kind != STRUCTURE) return false;
    if(strcmp(node->name,node->type->u.structure->name) != 0) return false;
    if(node->type->u.structure->type->kind != STRUCTURE) return false;
    return true;
}


static void print_field(const FieldList *,int);
static void print_type(const Type *,int);
static Type * Type_Ops_Type_copy(const Type *);
static FieldList * Type_Ops_Field_Copy(const FieldList *);
static void Type_Ops_Type_delete(Type *);
static void Type_Ops_Field_delete(FieldList *);
static Type * Type_Ops_Type_alloc_init(int kind);
static FieldList * Type_Ops_Field_alloc_init(char *,int,const Type*);

static bool Type_Ops_Type_Equal(const Type *,const Type *);
static bool Type_Ops_Field_Equal(const FieldList *,const FieldList *);

MODULE_DEF(Type_Ops_t,type_ops) = {
        .print_field = print_field,
        .print_type = print_type,

        .type_copy = Type_Ops_Type_copy,
        .field_copy = Type_Ops_Field_Copy,

        .type_delete = Type_Ops_Type_delete,
        .field_delete = Type_Ops_Field_delete,

        .type_alloc_init = Type_Ops_Type_alloc_init,
        .field_alloc_init = Type_Ops_Field_alloc_init,

        .type_equal = Type_Ops_Type_Equal,
        .field_equal = Type_Ops_Field_Equal,
};

static void print_field(const FieldList * field,int deep) {
    if(field == NULL) return;
    for (int i = 0;i < deep;i++) {
        printf("  ");
    }
    printf("%s line:%d\n",field->name,field->line);
    print_type(field->type,deep + 1);
    if(field->tail) {
        print_field(field->tail,deep);
    }
}

static void print_type(const Type * type,int deep) {
    if(type == NULL) return;
    for (int i = 0;i < deep;i++) {
        printf("  ");
    }
    printf("Type : %d ",type->kind);
    if(type->kind == BASIC) {
        printf("BASIC : %d\n",type->u.basic);
    } else if(type->kind == ARRAY) {
        printf("Array : size=%d ->",type->u.array.size);
        print_type(type->u.array.elem,deep + 1);
    } else if(type->kind == STRUCTURE){
        printf("\n");
        print_field(type->u.structure,deep + 1);
    } else if(type->kind == FUNC_DECL || type->kind == FUNC_IMPL ) {
        printf("return type:\n");
        print_type(type->u.func.ret_type,deep + 1);
        printf("var type:\n");
        print_field(type->u.func.var_list,deep + 1);
    } else if(type->kind) {
        printf("Wrong type\n");
    } else if(type->kind == REMAINED) {
        panic("Wrong Remained Modified Type");
    }
}

static Type * Type_Ops_Type_copy(const Type * type) {
    if(!type) return NULL;
    Type * ret = new(Type);
    ret->kind = type->kind;
    switch (type->kind) {
        case BASIC:
            ret->u.basic = type->u.basic;
            break;
        case ARRAY:
            ret->u.array.size = type->u.array.size;
            ret->u.array.elem = Type_Ops_Type_copy(type->u.array.elem);
            break;
        case STRUCTURE:
            ret->u.structure = Type_Ops_Field_Copy(type->u.structure);
            break;
        case FUNC_IMPL:
        case FUNC_DECL:
            ret->u.func.ret_type = Type_Ops_Type_copy(type->u.func.ret_type);
            ret->u.func.var_list = Type_Ops_Field_Copy(type->u.func.var_list);
            break;
        case REMAINED:
            panic("Wrong Remained Modified Type");
        case WRONG:
            break;
        default:
            panic("Wrong");
    }
    return ret;
}

static FieldList * Type_Ops_Field_Copy(const FieldList * field) {
    if(!field) return NULL;
    FieldList * ret = new(FieldList);
    strcpy(ret->name,field->name);
    ret->type = type_ops->type_copy(field->type);
    ret->line = field->line;
    if(field->tail) {
        ret->tail = type_ops->field_copy(field->tail);
    } else {
        ret->tail = NULL;
    }
    return ret;
}

static void Type_Ops_Type_delete(Type * type) {
    if(type == NULL) return;
    else {
        switch (type->kind) {
            case BASIC:
                break;
            case ARRAY:
                Type_Ops_Type_delete(type->u.array.elem);
                break;
            case STRUCTURE:
                Type_Ops_Field_delete(type->u.structure);
                break;
            case FUNC_DECL:
            case FUNC_IMPL:
                Type_Ops_Type_delete(type->u.func.ret_type);
                Type_Ops_Field_delete(type->u.func.var_list);
                break;
            case REMAINED:
                panic("Wrong Remained Modified Type");
            case WRONG:
                break;
            default:
                panic("Wrong");
        }
        free(type);
    }
}

static void Type_Ops_Field_delete(FieldList * field) {
    if(field == NULL) return;
    Type_Ops_Type_delete(field->type);
    if(field->tail != NULL) {
        Type_Ops_Field_delete(field->tail);
    }
    free(field);
}

static Type * Type_Ops_Type_alloc_init(int kind) {
    Type * ret = new(Type);
    memset(ret,0, sizeof(Type));
    ret->kind = kind;
    return ret;
}

static FieldList * Type_Ops_Field_alloc_init(char * name,int line,const Type * type) {
    FieldList * ret = new(FieldList);
    memset(ret,0, sizeof(FieldList));
    strcpy(ret->name,name);
    ret->line = line;
    ret->tail = NULL;
    ret->type = type_ops->type_copy(type);
    return ret;
}

static bool Type_Ops_Type_Equal(const Type * t1,const Type * t2) {
    if(t1 == t2) return true;
    if(!t1 || !t2 ) return false;
    if(t1->kind != t2->kind) {
        if((t1->kind != FUNC_IMPL && t1->kind != FUNC_DECL) && (t2->kind != FUNC_IMPL && t2->kind != FUNC_DECL )) {
            return false;
        }
    }
    switch (t1->kind) {
        case REMAINED:
            panic("Wrong");
        case WRONG:
            return true;
        case BASIC:
            return t1->u.basic == t2->u.basic;
        case ARRAY:
            return Type_Ops_Type_Equal(t1->u.array.elem,t2->u.array.elem);
        case STRUCTURE:
            return Type_Ops_Field_Equal(t1->u.structure,t2->u.structure);
        case FUNC_DECL:
        case FUNC_IMPL:
            if(!Type_Ops_Type_Equal(t1->u.func.ret_type,t2->u.func.ret_type)) return false;
            return Type_Ops_Field_Equal(t1->u.func.var_list,t2->u.func.var_list);
    }
}

static bool Type_Ops_Field_Equal(const FieldList * f1,const FieldList * f2) {
    if(f1 == f2) return true;
    if(!f1 || !f2) return false;
    if(!Type_Ops_Type_Equal(f1->type,f2->type)) return false;
    if(!Type_Ops_Field_Equal(f1->tail,f2->tail)) return false;
    return true;
}


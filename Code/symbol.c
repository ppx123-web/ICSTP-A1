#include "data.h"
#include "debug.h"


typedef SymbolInfoList_t list_t;
typedef struct SymbolStack_ele_t stack_ele_t;

//都应该使用static

static void node_init(unit_t * cur,int argc,...);
static void node_delete(void * cur,int mode);
static bool struct_node_equal(unit_t*,unit_t*);

static struct Info_Node_Ops InfoNodeOp = {
        .init = node_init,
        .delete = node_delete,
        .equal = struct_node_equal,
};

struct Info_Node_Ops * nodeop = &InfoNodeOp;


static void SymbolInfoList_insert(list_t * list,unit_t * cur,unit_t * new);
static void SymbolInfoList_remove(list_t * list,unit_t * cur);
static void SymbolInfoList_init(list_t * list);
static void SymbolInfoList_delete(list_t * list);
static unit_t * SymbolInfoList_find(list_t * list,char * name);
static list_t * SymbolInfoList_alloc();

static struct Hash_List_Ops {
    void (*insert)(list_t * list,unit_t * cur,unit_t * new); //在list链表节点cur之后插入新节点new         !!!注意插入节点前要先malloc一个节点
    void (*remove)(list_t * list,unit_t * cur);              //在list链表中删除cur节点,                 !!!注意：删除节点时候不free，在stack中pop的时候free;
    void (*init)(list_t * list);                             //初始化
    void (*delete)(list_t * list);
    unit_t * (* find)(list_t * list,char * name);
    list_t * (*alloc)();
}listop = {
        .insert = SymbolInfoList_insert,
        .remove = SymbolInfoList_remove,
        .init = SymbolInfoList_init,

        .find = SymbolInfoList_find,
        .alloc = SymbolInfoList_alloc,
        .delete = SymbolInfoList_delete,
};

//hash table list 接口


static unit_t * SymbolTable_node_alloc();
static void SymbolTable_init(int size);
static int SymbolTable_insert(unit_t * cur);
static int SymbolTable_remove(unit_t * cur);
static unit_t * SymbolTable_find(char *);
static void SymbolTable_node_init(unit_t * cur,char * name);
static void SymbolTable_rehash();
static void SymbolTable_display_node(unit_t *);

//SymbolTable API
MODULE_DEF(SymbolTable_t,symbol_table) = {
        .node_alloc = SymbolTable_node_alloc,
        .node_init = SymbolTable_node_init,

        .init = SymbolTable_init,
        .insert = SymbolTable_insert,
        .remove = SymbolTable_remove,
        .find = SymbolTable_find,

        .rehash = SymbolTable_rehash,
        .display_node = SymbolTable_display_node,
};

static stack_ele_t * SymbolStack_node_alloc();   //分配栈中的节点，由于也是链表，需要分配两个head和tail
static void SymbolStack_init();                     //初始化栈
static void SymbolStack_push(stack_ele_t * );     //在push前应调用stack的node_alloc来分配栈中的节点
static void SymbolStack_pop();                      //在pop时free掉所有这一层作用域申请的节点
static stack_ele_t * SymbolStack_top();
static bool SymbolStack_empty();

MODULE_DEF(SymbolStack_t,symbol_stack) = {
        .node_alloc = SymbolStack_node_alloc,

        .init = SymbolStack_init,
        .push = SymbolStack_push,
        .pop = SymbolStack_pop,
        .top = SymbolStack_top,
        .empty = SymbolStack_empty,
};


//Symbol Table
static void SymbolTable_display_node(unit_t * cur) {
    printf("Name: %s, deep: %d\n",cur->name,cur->deep);
    type_ops->print_field(cur->field,0);
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
    int id = symbol_table->hash(cur->name),insert = 0;
    list_t * list = symbol_table->table[id];
    if(symbol_table->table[id] != NULL) {
        unit_t * find = symbol_table->find(cur->name);
        if(find && find->deep == cur->deep) {
            switch (symbol_stack->top()->field_type) {
                case GLOB_FIELD:
                case FUNC_FIELD:
                case COMPST_FIELD:
                    semantic_check->ErrorHandling(3,cur->field->line);
                    break;
                case STRUCT_FIELD:
                    semantic_check->ErrorHandling(15,cur->field->line);
                    break;
                default:
                    panic("Error field type");
            }
        } else {
            listop.insert(list,&list->head,cur);
            insert = 1;
        }
    } else {
        insert = 1;
        list = listop.alloc();
        listop.init(list);
        symbol_table->table[id] = list;
        listop.insert(list,&list->head,cur);
    }
    symbol_table->cnt += insert;
    if(insert) {
        printf("---\nSymbol Table Insert: %s deep:%d\n",cur->name,cur->deep);
        type_ops->print_field(cur->field,0);
        printf("field finish\n");
        //还需在纵向的十字链表插入头部
        unit_t * scope = &symbol_stack->top()->head;
        unit_t * next = scope->scope_next;
        assert(scope->type == STACKNODE);
        scope->scope_next = cur;
        cur->scope_next = next;
        next->scope_prev = cur;
        cur->scope_prev = scope;
    }
    return insert;
}
//插入失败不会删除

static int SymbolTable_remove(unit_t * cur) {
    int id = symbol_table->hash(cur->name);
    list_t * list = symbol_table->table[id];
    assert(list != NULL);
    listop.remove(list,cur);
    symbol_table->cnt--;
    printf("---\nSymbol Table remove: %s %d\n",cur->name,cur->deep);
    type_ops->print_field(cur->field,0);
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

static void SymbolTable_rehash() {
    panic("Not implemented");
}

/*
 * 在stack中，unit_t *的指针作用
 * scope,用于纵向连接hash table中元素
 * hash，用于在stack中连接栈中的元素
*/
//Symbol Stack

static stack_ele_t * SymbolStack_node_alloc(int field_type) {
    stack_ele_t * node = (stack_ele_t *) malloc(sizeof(stack_ele_t));

    node->field_type = field_type;

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

static void SymbolStack_push(stack_ele_t * item) {
    stack_ele_t * first = &symbol_stack->first, * next = first->next;
    first->next = item;
    item->next = next;
    next->prev = item;
    item->prev = first;
    symbol_stack->stack_size++;
}

static void SymbolStack_pop() {
    stack_ele_t * top = symbol_stack->top();
    unit_t * cur = top->head.scope_next, * temp;
    while (cur != &top->tail) {
        temp = cur->scope_next;
        assert(temp->deep == top->head.deep);
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

    list->head.type = list->tail.type = HASHLIST;


}

static void SymbolInfoList_remove(list_t * list,unit_t * cur) {
    unit_t * prev = cur->hash_prev, * next = cur->hash_next;
    assert(prev != NULL && next != NULL);
    prev->hash_next = next;
    next->hash_prev = prev;
    list->list_cnt--;
//在这里不删除分配的内存，最后统一删除
}

static void SymbolInfoList_delete(list_t * list) {
    panic("Not implemented");
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
        cur->type = va_arg(ap,int );
    }
    va_end(ap);
    //初始化目前仅涉及了
    cur->hash_next = cur->hash_prev = cur->scope_next = cur->scope_prev = NULL;

}

static void node_delete(void * cur,int mode) {
    unit_t * info_node = (unit_t*) cur;
    SymbolStack_ele_t * stack_node = (SymbolStack_ele_t *) cur;
    switch (mode) {
        case INFONODE:
            type_ops->field_delete(info_node->field);
            free(info_node);
            //存实际信息的节点
            break;
        case HASHLIST:
            panic("Delete hash list node not allowed!");
            //不允许通过该API删除
            //hash table中每个链表的头尾节点
            break;
        case STACKNODE:
            free(stack_node);
            break;
        case STACKLIST:
            panic("Delete stack list node not allowed!");
            //不允许通过该API删除
            //stack中的
            break;
        default:
            panic("Wrong mode");
    }
}

static bool struct_node_equal(unit_t* n1,unit_t* n2) {
    panic("Not implemented");
}
//Symbol Info node

/*
每次向散列表中插入元素时，总是将新插入的元素放到该槽下挂的链表以及该层
所对应的链表的表头。每次查表时如果定位到某个槽，则按顺序遍历这个槽下挂的链表并返回
这个槽中符合条件的第一个变量

            stack: |  1  |  2  |  3  |

 |   |  -> List :  | 31  | 21  | 11  |
 |   |
 |   |
 |   |  -> List :  | 32  | 22  | 12  |
 |   |
 |   |
 |   |  -> List :  | 13  |
 |   |

 stack中构成的链表:
1 :  11 -> 12 -> 13
2 :  21 -> 22
3 :  31 -> 32

 以stack中开头的链表：纵向链表
 以hashtable中开头的链表：横向链表

 插入删除流程：
    插入：
        在当前作用域插入： 不需要处理stack，调用SymbolTable中的insert，insert进行hash，（分配链表，如果slot没有链表)，从头部插入hash表中的链表，也从头部插入对应作用域的纵向链表
        插入新的作用域： stack申请新的节点（是两个节点），将head节点push进去
    删除：
        通常是以作用域为单位删除：找到栈顶的作用域，沿着链表删除，先使用SymbolList中的remove删除hash table slot中的节点（这个节点不会free），
        然后free当前节点，然后删除下一个节点，知道end，最后只剩下push时申请的head和end，在stack的链表中删除并free

*/



/*
 * 一个类型表
 * 类型表的复制、删除（删除暂时不用实现，struct的定义一定是全局定义）
 * 类型表的查询
 * 类型表的插入
 */
//需要的接口

static void print_field(FieldList *,int);
static void print_type(Type *,int);
static Type * Type_Ops_Type_copy(const Type *);
static FieldList * Type_Ops_Field_Copy(const FieldList *);
static void Type_Ops_Type_delete(Type *);
static void Type_Ops_Field_delete(FieldList *);
static Type * Type_Ops_creat_int(Node_t *);
static Type * Type_Ops_creat_float(Node_t *);
static Type * Type_Ops_creat_array(Node_t *);

static Type * Type_Ops_creat_structure(Node_t *);

MODULE_DEF(Type_Ops_t,type_ops) = {
        .print_field = print_field,
        .print_type = print_type,

        .type_copy = Type_Ops_Type_copy,
        .field_copy = Type_Ops_Field_Copy,

        .type_delete = Type_Ops_Type_delete,
        .field_delete = Type_Ops_Field_delete,

        .creat_int = Type_Ops_creat_int,
        .creat_float = Type_Ops_creat_float,
        .creat_array = Type_Ops_creat_array,
        .creat_structure = Type_Ops_creat_structure,

};

static void print_field(FieldList * field,int deep) {
    for (int i = 0;i < deep;i++) {
        printf("  ");
    }
    printf("%s line:%d\n",field->name,field->line);
    print_type(field->type,deep + 1);
    if(field->tail) {
        print_field(field->tail,deep);
    }
}

static void print_type(Type * type,int deep) {
    for (int i = 0;i < deep;i++) {
        printf("  ");
    }
    printf("type : %d ",type->kind);
    if(type->kind == BASIC) {
        printf("BASIC : %d\n",type->u.basic);
    } else if(type->kind == ARRAY) {
        printf("Array : size=%d ->",type->u.array.size);
        print_type(type->u.array.elem,deep + 1);
    } else if(type->kind == STRUCTURE){
        printf("\n");
        print_field(type->u.structure,deep + 1);
    } else if(type->kind == FUNC_DECL || type->kind == FUNC_IMPL ) {
        print_field(type->u.func.ret_type,deep + 1);
        printf("\n");
        print_field(type->u.func.var_list,deep + 1);
    }
}

static Type * Type_Ops_Type_copy(const Type * type) {
    Type * ret = new(Type);
    ret->kind = type->kind;
    ret->u = type->u;
    if(type->kind == STRUCTURE) {
        ret->u.structure = type_ops->field_copy(type->u.structure);
    }
    return ret;
}

static FieldList * Type_Ops_Field_Copy(const FieldList * field) {
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
                //do nothing
                break;
            case ARRAY:
                Type_Ops_Type_delete(type->u.array.elem);
                break;
            case STRUCTURE:
                Type_Ops_Field_delete(type->u.structure);
                break;
            case FUNC_IMPL:
                panic("Not implemented");
                break;
            case FUNC_DECL:
                panic("Not implemented");
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

static Type * Type_Ops_creat_int(Node_t * cur) {
    panic("Not implemented");
}

static Type * Type_Ops_creat_float(Node_t *cur) {
    panic("Not implemented");
}

static Type * Type_Ops_creat_array(Node_t *cur) {
    panic("Not implemented");
}

static Type * Type_Ops_creat_structure(Node_t *cur) {
    panic("Not implemented");
}

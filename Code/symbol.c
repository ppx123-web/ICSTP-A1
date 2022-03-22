#include <malloc.h>
#include <stdio.h>
#include <assert.h>
#include "data.h"
#include "debug.h"

#define NONE_INFO_NAME_LENGTH 32

typedef Symbol_Node_t unit_t;
typedef SymbolInfoList_t list_t;
typedef struct SymbolStack_ele_t stack_ele_t;

//都应该使用static
static void node_init();
static void node_delete(void * cur,int mode);

static void SymbolInfoList_insert(list_t * list,unit_t * cur,unit_t * new);
static void SymbolInfoList_remove(list_t * list,unit_t * cur);
static void SymbolInfoList_init(list_t * list);
static unit_t * SymbolInfoList_find(unit_t * cur);
static list_t * SymbolInfoList_alloc();

static struct Hash_List_Ops {
    void (*insert)(list_t * list,unit_t * cur,unit_t * new); //在list链表节点cur之后插入新节点new         !!!注意插入节点前要先malloc一个节点
    void (*remove)(list_t * list,unit_t * cur);              //在list链表中删除cur节点,                 !!!注意：删除节点时候不free，在stack中pop的时候free;
    void (*init)(list_t * list);                             //初始化
    unit_t * (* find)(unit_t * cur);
    list_t * (*alloc)();
}listop = {
        .insert = SymbolInfoList_insert,
        .remove = SymbolInfoList_remove,
        .init = SymbolInfoList_init,

        .find = SymbolInfoList_find,
        .alloc = SymbolInfoList_alloc,
};

//hash table list 接口


static void SymbolTable_init(int size);
static unit_t * SymbolTable_node_alloc();
static void SymbolTable_insert(unit_t * cur);
static void SymbolTable_remove(unit_t * cur);

//SymbolTable API

static stack_ele_t * SymbolStack_node_alloc();   //分配栈中的节点，由于也是链表，需要分配两个head和tail
static void SymbolStack_init();                     //初始化栈
static void SymbolStack_push(stack_ele_t * );     //在push前应调用stack的node_alloc来分配栈中的节点
static void SymbolStack_pop();                      //在pop时free掉所有这一层作用域申请的节点
static stack_ele_t * SymbolStack_top();



MODULE_DEF(SymbolTable_t,symbol_table) = {
        .node_alloc = SymbolTable_node_alloc,

        .init = SymbolTable_init,
        .insert = SymbolTable_insert,
        .remove = SymbolTable_remove,
};


MODULE_DEF(SymbolStack_t,symbol_stack) = {
        .node_alloc = SymbolStack_node_alloc,

        .init = SymbolStack_init,
        .push = SymbolStack_push,
        .pop = SymbolStack_pop,
        .top = SymbolStack_top,
};


//Symbol Table
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
    unit_t * ans = (Symbol_Node_t *) malloc(sizeof(unit_t));
    return ans;
}

static void SymbolTable_insert(unit_t * cur) {
    int id = symbol_table->hash(cur->name);
    list_t * list = symbol_table->table[id];
    if(symbol_table->table[id] != NULL) {
        listop.insert(list,&list->head,cur);
    } else {
        symbol_table->table[id] = listop.alloc();
        list = symbol_table->table[id];
        listop.init(list);
        listop.insert(list,&list->head,cur);
    }
    symbol_table->cnt++;

    //还需在纵向的十字链表插入头部
    unit_t * scope = &symbol_stack->top()->head;
    unit_t * next = scope->scope_next;
    assert(scope->type == STACKNODE);
    scope->scope_next = cur;
    cur->scope_next = next;
    next->scope_prev = cur;
    cur->scope_prev = scope;
}

static void SymbolTable_remove(unit_t * cur) {
    int id = symbol_table->hash(cur->name);
    list_t * list = symbol_table->table[id];
    assert(list != NULL);
    listop.remove(list,cur);
    symbol_table->cnt--;
}//说明见最后，这里不free
//Symbol Table


/*
 * 在stack中，unit_t *的指针作用
 * scope,用于纵向连接hash table中元素
 * hash，用于在stack中连接栈中的元素
*/
//Symbol Stack

static stack_ele_t * SymbolStack_node_alloc() {
    stack_ele_t * node = (stack_ele_t *) malloc(sizeof(stack_ele_t));

    node->prev = node->next = NULL;

    unit_t * head = &node->head;
    unit_t * tail = &node->tail;


    head->hash_prev = head->hash_next = tail->hash_prev = tail->hash_next = NULL;
    head->scope_prev = NULL,tail->scope_next = NULL;

    head->type = tail->type = STACKNODE;

    head->name = malloc(NONE_INFO_NAME_LENGTH),tail->name = malloc(NONE_INFO_NAME_LENGTH);
    sprintf(head->name,"Stack node deep %d",symbol_stack->stack_size + 1);
    sprintf(tail->name,"Stack node deep %d",symbol_stack->stack_size + 1);

    head->deep = tail->deep = symbol_stack->stack_size + 1;

    head->scope_next = tail;
    tail->scope_prev = head;

    return node;
}//分配两个节点，连接两个节点

static void SymbolStack_init() {
    symbol_stack->stack_size = 0;
    stack_ele_t * first = &symbol_stack->first;
    stack_ele_t * last = &symbol_stack->last;
    first->prev = last->next = NULL;

    first->head.type = last->head.type = STACKLIST;
    first->head.name = "Stack List top",last->head.name = "Stack List down";

    first->next = last;
    last->prev = first;
}//初始化栈中的两个节点

static void SymbolStack_push(stack_ele_t * item) {
    stack_ele_t * first = &symbol_stack->first, * next = symbol_stack->top();
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
        node_delete(cur,INFONODE);
        cur = temp;
    }
    stack_ele_t * head = &symbol_stack->first, * next = top->next;
    head->next = next;
    next->prev = head;
    node_delete(top,STACKNODE);
}

static stack_ele_t * SymbolStack_top() {
    if(symbol_stack->first.next != &symbol_stack->last) {
        return symbol_stack->first.next;
    } else {
        Log("The Symbol Stack is empty!");
        assert(0);
        return NULL;
    }
}
//Symbol Stack


//Symbol List
static void SymbolInfoList_insert(list_t * list,unit_t * cur,unit_t * new) {
    unit_t * next = cur->hash_next, * prev = cur->hash_prev;
    prev->hash_next = next;
    next->hash_prev = prev;
}

static void SymbolInfoList_init(list_t * list) {

}

static void SymbolInfoList_remove(list_t * list,unit_t * cur) {
    unit_t * prev = cur->hash_prev, * next = cur->hash_next;
    assert(prev != NULL && next != NULL);
    prev->hash_next = next;
    next->hash_prev = prev;
}

static unit_t * SymbolInfoList_find(unit_t * cur) {

}

static list_t * SymbolInfoList_alloc() {

}
//Symbol List


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
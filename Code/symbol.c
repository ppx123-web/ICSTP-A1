#include "data.h"

//都应该使用static

static void SymbolInfoList_insert(SymbolInfoList_t * list,Symbol_Node_t * cur,Symbol_Node_t * new);
static void SymbolInfoList_remove(SymbolInfoList_t * list,Symbol_Node_t * cur);
static void SymbolInfoList_init();

//hash table list 接口，用于注册等

static void SymbolTable_init(int size,HashFun h);
static Symbol_Node_t * SymbolTable_node_alloc();
static void SymbolTable_insert(Symbol_Node_t * cur);
static void SymbolTable_remove(Symbol_Node_t * cur);

//SymbolTable API

static Symbol_Node_t * SymbolStack_node_alloc();   //分配栈中的节点，由于也是链表，需要分配两个head和tail
static void SymbolStack_init();                     //初始化栈
static void SymbolStack_push(Symbol_Node_t * );     //在push前应调用stack的node_alloc来分配栈中的节点
static void SymbolStack_pop();                      //在pop时free掉所有这一层作用域申请的节点
static Symbol_Node_t * SymbolStack_top();



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
static void SymbolTable_init(int size,HashFun h) {

}

static Symbol_Node_t * SymbolTable_node_alloc() {

}

static void SymbolTable_insert(Symbol_Node_t * cur) {

}

static void SymbolTable_remove(Symbol_Node_t * cur) {

}
//Symbol Table


//Symbol Stack
static Symbol_Node_t * SymbolStack_node_alloc(){

}

static void SymbolStack_init() {

}

static void SymbolStack_push(Symbol_Node_t * item) {

}

static void SymbolStack_pop() {

}

static Symbol_Node_t * SymbolStack_top() {

}
//Symbol Stack


//Symbol List
static void SymbolInfoList_insert(SymbolInfoList_t * list,Symbol_Node_t * cur,Symbol_Node_t * new) {

}

static void SymbolInfoList_remove(SymbolInfoList_t * list,Symbol_Node_t * cur) {

}

static void SymbolInfoList_init(){

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
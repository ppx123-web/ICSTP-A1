#ifndef STRUCT_H
#define STRUCT_H


//语法树结构

typedef struct Tree_node_t {
    char * content;
    enum {
        NONE,Sentinel,
    }type;
    int line;
    char text[32];
    struct Tree_node_t * lchild,* rchild,* left,* right;
}Node_t;

typedef Node_t* (* Multiway_Api_)(Node_t * cur,Node_t* node);

typedef struct MultiwayTree_t{
    Node_t * root;

    Multiway_Api_ lminsert;
    Multiway_Api_ rminsert;
    Multiway_Api_ remove;

    struct MultiwayTree_t* (*init)(struct MultiwayTree_t * t);
    Node_t * (* Node_alloc)(char * content,int line);
    void (* traverse)(Node_t * cur,int deep);
    void (*insert_all)(Node_t * cur,int argc,Node_t * childs[]);
}MultiwayTree_t;

extern MultiwayTree_t * tree;

//语法树结构

//符号表结构

/*
要求：
 填表：填表前查表

采用hashmap，作用解决方案：十字链表

 */

typedef struct SymbolInfoList_t SymbolInfoList_t;



//哈希表用到的结构
typedef struct Symbol_Node_t {
    char * name;
    int type;
    int deep;
    //类型待处理，0为保留属性，不允许使用，用作head，tail，last,first等属性
    SymbolInfoList_t * list; //在hash table中每个slot中的每个节点所以在list

    //维护数据结构需要的信息
    struct Symbol_Node_t * hash_prev, * hash_next;
    struct Symbol_Node_t * scope_prev, * scope_next;
}Symbol_Node_t;

typedef void (*SymbolInfoList_Api_insert)(SymbolInfoList_t * list,Symbol_Node_t * cur,Symbol_Node_t * new);
typedef void (*SymbolInfoList_Api_remove)(SymbolInfoList_t * list,Symbol_Node_t * cur);

struct SymbolInfoList_t {
    Symbol_Node_t head,tail;

    void (*init)(SymbolInfoList_Api_insert a,SymbolInfoList_Api_remove b);                                 //初始化
    SymbolInfoList_Api_insert insert;               //在list链表节点cur之后插入新节点new         !!!注意插入节点前要先malloc一个节点
    SymbolInfoList_Api_remove remove;               //在list链表中删除cur节点,                 !!!注意：删除节点时候不free，在stack中pop的时候free;
};

typedef int (*HashFun)(char * name);

typedef struct SymbolTable_t {
    SymbolInfoList_t * table;
    int table_size;
    HashFun hash;
    //Api
    Symbol_Node_t * (* node_alloc)();

    void (*init)(int,HashFun);           //初始化哈希表
    void (*insert)(Symbol_Node_t *);        //插入节点
    void (*remove)(Symbol_Node_t *);        //删除节点

}SymbolTable_t;

extern SymbolTable_t * symbol_table;



//作用域用到的栈结构

typedef struct SymbolStack_t {
    Symbol_Node_t last,first;
    int stack_size;

    Symbol_Node_t * (* node_alloc)();           //分配栈中的节点，由于也是链表，需要分配两个head和tail

    void (*init)();                             //初始化栈
    void (*push)(Symbol_Node_t * );         //在push前应调用stack的node_alloc来分配栈中的节点
    void (*pop)();                              //在pop时free掉所有这一层作用域申请的节点
    Symbol_Node_t * (*top)();
}SymbolStack_t;

extern SymbolStack_t * symbol_stack;


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

#define MODULE_DEF(type,name)                       \
        extern type  name ## _mod_t;                \
        extern type * name;                         \
        type * name = &name ## _mod_t;              \
        type name ## _mod_t






#endif

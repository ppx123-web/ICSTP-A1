#ifndef STRUCT_H
#define STRUCT_H

#include <stdbool.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define new(A) (A*)(malloc(sizeof(A)))
#define NAME_LENGTH 32

//语法树结构

typedef struct Tree_node_t {
    char  content[NAME_LENGTH];
    int line;
    char text[NAME_LENGTH];
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

采用hashmap，解决方案：十字链表

 */

typedef struct Type_ Type;
typedef struct FieldList_ FieldList;

struct Type_ {
    enum {
        BASIC, ARRAY, STRUCTURE, FUNC,
    } kind;
    union {
        int basic;              //基本类型 0 int, 1 float
        struct {
            Type * elem;
            int size;
        } array;                //数组类型信息以及数组大小构成
        FieldList * structure;
        struct  {
            FieldList * ret_type;
            FieldList * var_list;
        } func;
    } u;
};

struct FieldList_ {
    char name[NAME_LENGTH];     //域的名字，指每个成员的名字，不是类型名
    Type * type;                //域的类型
    FieldList * tail;           //下一个域
    int line;
};

//结构体类型以链表的类型存储



typedef struct SymbolInfoList_t SymbolInfoList_t;


//哈希表用到的结构
typedef struct Symbol_Node_t {
    char  name[NAME_LENGTH];
    int deep;
    enum {
        HASHLIST,STACKLIST,STACKNODE,INFONODE,
    }type;
    FieldList * field;

    //维护数据结构需要的信息
    struct Symbol_Node_t * hash_prev, * hash_next;
    struct Symbol_Node_t * scope_prev, * scope_next;
}Symbol_Node_t;

typedef Symbol_Node_t unit_t;
extern struct Info_Node_Ops * nodeop;

struct Info_Node_Ops {
    void (*init)(unit_t * cur,int,...); //argc,char * name,deep,Type
    void (*delete)(void *,int);
    bool (*equal)(unit_t*,unit_t*);
};


struct SymbolInfoList_t {
    Symbol_Node_t head,tail;
    int list_cnt;
    //在list链表节点cur之后插入新节点new         !!!注意插入节点前要先malloc一个节点
    //在list链表中删除cur节点,                 !!!注意：删除节点时候不free，在stack中pop的时候free;
};

/*
 * 插入接口说明，每次插入插入的实际是指针，即要求插入的node都必须使用malloc分配内存
 * 不可以使用临时变量
 */

typedef int (*HashFun)(char * name);

typedef struct SymbolTable_t {
    SymbolInfoList_t ** table;
    int table_size;     //表的总大小，不是元素个数
    int cnt;            //元素个数
    HashFun hash;
    //Api
    Symbol_Node_t * (* node_alloc)();
    void (* node_init)(unit_t *,char *);

    void (*init)(int);                      //初始化哈希表
    int (*insert)(Symbol_Node_t *);        //插入节点
    int (*remove)(Symbol_Node_t *);        //删除节点
    Symbol_Node_t * (*find)(char *);                   //查询元素，返回true，找到，false没有找到
    void (*rehash)();
    void (*display_node)(unit_t *);
}SymbolTable_t;
extern SymbolTable_t * symbol_table;



//作用域用到的栈结构
/*
 * 在stack中，node的指针作用
 * scope,用于纵向连接hash table中元素
 * hash，用于在stack中连接栈中的元素
*/
typedef struct SymbolStack_ele_t {
    Symbol_Node_t head;
    Symbol_Node_t tail;
    enum {
        GLOB_FIELD,FUNC_FIELD,STRUCT_FIELD,COMPST_FIELD
    }field_type;
    struct SymbolStack_ele_t * prev, * next;
}SymbolStack_ele_t;


typedef struct SymbolStack_t {
    SymbolStack_ele_t last,first;                   //first栈顶，last是栈底
    int stack_size;

    SymbolStack_ele_t * (* node_alloc)(int);           //分配栈中的节点，由于也是链表，需要分配两个head和tail

    void (*init)();                             //初始化栈
    void (*push)(SymbolStack_ele_t * );         //在push前应调用stack的node_alloc来分配栈中的节点
    void (*pop)();                              //在pop时free掉所有这一层作用域申请的节点
    bool (*empty)();
    SymbolStack_ele_t * (*top)();
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

#define type(A,B) (strcmp((A)->content,B) == 0)



#define MODULE_DEF(type,name)                       \
        extern type  name ## _mod_t;                \
        extern type * name;                         \
        type * name = &name ## _mod_t;              \
        type name ## _mod_t


//需要的接口
/*
 * 一个类型表
 * 类型表的复制、删除（删除暂时不用实现，struct的定义一定是全局定义）
 * 类型表的查询
 * 类型表的插入
 */

typedef struct Type_Ops_t {
    Type * (*type_copy)(const Type *);
    FieldList * (*field_copy)(const FieldList *);
    void (*type_delete)(Type *);
    void (*field_delete)(FieldList *);

    Type * (*creat_int)(Node_t *);
    Type * (*creat_float)(Node_t *);
    Type * (*creat_array)(Node_t *);
    Type * (*creat_structure)(Node_t *);

    void (*print_field)(FieldList *,int);
    void (*print_type)(Type *,int);
}Type_Ops_t;
extern Type_Ops_t * type_ops;


typedef struct Semantic_Check_t {
    void (*init)();
    void (*main)(Node_t * );
    FieldList * (*gettype)(Node_t *);

//    void (*CompSt)(Node_t * );
//
//    void (*Exp)(Node_t * );
//
//    void (*ExtDefList)(Node_t * );
//    void (*ExtDef)(Node_t * );
//    FieldList * (*ExtDecList)(Node_t *,const FieldList * );
//    FieldList * (*ExtDec)(Node_t *,const FieldList * );

    FieldList * (*DefList)(Node_t * cur);
    FieldList * (*Def)(Node_t * cur);
    FieldList * (*DecList)(Node_t * cur,const FieldList * type);
    FieldList * (*Dec)(Node_t * cur,const FieldList *  type);
    FieldList * (*VarDec)(Node_t * root,const FieldList *  type);
    FieldList * (*Struct)(Node_t * );

    void (*ErrorHandling)(int ,int );

}Semantic_Check_t;
extern Semantic_Check_t * semantic_check;

#endif

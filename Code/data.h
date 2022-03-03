#ifndef STRUCT_H
#define STRUCT_H

typedef struct __Tree_node_t {
    char * content;
    enum {
        NONE,Sentinel,
    }type;
    int line;
    struct __Tree_node_t * lchild,* rchild,* left,* right;
}Node_t;

typedef Node_t* (* __Multiway_Api_)(Node_t * cur,Node_t* node);

typedef struct MultiwayTree_t{
    Node_t * root;

    __Multiway_Api_ lminsert;
    __Multiway_Api_ rminsert;
    __Multiway_Api_ remove;

    struct MultiwayTree_t* (*init)(struct MultiwayTree_t * t);
    Node_t * (* Node_alloc)(char * content,int line);
    void (* traverse)(Node_t * cur,int deep);
    void (*insert_all)(Node_t * cur,int argc,Node_t * childs[]);
}MultiwayTree_t;

#endif

extern MultiwayTree_t * tree;
#include "data.h"
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include "debug.h"
#include <stdarg.h>

static MultiwayTree_t * __MultiwayTree_init(MultiwayTree_t * t) {
    t->root = tree->Node_alloc("begin",0);
    return t;
}

static Node_t * __MultiwayTree_leftmostinsert(Node_t * cur ,Node_t * node) {
    Node_t * lastleft = cur->lchild;
    if (lastleft == NULL) {
        cur->lchild = cur->rchild = node;
    } else {
        cur->lchild = node;
        node->right = lastleft;
        lastleft->left = node;
    }
    return cur;
}

static Node_t * __MultiwayTree_rightmostinsert(Node_t * cur ,Node_t * node) {
    assert(cur != NULL);
    Node_t * lastright = cur->rchild;
    if (lastright == NULL) {
        cur->lchild = cur->rchild = node;
        node->right = node->left = NULL;
    } else {
        cur->rchild = node;
        node->left = lastright;
        lastright->right = node;
    }
    return cur;
}

static Node_t * __MultiwayTree_remove(Node_t * cur ,Node_t * node) {
    assert(0);
    return cur;
}

static Node_t * __MultiwayTree_Node_alloc(char * content,int line) {
    Node_t * new_node = (Node_t*)malloc(sizeof(Node_t));

    char * newstr = (char *)malloc(strlen(content)+5);
    // Log("%s %d",content,line);
    strcpy(newstr,content);
    new_node->content = newstr;
    new_node->line = line;
    new_node->type = NONE;
    new_node->rchild = new_node->lchild = new_node->right = new_node->left = NULL;


    return new_node;
}

static void __MultiwayTree_Traverse(Node_t * cur,int deep) {
    if (cur == NULL) return;
    for (int i = 0;i < deep;i++) {
        printf("\t");
    }
    printf("%s (%d)\n",cur->content,cur->line);
    if (cur->left != NULL) {
        Node_t * child = cur->lchild;
        do {
            __MultiwayTree_Traverse(child,deep + 1);
            child = child->right;
        }while (child != cur->right);
    }
}

void __MultiwayTree_insert_all(Node_t* cur,int argc,Node_t* childs[]) {
    for(int i = 0; i < argc;i++) {
        tree->rminsert(cur,childs[i]);
    }
}//注意，argc为childs数组参数的数量，同时注意可变参数的数量

extern MultiwayTree_t * tree;
MultiwayTree_t __Multiwaytree = {
    .init = __MultiwayTree_init,
    .lminsert = __MultiwayTree_leftmostinsert,
    .rminsert = __MultiwayTree_rightmostinsert,
    .Node_alloc = __MultiwayTree_Node_alloc,
    .remove = __MultiwayTree_remove,
    .traverse = __MultiwayTree_Traverse,
    .insert_all = __MultiwayTree_insert_all,
};
MultiwayTree_t * tree = &__Multiwaytree;


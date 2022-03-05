#include <malloc.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "data.h"
#include "debug.h"

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
    
    new_node->text[0] = 0;
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
        printf("  ");
    }
    if(cur->lchild == NULL) {
        printf("%s",cur->content);
        char ** s;
        if(strcmp(cur->content,"ID") == 0) {
            printf(": %s",cur->text);
        } else if(strcmp(cur->content,"INT") == 0) {
            printf(": %d",strtol(cur->text,s,0));
        } else if(strcmp(cur->content,"FLOAT") == 0) {
            printf(": %f",(float)atof(cur->text));
        } else if(strcmp(cur->content,"TYPE") == 0) {
            printf(": %s",cur->text);
        }
        printf("\n");
    } else {
        printf("%s (%d)\n",cur->content,cur->line);
    } 
    if (cur->lchild != NULL) {
        Node_t * child = cur->lchild;
        do {
            __MultiwayTree_Traverse(child,deep + 1);
            child = child->right;
        }while (child != NULL);
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

Node_t * Operator(Node_t * cur,char * content,int line,int argc,...) {
    cur = tree->Node_alloc(content,line);
    printf("%d: parent:%p %s:%s--- child:",argc,cur,cur->content,cur->text);
    va_list ap;
    va_start(ap,argc);
    for (int i = 0;i < argc;i++) {
        Node_t * temp = (Node_t*)va_arg(ap,Node_t*);
        if(temp == NULL) {printf("a nullptr--- "); continue; };
        printf("%p %s:%s--- ",temp,temp->content,temp->text);
        tree->rminsert(cur,temp);
    }
    va_end(ap);
    printf("\n");
    return cur;
}



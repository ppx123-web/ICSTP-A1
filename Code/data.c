#include "data.h"
#include "debug.h"

static MultiwayTree_t * MultiwayTree_init(MultiwayTree_t * t) {
//    t->root = tree->Node_alloc("begin",0);
    return t;
}

static Node_t * MultiwayTree_leftmostinsert(Node_t * cur ,Node_t * node) {
    Node_t * lastleft = cur->lchild;
    if (lastleft == NULL) {
        cur->lchild = cur->rchild = node;
    } else {
        cur->lchild = node;
        node->right = lastleft;
        lastleft->left = node;
        node->left = NULL;
    }
    return cur;
}

static Node_t * MultiwayTree_rightmostinsert(Node_t * cur ,Node_t * node) {
    assert(cur != NULL);
    Node_t * lastright = cur->rchild;
    if (lastright == NULL) {
        cur->lchild = cur->rchild = node;
        node->right = node->left = NULL;
    } else {
        cur->rchild = node;
        node->left = lastright;
        lastright->right = node;
        node->right = NULL;
    }
    return cur;
}

static Node_t * MultiwayTree_remove(Node_t * cur ,Node_t * node) {
    assert(0);
    return cur;
}

static Node_t * MultiwayTree_Node_alloc(char * content,int line) {
    Node_t * new_node = (Node_t*)malloc(sizeof(Node_t));
    //char * newstr = (char *)malloc(strlen(content)+5);

    memset(new_node->text,0,NAME_LENGTH);
    // Log("%s %d",content,line);
    strcpy(new_node->content,content);
    new_node->line = line;

    new_node->rchild = new_node->lchild = new_node->right = new_node->left = NULL;

    new_node->syn = new_node->inh = NULL;
    return new_node;
}

static void MultiwayTree_Traverse(Node_t * cur,int deep) {
    if (cur == NULL) return;
    for (int i = 0;i < deep;i++) {
        printf("  ");
    }
    if(cur->lchild == NULL) {
        printf("%s",cur->content);
        char * s = NULL;
        if(strcmp(cur->content,"ID") == 0) {
            printf(": %s",cur->text);
        } else if(strcmp(cur->content,"INT") == 0) {
            printf(": %d",(int)strtol(cur->text,&s,0));
        } else if(strcmp(cur->content,"FLOAT") == 0) {
            printf(": %f",(float)strtod(cur->text,&s));
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
            MultiwayTree_Traverse(child,deep + 1);
            child = child->right;
        }while (child != NULL);
    }
}

void MultiwayTree_insert_all(Node_t* cur,int argc,Node_t* childs[]) {
    for(int i = 0; i < argc;i++) {
        tree->rminsert(cur,childs[i]);
    }
}//注意，argc为childs数组参数的数量，同时注意可变参数的数量


MultiwayTree_t Multiwaytree = {
        .init = MultiwayTree_init,
        .lminsert = MultiwayTree_leftmostinsert,
        .rminsert = MultiwayTree_rightmostinsert,
        .Node_alloc = MultiwayTree_Node_alloc,
        .remove = MultiwayTree_remove,
        .traverse = MultiwayTree_Traverse,
        .insert_all = MultiwayTree_insert_all,
};

MultiwayTree_t * tree = &Multiwaytree;

Node_t * Operator(Node_t * cur,char * content,int line,int argc,...) {
    cur = tree->Node_alloc(content,line);
    Treedebug("%d: parent:%p %s:%s--- child:",argc,cur,cur->content,cur->text);
    va_list ap;
    va_start(ap,argc);
    for (int i = 0;i < argc;i++) {
        Node_t * temp = (Node_t*)va_arg(ap,Node_t*);
        if(temp == NULL) {
            Treedebug("a nullptr--- "); continue; 
        };
        Treedebug("%p %s:%s--- ",temp,temp->content,temp->text);
        tree->rminsert(cur,temp);
    }
    va_end(ap);
    Treedebug("\n");
    return cur;
}



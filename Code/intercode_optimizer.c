#include "intercode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

extern CodeList_t code_list;


typedef struct Code_Block_t {
    int first_code,end_code;
    InterCode * head,* tail;
}Code_Block_t;

Code_Block_t * code_block;

static void code_optimizer_init(int size);
static void code_optimizer_finish(int size);
static int partition_code_block();
static CodeList_t * code_optimizer_code_blocks(int size);
static CodeList_t * code_optimizer_block(hashmap * map);


CodeList_t * code_optimizer(int size) {
    CodeList_t origin = code_list;
    codelist_init(&code_list);
    code_optimizer_init(size);
    int code_block_size = partition_code_block();
    CodeList_t * ret = code_optimizer_code_blocks(size);
    code_optimizer_finish(code_block_size);

    return ret;
}

static void code_optimizer_init(int size) {
    code_block = (Code_Block_t *) malloc(sizeof(Code_Block_t) * size);
}

static void code_optimizer_finish(int size) {
    free(code_block);
}

static int partition_code_block() {
    InterCode * cur = code_list.head.next;
    int id = -1;
    code_block[++id].first_code = cur->line;
    code_block[id].head = cur;
    cur = cur->next;
    while (cur != &code_list.tail) {
        cur->block = id;
        switch (cur->kind) {
            case T_GO:
            case T_IF:
                code_block[id].end_code = cur->line;
                code_block[id].tail = cur;
                id++;
                code_block[id].first_code = cur->line + 1;
                code_block[id].head = cur->next;
                break;
            case T_LABEL:
                if (code_block[id].first_code != cur->line) {
                    code_block[id].end_code = cur->line - 1;
                    code_block[id].tail = cur->prev;
                    id++;
                    code_block[id].first_code = cur->line;
                    code_block[id].head = cur;
                }
                break;
            default:;
        }
        cur = cur->next;
    }
    return id;
}

typedef struct Graph_Node_t {
    union {
        struct {
            vector arg_list;
            Operand arg1,arg2;
        };
        Operand * var;
    };

    enum {
        G_VAR, G_ADD, G_MINUS, G_MUL, G_DIV, G_ASSIGN, G_A_STAR, G_STAR_A,
    } kind;
    int line,vis;
    struct Graph_Node_t * left, * right;
}Graph_Node_t;

typedef struct map_pair {
    Operand a;
    Operand b;
}map_pair;


typedef struct hashmap hashmap_t;

static int hashcompare(void * ka,void * kb) {
    const Operand * a = ka, * b = kb;
    if(a->kind == b->kind) {
        if(a->kind == CONSTANT) {
            return (strcmp(a->var.value,b->var.value) == 0);
        } else {
            return a->var.var_no == b->var.var_no;
        }
    } else {
        return 0;
    }
}

#define new(A) ((A*)malloc(sizeof(A)))

static int compare_two_operand(void * ka,void * kb) {
    return hashcompare(ka,kb) && hashcompare(ka + sizeof(Operand),kb + sizeof(Operand));
}

static Operand empty_operand = {
        .kind = 0,
        .var.var_no = 0,
};

static Graph_Node_t * search_operand_op(hashmap_t * map,Operand * a,Operand * b) {
    assert(map->key_size == 2 * sizeof(Operand));
    if(b == NULL) {
        map_pair m = {
                .a = *a,
                .b = empty_operand,
        };
        return hashmap_find(map,&m);
    } else {
        map_pair m = {
                .a = *a,
                .b = *b,
        };
        return hashmap_find(map,&m);
    }
}

static void code_optimizer_code(void * keydata,void * valuedata) {
    Graph_Node_t * node = valuedata;
    if(node == NULL) return;
    if(node->kind == G_VAR) {
        int size = vector_size(&node->arg_list);
        Operand val = *(Operand*)vector_id(&node->arg_list,0);
        if(size >= 2) {
            for(int i = 1;i < size;i++) {
                gencode(T_ASSIGN, *(Operand*)vector_id(&node->arg_list,i), val);
            }
        }
    }
    if(node->vis == 1) {
        return;
    } else {
#define OPZERO (*(Operand*)vector_id(&node->arg_list,0))
#define OPFIRST (node->arg1)
#define OPSECOND (node->arg2)
        node->vis = 1;
        code_optimizer_code(NULL,node->left);
        code_optimizer_code(NULL,node->right);
        switch (node->kind) {
            case G_ADD:
                gencode(G_ADD, OPZERO, OPFIRST, OPSECOND);
                break;
            case G_MINUS:
                gencode(G_MINUS, OPZERO, OPFIRST, OPSECOND);
                break;
            case G_MUL:
                gencode(G_MUL, OPZERO, OPFIRST, OPSECOND);
                break;
            case G_DIV:
                gencode(G_DIV, OPZERO, OPFIRST, OPSECOND);
                break;
            case G_A_STAR:
                gencode(G_A_STAR, OPZERO, OPFIRST, OPSECOND);
                break;
            case G_STAR_A:
                gencode(G_STAR_A, OPZERO, OPFIRST, OPSECOND);
                break;
            case G_ASSIGN:
                gencode(G_ASSIGN, OPZERO, OPFIRST, OPSECOND);
                break;
            default:
                ;
        }
        int size = vector_size(&node->arg_list);
        Operand val = *(Operand*)vector_id(&node->arg_list,0);
        if(size >= 2) {
            for(int i = 1;i < size;i++) {
                gencode(T_ASSIGN, *(Operand*)vector_id(&node->arg_list,i), val);
            }
        }
    }
}

static CodeList_t * code_optimizer_block(hashmap_t * map) {
    CodeList_t * ret = (CodeList_t*) malloc(sizeof(CodeList_t));
    ret->head.next = &ret->tail;
    ret->tail.prev = &ret->head;
    hashmap_traverse(map,code_optimizer_code);
    //消除公共子表达式，
}


static CodeList_t * code_optimizer_dag(Code_Block_t * block) {
    InterCode * code = block->head;
    int graph_size = (block->end_code - block->first_code + 5) * 3;
    hashmap_t operand_map,op_map;
    hashmap_init(&operand_map,graph_size, sizeof(Operand), sizeof(Graph_Node_t),NULL,hashcompare);
    hashmap_init(&op_map,graph_size, 2 * sizeof(Operand), sizeof(Graph_Node_t),NULL,compare_two_operand);
    Graph_Node_t * root = NULL;
    while (code != block->tail) {
        Graph_Node_t node, * cur = &node, * temp = NULL;
        cur->left = cur->right = NULL;
        node.vis = 0;
        switch (code->kind) {
            case T_ADD:
                cur->kind = G_ADD;
                goto NODE1;
            case T_MINUS:
                cur->kind = G_MINUS;
                goto NODE1;
            case T_MUL:
                cur->kind = G_MUL;
                goto NODE1;
            case T_DIV:
                cur->kind = G_DIV;
                goto NODE1;
            NODE1:
                // x = y op z
                cur->arg1 = code->op.args[1];
                cur->arg2 = code->op.args[2];
                for(int i = 1;i < 3;i++) {
                    if(!hashmap_find(&operand_map,&code->op.args[i])) {
                        hashmap_set(&operand_map,&code->op.args[i],&(Graph_Node_t){.kind = G_VAR, .var = &code->op.args[i]});
                    }
                }//变量没有节点则为变量在表中构建一个节点
                temp = search_operand_op(&op_map,&code->op.args[1],&code->op.args[2]);
                //查看是否有使用y和z进行计算的节点
                if(temp == NULL || temp->kind != cur->kind) {
                    //没有，则将node插入到表中，维护其左右子节点
                    cur->left = hashmap_find(&operand_map,&code->op.args[1]);
                    cur->right = hashmap_find(&operand_map,&code->op.args[2]);

                    vector_init(&cur->arg_list, sizeof(Operand),8);
                    vector_push_back(&cur->arg_list,&code->op.args[0]);
                    hashmap_set(&op_map,&(map_pair){.a = code->op.args[1],.b = code->op.args[2]},cur);
                } else {
                    vector_push_back(&temp->arg_list,&code->op.args[0]);
                }
                //更新x对应的节点
                hashmap_set(&operand_map,&code->op.args[0],cur);
                break;
            case T_A_STAR:
                cur->kind = G_A_STAR;
                goto NODE2;
            case T_STAR_A:
                cur->kind = G_STAR_A;
            NODE2:
                if(!hashmap_find(&operand_map,&code->op.args[1])) {
                    hashmap_set(&operand_map,&code->op.args[1],&(Graph_Node_t){.kind = G_VAR, .var = &code->op.args[1]});
                }
                temp = search_operand_op(&op_map,&code->op.args[1],NULL);
                //查看是否有使用y
                if(temp == NULL || temp->kind != cur->kind) {
                    //没有，则将node插入到表中，维护其左子节点
                    cur->left = hashmap_find(&operand_map,&code->op.args[1]);
                    cur->right = NULL;

                    vector_init(&cur->arg_list, sizeof(Operand),8);
                    vector_push_back(&cur->arg_list,&code->op.args[0]);
                    hashmap_set(&op_map,&(map_pair){.a = code->op.args[1],.b = empty_operand},cur);
                } else {
                    vector_push_back(&temp->arg_list,&code->op.args[0]);
                }
                //更新x对应的节点
                hashmap_set(&operand_map,&code->op.args[0],cur);
                break;
            case T_ASSIGN:
                cur->kind = G_ASSIGN;
                if(!hashmap_find(&operand_map,&code->op.args[1])) {
                    hashmap_set(&operand_map,&code->op.args[1],&(Graph_Node_t){.kind = G_VAR, .var = &code->op.args[1]});
                }
                temp = search_operand_op(&op_map,&code->op.args[1],NULL);
                if(temp == NULL || temp->kind != cur->kind) {
                    //没有，则将node插入到表中，
                    vector_init(&temp->arg_list, sizeof(Operand),8);
                    vector_push_back(&temp->arg_list,&code->op.args[0]);
                } else {
                    vector_push_back(&temp->arg_list,&code->op.args[0]);
                }
                break;
            default:
                ;
        }
        code = code->next;
    }
    CodeList_t * ret = code_optimizer_block(&operand_map);
    //TODO free hashmap vector
    return ret;
}




static CodeList_t * code_optimizer_code_blocks(int size) {
    CodeList_t * ret = (CodeList_t*) malloc(sizeof(CodeList_t));
    ret->head.next = &ret->tail;
    ret->tail.prev = &ret->head;
    for(int i = 0;i < size;i++) {
        CodeList_t * temp = code_optimizer_dag(&code_block[i]);
        ret->tail.prev = temp->head.next;
        temp->head.next->prev = ret->tail.prev;
        temp->tail.prev->next = &ret->tail;
        ret->tail.prev = temp->tail.prev;
        free(temp);
    }
}




#include "intercode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

extern CodeList_t code_list;
extern int * variable_map;

static void gencode(CodeList_t * cl,int kind,...) {
    va_list ap;
    va_start(ap,kind);
    InterCode * code = u_gencode(kind,ap);
    va_end(ap);
    if(code) {
        codelist_insert(cl,cl->tail.prev,code);
    }
}

static void code_optimizer_remove_const_assign(int);

CodeList_t * code_optimizer(int size) {
    code_optimizer_remove_const_assign(size);
    return &code_list;
}

static void code_optimizer_remove_const_assign(int size) {
    InterCode * cur = code_list.head.next, * temp = NULL;
    typedef struct maptable {
        int use;
        int value;
    }maptable;
    maptable * map = malloc(sizeof(maptable) * size * 2);
    memset(map,0, sizeof(maptable) * size * 2);
    while (cur != code_list.tail.next) {
        temp = cur->next;
        if(cur->kind == T_ASSIGN) {
            if(cur->op.assign.arg2.kind == INT_CONST && cur->op.assign.arg1.kind == VARIABLE) {
                map[cur->op.assign.arg1.var.var_no].value = cur->op.assign.arg2.var.int_const;
                map[cur->op.assign.arg1.var.var_no].use = 1;
                cur->prev->next = temp;
                temp->prev = cur->prev;
                free(cur);
            }
        } else {
            if(cur->kind == T_RETURN) {
                if(cur->op.args[0].kind == VARIABLE && map[cur->op.args[0].var.var_no].use) {
                    cur->op.args[0].kind = INT_CONST;
                    cur->op.args[0].var.int_const = map[cur->op.args[0].var.var_no].value;
                }
            }
            for(int i = 1;i < 3;i++) {
                if(cur->kind != T_EMPTY) {
                    if(cur->op.args[i].kind == VARIABLE && map[cur->op.args[i].var.var_no].use) {
                        cur->op.args[i].kind = INT_CONST;
                        cur->op.args[i].var.int_const = map[cur->op.args[i].var.var_no].value;
                    }
                }
            }
        }
        cur = temp;
    }
    free(map);
}

//========================================================================

typedef struct Graph_Node_t {
    vector arg_list;
    union {
        struct {
            Operand arg1;
            Operand arg2;
        };
        Operand args[3];
    };

    enum {
        G_VAR, G_ADD, G_MINUS, G_MUL, G_DIV, G_ASSIGN, G_A_STAR, G_STAR_A,
    } kind;
    int line;
}Graph_Node_t;

typedef struct hashmap hashmap_t;

typedef struct Code_Block_t {
    int first_code,end_code;
    InterCode * head,* tail;
}Code_Block_t;
Code_Block_t * code_block;

static Graph_Node_t ** block_op_map;

static void code_optimizer_init(int size);
static void code_optimizer_finish(int size);
static int partition_code_block();
static CodeList_t * code_optimizer_code_blocks(int size);
static CodeList_t * code_optimizer_block(hashmap * map);
static CodeList_t code_optimizer_dag(Code_Block_t * block);
static void block_op_map_init();

static void block_dag(int size) {
    code_optimizer_init(size);
    int code_block_size = partition_code_block();
    CodeList_t * ret = code_optimizer_code_blocks(code_block_size);
    code_optimizer_finish(code_block_size);
}


static CodeList_t * code_optimizer_code_blocks(int size) {
    CodeList_t * ret = (CodeList_t*) malloc(sizeof(CodeList_t));
    ret->head.next = &ret->tail;
    ret->tail.prev = &ret->head;
    CodeList_t temp;
    for(int i = 0;i < size;i++) {
//        printf("%d::begin:%d, end:%d\n",i,code_block[i].first_code,code_block[i].end_code);
        block_op_map_init();
        temp = code_optimizer_dag(&code_block[i]);
        ret->tail.prev = temp.head.next;
        temp.head.next->prev = ret->tail.prev;
        temp.tail.prev->next = &ret->tail;
        ret->tail.prev = temp.tail.prev;
    }
}


static void code_optimizer_init(int size) {
    code_block = (Code_Block_t *) malloc(sizeof(Code_Block_t) * size);
    memset(code_block,0,sizeof(Code_Block_t) * size);
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
            case T_RETURN:
                code_block[id].end_code = cur->line;
                code_block[id].tail = cur;
                id++;
                code_block[id].first_code = cur->line + 1;
                code_block[id].head = cur->next;
                break;
            case T_LABEL:
            case T_FUNCTION:
                if (code_block[id].first_code != cur->line) {
                    code_block[id].end_code = cur->line - 1;
                    code_block[id].tail = cur->prev;
                    id++;
                    code_block[id].first_code = cur->line;
                    code_block[id].head = cur;
                }
            default:;
        }
        cur = cur->next;
    }
    code_block[id].end_code = code_list.tail.prev->line;
    return id;
}

static int block_op_map_size = 0;
static void block_op_map_init(int block_size) {
    block_op_map = malloc(sizeof(typeof(block_op_map)) * block_size * 2);
}

static void block_op_map_delete() {
    free(block_op_map);
}

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

static int gen_block_op(Graph_Node_t * node) {
    block_op_map[block_op_map_size++] = node;
}

static int operand_equal(Operand * a,Operand * b) {
    if(a->kind == b->kind) {
        if((a->kind == VARIABLE && a->var.var_no == b->var.var_no)
            || (a->kind == INT_CONST && a->var.int_const == b->var.int_const)) {
            return 1;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}

static Operand * operand_vector_id(vector* vec,int id) {
    return vector_id(vec,0);
}

static Operand * search_op_map_first(hashmap_t * map, Operand * a) {
    Graph_Node_t * node = hashmap_find(map,a);
    if(node) {
        return operand_vector_id(&node->arg_list,0);
    } else {
        return &;
    }
}

static Graph_Node_t * search_operand_op(hashmap_t * map,Operand * a,Operand * b) {
    assert(map->key_size == 2 * sizeof(Operand));
    for(int i = block_op_map_size - 1;i >= 0;i--) {
        if(operand_equal(&block_op_map[i]->arg1,search_op_map_first(map,a))
        && operand_equal(&block_op_map[i]->arg2,search_op_map_first(map,b))) {
            return block_op_map[i];
        }
    }
    return NULL;
}

#define new(A) ((A*)malloc(sizeof(A)))

static int compare_two_operand(void * ka,void * kb) {
    return hashcompare(ka,kb) && hashcompare(ka + sizeof(Operand),kb + sizeof(Operand));
}

static Operand empty_operand = {
        .kind = 0,
        .var.var_no = 0,
};

static CodeList_t code_optimizer_dag(Code_Block_t * block) {
    CodeList_t ret;
    ret.head.next = &ret.tail;
    ret.tail.prev = &ret.head;

    InterCode * code = block->head;
    if(!code || block->first_code > block->end_code) return ret;
    int graph_size = (block->end_code - block->first_code + 5) * 3;
    hashmap_t operand_map;
    hashmap_init(&operand_map,graph_size, sizeof(Operand), sizeof(Graph_Node_t),NULL,hashcompare,NULL,NULL);
//    hashmap_init(&op_map,graph_size, sizeof(map_pair), sizeof(Graph_Node_t),NULL,compare_two_operand,NULL,NULL);
    while (code != block->tail->next) {
        Graph_Node_t node, * cur = &node, * temp = NULL;
        InterCode * temp_code = NULL;
        for(int i = 1;i < 3;i++) {
            if(code->kind) {
                cur->args[i] = code->op.args[i];
            } else {
                cur->args[i] = empty_operand;
            }
            if(code->kind && code->op.args[i].kind == VARIABLE && !hashmap_find(&operand_map,&code->op.args[i])) {
                Graph_Node_t arg_node = {
                        .kind = G_VAR,
                };
                vector_init(&arg_node.arg_list, sizeof(Operand),8);
                vector_push_back(&arg_node.arg_list,&code->op.args[i]);
                hashmap_set(&operand_map,&code->op.args[i],&arg_node);
            }
        }//变量没有节点则为变量在表中构建一个节点
        Operand * arg1 = NULL, * arg2 = NULL;
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
                //TODO 优化需要使用args[1] args[2]的对应的图中的节点的arg list中第一个有效的节点来找search_operand_op
                temp = search_operand_op(&operand_map,&cur->arg1,&cur->arg2);
                //查看是否有使用y和z进行计算的节点
                if(temp && temp->kind == cur->kind && !variable_map[operand_vector_id(&temp->arg_list,0)->var.var_no]) {//节点存在
                    vector_push_back(&temp->arg_list,&code->op.args[0]);
                    //gencode(&ret,T_ASSIGN,code->op.args[0], *(Operand*)vector_id(&temp->arg_list,0));
                } else {
                    vector_init(&cur->arg_list, sizeof(Operand),8);
                    vector_push_back(&cur->arg_list,&code->op.args[0]);
                    hashmap_set(&op_map,&(map_pair){
                            .a = *search_op_vector(&operand_map,&code->op.args[1]),
                            .b = *search_op_vector(&operand_map,&code->op.args[2]) },cur);
                    gencode(&ret,code->kind,code->op.args[0],code->op.args[1],code->op.args[2]);
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
                temp = search_operand_op(&op_map,search_op_vector(&operand_map,&code->op.args[1]),NULL);
                //查看是否有使用y
                if(temp && temp->kind == cur->kind) {
                    vector_push_back(&temp->arg_list,&code->op.args[0]);
                    //gencode(&ret,T_ASSIGN,code->op.args[0], *(Operand*)vector_id(&temp->arg_list,0));
                } else {
                    vector_init(&cur->arg_list, sizeof(Operand),8);
                    vector_push_back(&cur->arg_list,&code->op.args[0]);
                    hashmap_set(&op_map,&(map_pair){
                            .a =*search_op_vector(&operand_map,&code->op.args[1]),
                            .b = empty_operand},cur);
                    gencode(&ret,code->kind,code->op.args[0],code->op.args[1]);
                }
                hashmap_set(&operand_map,&code->op.args[0],cur);
                break;
            case T_ASSIGN:
                cur = hashmap_find(&operand_map,&code->op.args[1]);
                vector_push_back(&cur->arg_list,&code->op.args[0]);
                hashmap_set(&operand_map,&code->op.args[0],cur);
                gencode(&ret,T_ASSIGN,code->op.assign.arg1,code->op.assign.arg2);
                break;
            default:
                temp_code = new(InterCode);
                memcpy(temp_code,code, sizeof(InterCode));
                codelist_insert(&ret,ret.tail.prev,temp_code);
                intercode_display(temp_code);
        }
        code = code->next;
    }
    //TODO free hashmap vector
    return ret;
}





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
static CodeList_t * block_dag(int size);

CodeList_t * code_optimizer(int size) {
//    code_optimizer_remove_const_assign(size);
    return block_dag(size);
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

static Graph_Node_t * block_op_map;

static void code_optimizer_init(int size);
static void code_optimizer_finish(int size);
static int partition_code_block();
static CodeList_t * code_optimizer_code_blocks(int size);
static CodeList_t * code_optimizer_block(hashmap * map);
static CodeList_t code_optimizer_dag(Code_Block_t * block);
static void block_op_map_init();
static void block_op_map_delete();

static CodeList_t * block_dag(int size) {
    code_optimizer_init(size);
    int code_block_size = partition_code_block();
    CodeList_t * ret = code_optimizer_code_blocks(code_block_size);
    code_optimizer_finish(code_block_size);
    return ret;
}


static CodeList_t * code_optimizer_code_blocks(int size) {
    CodeList_t * ret = (CodeList_t*) malloc(sizeof(CodeList_t));
    codelist_init(ret);
    CodeList_t temp;
    for(int i = 0;i < size;i++) {
//        printf("%d::begin:%d, end:%d\n",i,code_block[i].first_code,code_block[i].end_code);
        block_op_map_init(code_block[i].end_code - code_block[i].first_code);
        temp = code_optimizer_dag(&code_block[i]);
        ret->tail.prev->next = temp.head.next;
        temp.head.next->prev = ret->tail.prev;
        temp.tail.prev->next = &ret->tail;
        ret->tail.prev = temp.tail.prev;
        block_op_map_delete();
    }
    return ret;
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
    block_op_map = malloc(sizeof(Graph_Node_t) * block_size * 3);
    block_op_map_size = 0;
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

static void gen_block_op(Graph_Node_t node) {
    block_op_map[block_op_map_size++] = node;
}

static Operand empty_operand = {
        .kind = 0,
        .var.var_no = 0,
};

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
    return vector_id(vec,id);
}

static Operand * search_op_map_first(hashmap_t * map, Operand * a) {
    Graph_Node_t * node = hashmap_find(map,a);
    if(node) {
        return operand_vector_id(&node->arg_list,0);
    } else {
        return &empty_operand;
    }
}

static Graph_Node_t * search_operand_op(hashmap_t * map,Operand * a,Operand * b,int kind) {
    for(int i = block_op_map_size - 1;i >= 0;i--) {
        if(kind == block_op_map[i].kind && operand_equal(&block_op_map[i].args[1],search_op_map_first(map,a))
            && operand_equal(&block_op_map[i].args[2],search_op_map_first(map,b))) {
            return &block_op_map[i];
        }
    }
    return NULL;
}
//找到a和b所在节点对应的图节点的arg list的第一个元素，然后在block_op_map中遍历查找是否已经计算过

static void test_hash_var(hashmap_t * map,Operand * a) {
    Graph_Node_t * node = hashmap_find(map,a);
    operand_display(a);
    if(node) {
        printf("-find:");
        operand_display(operand_vector_id(&node->arg_list,0));
    } else {
        printf("-Not find");
    }
    printf("\n");
}

static CodeList_t code_optimizer_dag(Code_Block_t * block) {
    CodeList_t ret;
    codelist_init(&ret);

    InterCode * code = block->head;
    if(!code || block->first_code > block->end_code) return ret;
    int graph_size = (block->end_code - block->first_code + 5) * 3;
    hashmap_t operand_map;
    hashmap_init(&operand_map,graph_size, sizeof(Operand), sizeof(Graph_Node_t),NULL,hashcompare,NULL,NULL);
    while (code != block->tail->next) {
        Graph_Node_t node, * cur = &node, * temp = NULL;
        InterCode * temp_code = NULL;
        Operand * arg[3] = {NULL};
        for(int i = 1;i < 3;i++) {
            if(code->op.args[i].kind && !hashmap_find(&operand_map,&code->op.args[i])) {
                Graph_Node_t arg_node = {
                        .kind = G_VAR,
                };
                vector_init(&arg_node.arg_list, sizeof(Operand),8);
                vector_push_back(&arg_node.arg_list,&code->op.args[i]);
                hashmap_set(&operand_map,&code->op.args[i],&arg_node);
            }
        }//变量没有节点则为变量在表中构建一个节点
        for(int i = 0;i < 3;i++) {
            if(code->op.args[i].kind && hashmap_find(&operand_map,&code->op.args[i])) {
                arg[i] = operand_vector_id(
                        &((Graph_Node_t*)hashmap_find(&operand_map,&code->op.args[i]))->arg_list,0);
            } else if(code->op.args[i].kind) {
                arg[i] = &code->op.args[i];
            } else {
                arg[i] = &empty_operand;
            }
        }
        for(int i = 1;i < 3;i++) {
            if(code->op.args[i].kind) {
                cur->args[i] = *arg[i];
            } else {
                cur->args[i] = empty_operand;
            }
        }
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
                temp = search_operand_op(&operand_map,arg[1],arg[2],cur->kind);
                if(temp && temp->kind == cur->kind && !variable_map[operand_vector_id(&temp->arg_list,0)->var.var_no]) {
                    vector_push_back(&temp->arg_list,&code->op.args[0]);
                    hashmap_set(&operand_map,&code->op.args[0],cur);
                    cur = temp;
                } else {
                    vector_init(&cur->arg_list, sizeof(Operand),8);
                    vector_push_back(&cur->arg_list,&code->op.args[0]);
                    gen_block_op(node);
                    gencode(&ret,code->kind,*arg[0],*arg[1],*arg[2]);
                }
                hashmap_set(&operand_map,&code->op.args[0],cur);
                break;
            case T_A_STAR:
                cur->kind = G_A_STAR;
                goto NODE2;
            case T_STAR_A:
                cur->kind = G_STAR_A;

            NODE2:
                temp = search_operand_op(&operand_map,arg[1],&empty_operand,cur->kind);
                //查看是否有使用y
                if(temp && temp->kind == cur->kind && !variable_map[operand_vector_id(&temp->arg_list,0)->var.var_no]) {
                    vector_push_back(&temp->arg_list,&code->op.args[0]);
                    cur = temp;
                } else {
                    vector_init(&cur->arg_list, sizeof(Operand),8);
                    vector_push_back(&cur->arg_list,&code->op.args[0]);
                    gen_block_op(node);
                    gencode(&ret,code->kind,*arg[0],*arg[1]);
                }
                hashmap_set(&operand_map,&code->op.args[0],cur);
                break;
            case T_ASSIGN:
                cur = hashmap_find(&operand_map,&code->op.args[1]);
                if(cur && !variable_map[arg[1]->var.var_no]) {
                    vector_push_back(&cur->arg_list,&code->op.args[0]);
                } else {
                    cur = &node;
                    gencode(&ret,T_ASSIGN,code->op.assign.arg1, *arg[1]);
                    vector_init(&cur->arg_list, sizeof(Operand),8);
                    vector_push_back(&cur->arg_list,&code->op.args[0]);
                }
                hashmap_set(&operand_map,&code->op.args[0],cur);
                //test_hash_var(&operand_map,&(Operand){.kind = VARIABLE,.var.var_no = 5});
                break;
            default:
                for(int i = 0;i < 3;i++) {
                    if(code->op.args[i].kind == VARIABLE) {
                        Graph_Node_t * o1 = hashmap_find(&operand_map,&code->op.args[i]);
                        if(o1) {
                            code->op.args[i] = *operand_vector_id(&o1->arg_list,0);
                        }
                    }
                }
                temp_code = malloc(sizeof(InterCode));
                memcpy(temp_code,code, sizeof(InterCode));
                codelist_insert(&ret,ret.tail.prev,temp_code);
#ifdef INTERCODE_DEBUG
                intercode_display(temp_code);
#endif
        }
        code = code->next;
    }
    //TODO free hashmap vector
    return ret;
}





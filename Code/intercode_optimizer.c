#include "intercode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern CodeList_t code_list;

typedef struct Info_Use_later {
    enum {
        NONEXIST, ACTIVATE , DEACTIVATE ,
    } type;
    InterCode * connect;
}Info_Use_later;


typedef struct Code_Block_t {
    int first_code,end_code;
    InterCode * head,* tail;
    Info_Use_later * info;
}Code_Block_t;

Code_Block_t * code_block;

static void code_optimizer_init(int size);
static void code_optimizer_finish(int size);
static int partition_code_block();
static CodeList_t * code_optimizer_code_blocks(int size);
static CodeList_t * code_optimizer_block(Code_Block_t * block);

CodeList_t * code_optimizer(int size) {
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
    for(int i = 0;i < size;i++) {
        free(code_block[i].info);
    }
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

//static Info_Use_later * search_info(int var_no,Code_Block_t * block) {
//    return &block->info[var_no - block->first_code];
//}

//static void code_optimizer_info_use_later(Code_Block_t * block) {
//#define BIAS(A) ((A) - block->first_code)
//    block->info = (Info_Use_later *) malloc(3 * (block->end_code - block->first_code + 1) * sizeof(Info_Use_later));
//    memset(block->info,0, (block->end_code - block->first_code + 1) * sizeof(Info_Use_later));
//    for(InterCode * cur = block->tail;cur != block->head; cur = cur->prev) {
//        switch (cur->kind) {
//            case T_ASSIGN:
//                break;
//            case T_ADD:
//            case T_MINUS:
//            case T_MUL:
//            case T_DIV:
//                block->info[BIAS(cur->line)].connect =
//        }
//    }
//}

typedef struct Graph_Node_t {
    int var_list;
    int kind; //use inter code kind
}Graph_Node_t;

static CodeList_t * code_optimizer_code_blocks(int size) {
    CodeList_t * ret = (CodeList_t*) malloc(sizeof(CodeList_t));
    ret->head.next = &ret->tail;
    ret->tail.prev = &ret->head;
    for(int i = 0;i < size;i++) {
        //TODO
        //分析每个基本块的后续使用信息，P340
        //构建DAG，P343
        //消除公共子表达式
        CodeList_t * temp = code_optimizer_block(&code_block[i]);
        ret->tail.prev = temp->head.next;
        temp->head.next->prev = ret->tail.prev;
        temp->tail.prev->next = &ret->tail;
        ret->tail.prev = temp->tail.prev;
        free(temp);
    }
}

static CodeList_t * code_optimizer_block(Code_Block_t * block) {
    CodeList_t * ret = (CodeList_t*) malloc(sizeof(CodeList_t));
    ret->head.next = &ret->tail;
    ret->tail.prev = &ret->head;
    //消除公共子表达式，
}




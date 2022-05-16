#include "intercode.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>


void vector_init(vector * vec,int ele_size,int capacity,void (* deallocator)(void *)) {
    vec->size = 0;
    vec->ele_size = ele_size;
    vec->capacity = capacity;
    vec->data = malloc(vec->ele_size * capacity);

    vec->deallocator = deallocator ? deallocator : NULL;
}

vector * vector_alloc_init(int ele_size,int capacity,void (* deallocator)(void *)) {
    vector * vec = new(vector);

    vec->size = 0;
    vec->ele_size = ele_size;
    vec->capacity = capacity;
    vec->data = malloc(vec->ele_size * capacity);

    vec->deallocator = deallocator ? deallocator : NULL;

    return vec;
}

void vector_resize(vector * vec,int cap) {
    vec->capacity = cap;
    void * ndata = malloc(vec->ele_size * cap);
    memcpy(ndata,vec->data,vec->ele_size * vec->size);
    free(vec->data);
    vec->data = ndata;
}

void vector_push_back(vector * vec,void * udata) {
    if(vec->size == vec->capacity) {
        vector_resize(vec,vec->capacity * 2);
    }
    void * p = vec->data + vec->ele_size * vec->size;
    memcpy(p,udata,vec->ele_size);
    vec->size++;
}//如果udata中含有指针，则要进行移动语义，需要移动构造函数，这里不实现

void vector_delete(vector * vec) {
    for(int i = 0;i < vec->size;i++) {
        void * ptr = vec->data + vec->ele_size * i;
        if(vec->deallocator) {
            vec->deallocator(ptr);
        }
    }
    free(vec->data);
}

void * vector_id(vector * vec,int id) {
    if(id >= vec->size) {
        assert(0);
    } else {
        return vec->data + vec->ele_size * id;
    }
}

int vector_size(vector * vec) {
    return vec->size;
}


static int hashdata(hashmap * map,void * data,int size) {
    int val = 0,i,id = 0;
    char * name = data;
    for(; id < size; ++name,++id) {
        val = (val << 2) + *name;
        if ((i = val & map->bucket_capacity)) {
            val = (val ^ (i >> 12)) & map->bucket_capacity;
        }
    }
    return val;
}

void hashmap_init(hashmap * map,int bucket_capacity,int key_size,int value_size,
                int (*hash)(struct hashmap * map,void * keydata,int size),int (*compare)(void * ka,void * kb),
                void (*deallocator)(void *)) {
    map->bucket_capacity = bucket_capacity;
    map->key_size = key_size;
    map->value_size = value_size;
    map->unit_size = key_size + value_size;
    map->size = 0;

    map->hash = hash ? hash : hashdata;
    map->compare = compare;
    assert(map->hash && map->compare);
    map->deallocator = deallocator ? deallocator : free;

    map->bucket = malloc(bucket_capacity * sizeof(hashmap_node_t));
    for(int i = 0;i < bucket_capacity;i++) {
        map->bucket[i].keydata = map->bucket[i].valuedata = NULL;
        map->bucket[i].next = NULL;
    }
}

void hashmap_delete(hashmap * map,void * keydata) {
    int entry = map->hash(map,keydata,map->key_size) % map->bucket_capacity;
    hashmap_node_t * node = &map->bucket[entry];
    hashmap_node_t * cur = node->next, * prev = node, * next;
    while (cur) {
        next = cur->next;
        if(map->compare(cur->keydata,keydata)) {
            prev->next = next;
            free(cur->keydata);
            if(map->deallocator) {
                map->deallocator(cur->data.second);
            }
            free(cur->valuedata);
            free(cur);
            return;
        }
        prev = cur;
        cur = next;
    }
}

typedef struct Graph_Node_t {
    vector arg_list;
    Operand arg1,arg2;

    enum {
        G_VAR, G_ADD, G_MINUS, G_MUL, G_DIV, G_ASSIGN, G_A_STAR, G_STAR_A,
    } kind;
    int line;
}Graph_Node_t;


void hashmap_set(hashmap * map,void * keydata,void * valuedata) {
    int entry = map->hash(map,keydata,map->key_size) % map->bucket_capacity;
    hashmap_node_t * node = &map->bucket[entry];

    void * find = hashmap_find(map,keydata);
    if(find) {
        if(map->deallocator) {
            map->deallocator(find);
        }
        memcpy(find,valuedata,map->value_size);
    } else {
        hashmap_node_t * temp = node->next;
        hashmap_node_t * cur = malloc(sizeof(hashmap_node_t));
        node->next = cur;
        cur->next = temp;

        cur->keydata = malloc(map->key_size);
        cur->valuedata = malloc(map->value_size);
        memcpy(cur->keydata,keydata,map->key_size);
        memcpy(cur->valuedata,valuedata,map->value_size);

        map->size++;
    }

}

void * hashmap_find(hashmap * map,void * keydata) {
    int entry = map->hash(map,keydata,map->key_size) % map->bucket_capacity;
    hashmap_node_t * node = &map->bucket[entry];
    hashmap_node_t * cur = node->next, * next;

    while (cur) {
        next = cur->next;
        if(map->compare(cur->keydata,keydata)) {
            return cur->valuedata;
        }
        cur = next;
    }
    return NULL;
}

static void hashmap_node_list_delete(hashmap * map,hashmap_node_t * cur) {
    if(cur == NULL) return;
    else {
        hashmap_node_list_delete(map,cur->next);
        free(cur->keydata);
        map->deallocator(cur->valuedata);
        free(cur);
        return;
    }
}

void hashmap_deconstruct(hashmap * map) {
    for(int i = 0;i < map->bucket_capacity;i++) {
        hashmap_node_list_delete(map,map->bucket[i].next);
    }
    free(map->bucket);
}

void hashmap_traverse(hashmap * map,void (* func)(void *,void *)) {
    for(int i = 0;i < map->bucket_capacity;i++) {
        hashmap_node_t * cur = map->bucket[i].next;
        while (cur) {
            func(cur->keydata,cur->valuedata);
            cur = cur->next;
        }
    }
}




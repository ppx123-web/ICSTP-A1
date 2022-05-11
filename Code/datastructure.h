#ifndef DATASTRUCTURE_H


typedef struct vector {
    int size;
    int capacity;
    void * data;
    int ele_size;
    void (* deallocator)(void *);
}vector;

void vector_init(vector * vec,int ele_size,int capacity,void (* deallocator)(void *));
vector * vector_alloc_init(int ele_size,int capacity,void (* deallocator)(void *));
void vector_resize(vector * vec,int size);
void vector_push_back(vector * vec,void * udata);
void vector_delete(vector * vec);
void * vector_id(vector * vec,int id);
int vector_size(vector *vec);


typedef struct hashmap_node_t {
    union {
        struct {
            void * keydata;
            void * valuedata;
        };
        struct {
            void * first;
            void * second;
        }data;
    };

    struct hashmap_node_t * next;
}hashmap_node_t;

typedef struct hashmap {
    int size;
    int bucket_capacity;
    hashmap_node_t * bucket;
    int key_size;
    int value_size;
    int unit_size;


    int (*hash)(struct hashmap * map,void * keydata,int size);
    int (*compare)(void * ka,void * kb);

    void (*deallocator)(void *);
}hashmap;

void hashmap_init(hashmap * map,int bucket_capacity,int key_size,int value_size,
                int (*hash)(struct hashmap * map,void * keydata,int size),int (*compare)(void * ka,void * kb),
                void (*deallocator)(void *));
void hashmap_delete(hashmap * map,void * keydata);
void hashmap_set(hashmap * map,void * keydata,void * valuedata);
void * hashmap_find(hashmap * map,void * keydata);

void hashmap_deconstruct(hashmap * map);
void hashmap_traverse(hashmap * map,void (* func)(void *,void *));


#endif
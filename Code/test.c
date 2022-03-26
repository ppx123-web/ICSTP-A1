#include "debug.h"
#include "data.h"

#define TEST(A) test_ ## A
#define ASSIGN(A) .A = TEST(A),

static void test_Struct(Node_t * cur,int deep) {
    if (cur == NULL) return;
    if(type(cur,"StructSpecifier")) {
        FieldList * field = semantic_check->Struct(cur);
        type_ops->print_field(field,0);
        return;
    }
    if (cur->lchild != NULL) {
        Node_t * child = cur->lchild;
        do {
            test_Struct(child,deep + 1);
            child = child->right;
        }while (child != NULL);
    }
}

static void test_main() {
    test->display_symbol_stack();
}

static void test_display_symbol_table() {

}

static void test_display_symbol_stack() {
    SymbolStack_ele_t * cur = symbol_stack->first.next;
    while (cur != &symbol_stack->last) {
        for(unit_t * node = cur->head.scope_next;node != &cur->tail;node = node->scope_next) {
            symbol_table->display_node(node);
            printf("\n");
        }
        cur = cur->next;
    }
}



MODULE_DEF(Test_t,test) = {
        ASSIGN(main)
        ASSIGN(Struct)
        ASSIGN(display_symbol_table)
        ASSIGN(display_symbol_stack)
};
#include <stdio.h>
#include "debug.h"
#include "data.h"

int yyrestart(FILE*);
int yyparse (void);
int yylex(void);
void yyerror(char* s);

int syntax = 0;
void yyinit();
static void end_free();
void translate();

int main(int argc,char *argv[]) {
    if (argc <= 1) {
        printf("Usage:%s $FILE\n",argv[0]);
    }
    for (int i = 1;i < argc;i++) {
    #ifndef FINAL
        Log("\n\n\n%s:\n",argv[i]);
    #endif
        yyinit();

        FILE * f = fopen(argv[i], "r");
        if (!f) {
            perror(argv[1]);
            return 1;
        }
        yyrestart(f);
        yyparse();
        fclose(f);
        semantic_check->init();
        if (syntax == 0) {
            //tree->traverse(tree->root,0);
            //semantic_check->main(tree->root);
            translate();
            //test->main();
        }
        end_free();
    }
    return 0;
}


static void free_Traverse(Node_t * cur) {
    if (cur == NULL) return;
    Node_t * child = cur->lchild, * temp;
    while (child != NULL) {
        temp = child->right;
        free_Traverse(child);
        child = temp;
    }
    free(cur);
}

static void end_free() {
#ifndef FINAL
    //yylex bison自己会申请空间，但是不释放，共计3次  16458 bytes
    symbol_stack->pop();//先pop
    for(int i = 0;i < symbol_table->table_size;i++) {
        if(symbol_table->table[i]) {
            free(symbol_table->table[i]);
        }
    }
    free(symbol_table->table);
    free_Traverse(tree->root);
    panic_on("Error",symbol_table->cnt != 0);
#endif
}

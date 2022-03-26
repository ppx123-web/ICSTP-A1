#include <stdio.h>
#include "debug.h"
#include "data.h"

int yyrestart(FILE*);
int yyparse (void);
int yylex(void);
void yyerror(char* s);

int syntax = 0;
extern int yycolumn,yylineno;
static void end_free();


int main(int argc,char *argv[]) {
    if (argc <= 1) {
        printf("Usage:%s $FILE\n",argv[0]);
    }
    for (int i = 1;i < argc;i++) {
    #ifndef FINAL
        printf("\n\n\n");
        printf("%s:\n",argv[i]);
    #endif
        syntax = 0;
        yycolumn = 1;
        yylineno = 1;
        FILE * f = fopen(argv[i], "r");
        if (!f) {
            perror(argv[1]);
            return 1;
        }
        yyrestart(f);
        yyparse();
        fclose(f);
        if (syntax == 0) {
            semantic_check->init();
            tree->traverse(tree->root,0);
            //semantic_check->main(tree->root);

            test->main();
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
    for(int i = 0;i < symbol_table->table_size;i++) {
        if(symbol_table->table[i]) {
            free(symbol_table->table[i]->head.name);
            free(symbol_table->table[i]->tail.name);
            free(symbol_table->table[i]);
        }
    }
    free(symbol_table->table);
    free_Traverse(tree->root);
}

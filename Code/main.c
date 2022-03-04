#include <stdio.h>
#include "debug.h"
#include "data.h"

int yyrestart(FILE*);
int yyparse (void);
int yylex(void);
void yyerror(char* s);

int syntax = 0;
extern int yycolumn,yylineno;

int main(int argc,char *argv[]) {
    if (argc <= 1) {
        printf("Usage:%s $FILE\n",argv[0]);
    }
    for (int i = 1;i < argc;i++) {
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
        if(syntax == 0) {
            tree->traverse(tree->root,0);
        } else {
            printf("Syntax error:%d\n",syntax);
        }
        printf("\n\n\n");
    }
    return 0;
}


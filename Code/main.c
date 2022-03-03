#include <stdio.h>
#include "debug.h"
#include "data.h"

int yyrestart(FILE*);
int yyparse (void);
int yylex(void);
void yyerror(char* s);

int main(int argc,char *argv[]) {
    if (argc <= 1) {
        printf("Usage:%s $FILE\n",argv[0]);
    }
    for (int i = 1;i < argc;i++) {
        FILE * f = fopen(argv[i], "r");
        if (!f) {
            perror(argv[1]);
            return 1;
        }
        yyrestart(f);
        yyparse();
        fclose(f);   
    }
    return 0;
}


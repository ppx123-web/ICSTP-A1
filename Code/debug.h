// #define DEBUG
// #define SCANNER_DEBUG

#ifndef DEBUG_H
#define DEBUG_H



#ifdef SCANNER_DEBUG
    #define ELEMENT(_S_) {printf("TYPE =( %s ),content=( %s )\n",_S_,yytext);yylval.node = add_node_text(_S_,yytext,yyleng);}
#else
    #define ELEMENT(_S_) { yylval.node = add_node_text(_S_,yytext,yyleng);}
#endif


#define _Log(...) fprintf(stderr,__VA_ARGS__);
#define SyntaxError(format, ...) _Log("\33[1;31m row=%d col=%d, " format "\33[0m\n",yylloc.first_line,yycolumn, ## __VA_ARGS__)
#define LexicalError(format, ...) _Log("\33[1;32m row=%d col=%d, " format "\33[0m\n",yylloc.first_line,yycolumn, ## __VA_ARGS__)

#define Log(format, ...) \
    _Log("\33[1;34m[%s,%d,%s] " format "\33[0m\n", \
        __FILE__, __LINE__, __func__, ## __VA_ARGS__)

#define PRINT_TREE(A) tree->traverse(A,0);

#endif
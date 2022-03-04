// #define DEBUG
// #define SCANNER_DEBUG
// #define FINAL


#ifndef DEBUG_H
#define DEBUG_H


#ifdef SCANNER_DEBUG
    #define ELEMENT(_S_) {printf("TYPE =( %s ),content=( %s )\n",_S_,yytext);yylval.node = add_node_text(_S_,yytext,yyleng);}
#else
    #define ELEMENT(_S_) { yylval.node = add_node_text(_S_,yytext,yyleng);strcat(linetext,yytext);}
#endif

#ifdef FINAL
    #define _Log(...) fprintf(stderr,__VA_ARGS__);
    #define SyntaxError(format, ...) _Log("Error type B at Line %d: Syntax error. \'%s\'\n", yylineno, yytext)
    #define LexicalError(format, ...) _Log("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext)
#else
    #define _Log(...) fprintf(stderr,__VA_ARGS__);
    #define SyntaxError(format, ...) _Log("\33[1;31m row=%d col=%d, " format "\33[0m\n",yylloc.first_line,yycolumn, ## __VA_ARGS__)
    #define LexicalError(format, ...) _Log("\33[1;32m row=%d col=%d, " format "\33[0m\n",yylloc.first_line,yycolumn, ## __VA_ARGS__)
#endif

#define Log(format, ...) \
    _Log("\33[1;34m[%s,%d,%s] " format "\33[0m\n", \
        __FILE__, __LINE__, __func__, ## __VA_ARGS__)

#define PRINT_TREE(A) tree->traverse(A,0);

#ifdef FINAL

#endif

#endif
// #define DEBUG
// #define SCANNER_DEBUG
// #define FINAL
// #define TREEDEBUG


#ifndef DEBUG_H
#define DEBUG_H

//通用的Log
#define Log(format, ...) \
    _Log("\33[1;34m[%s,%d,%s] " format "\33[0m\n", \
        __FILE__, __LINE__, __func__, ## __VA_ARGS__)

#define _Log(...) \
    fprintf(stderr,## __VA_ARGS__)

//用于检测词法分析
#ifdef SCANNER_DEBUG
    #define ELEMENT(_S_) {printf("TYPE =( %s ),content=( %s )\n",_S_,yytext);yylval.node = add_node_text(_S_,yytext,yyleng);}
#else
    #define ELEMENT(_S_) { yylval.node = add_node_text(_S_,yytext,yyleng);strcat(linetext,yytext);}
#endif

//用于最后的输出
#ifdef FINAL
    #define SyntaxError(format, ...) printf("Error type B at Line %d: Syntax error. \'%s\'\n", yylineno, yytext)
    #define LexicalError(format, ...) printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext)
#else
    #define SyntaxError(format, ...) _Log("\33[1;31m row=%d col=%d, Syntax Error: " format "\33[0m\n",yylloc.first_line,yycolumn, ## __VA_ARGS__)
    #define LexicalError(format, ...) _Log("\33[1;32m row=%d col=%d, Lexical Error: " format "\33[0m\n",yylloc.first_line,yycolumn, ## __VA_ARGS__)
#endif

//用于打印树的信息
#ifdef TREEDEBUG
    #define Treedebug(format, ...) _Log(format,## __VA_ARGS__)
#else
    #define Treedebug(format, ...) {;}
#endif


#endif
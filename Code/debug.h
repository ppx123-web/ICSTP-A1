// #define DEBUG
// #define SCANNER_DEBUG

#ifdef SCANNER_DEBUG
    #define ELEMENT(_S_) printf("TYPE =( %s ),content=( %s )\n",_S_,yytext)
    #define DEBUGRE(S) {;}
#else
    #define ELEMENT(_S_) {;}
    #define DEBUGRE(S) return S;
#endif

#ifdef DEBUG
    #define _Log(...) fprintf(stderr,__VA_ARGS__);
    #define Log(format, ...) _Log("\33[1;34m " format "\33[0m\n", ## __VA_ARGS__)
#else 
    #define Log(format, ...) {;}
#endif
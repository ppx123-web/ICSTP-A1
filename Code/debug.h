#define ELEMENT(_S_) printf("TYPE =( %s ),content=( %s )\n",_S_,yytext)
#define _Log(...) printf(__VA_ARGS__);
#define Log(format, ...) _Log("\33[1;34m " format "\33[0m\n", ## __VA_ARGS__)
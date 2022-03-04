%{
    #include <stdio.h>
    #include "debug.h"
    #include "syntax.tab.h"
    #include "data.h"
    #include <string.h>

    static Node_t* add_node_text(char * content,char *text,int len);
%}

INT (([+-]?[0-9])|([+-]?[1-9][0-9]+)|(0[xX][0-9A-Fa-f]+)|(0[1-7][0-7]*))[^.]
FLOAT   [+-]?([0-9]*(\.)[0-9]+([eE][+-]?[0-9]+)?|[0-9]+(\.)[0-9]*([eE][+-]?[0-9]+)?)
ID  [A-Za-z_]+[0-9_A-za-z]*
SEMI    ;
COMMA	,
ASSIGNOP	=
RELOP	>|<|>=|<=|==|!=
PLUS	\+
MINUS	-
STAR	\*
DIV \/
AND	&&
OR	\|\|
DOT	\.
NOT	!
TYPE int|float
LP	\(
RP	\)
LB	\[
RB	\]
LC  \{
RC	\}
STRUCT	struct
RETURN	return
IF	if
ELSE	else
WHILE	while

%%
{STRUCT}  { ELEMENT("STRUCT"); return STRUCT; }
{RETURN}  { ELEMENT("RETURN"); return RETURN; }
{IF}      { ELEMENT("IF"); return IF; }
{ELSE}    { ELEMENT("ELSE"); return ELSE; }
{WHILE}   { ELEMENT("WHILE"); return WHILE; }
{TYPE}    { ELEMENT("TYPE"); return TYPE; }
{INT}     { unput(yytext[yyleng-1]); yycolumn--,yytext[yyleng-1] = 0;ELEMENT("INT"); return INT;}
{FLOAT}   { ELEMENT("FLOAT"); return FLOAT;}
{ID}      { ELEMENT("ID"); return ID; }
{SEMI}	  { ELEMENT("SEMI"); return SEMI;}
{COMMA}   { ELEMENT("COMMA"); return COMMA;}
{ASSIGNOP}	{ ELEMENT("ASSIGNOP"); return ASSIGNOP;}
{PLUS}	    { ELEMENT("PLUS"); return PLUS;}
{MINUS}	    { ELEMENT("MINUS"); return MINUS;}
{STAR}	    { ELEMENT("STAR"); return STAR;}
{DIV}	    { ELEMENT("DIV"); return DIV;}
{AND}	    { ELEMENT("AND"); return AND;}
{OR}  	    { ELEMENT("OR"); return OR;}
{DOT}	    { ELEMENT("DOT"); return DOT;}
{NOT} 	    { ELEMENT("NOT"); return NOT;}
{LP}	    { ELEMENT("LP"); return LP;}
{RP}	    { ELEMENT("RP"); return RP;}
{LB}	    { ELEMENT("LB"); return LB;}
{RB}	    { ELEMENT("RB"); return RB;}
{LC}	    { ELEMENT("LC"); return LC;}
{RC}	    { ELEMENT("RC"); return RC;}
[ ]         { ;}
[\t]        { yycolumn += 3; }
[\n]        { yycolumn = 1; yylineno += 1; }
.           { LexicalError("Lexical Error: %s",yytext);     /*printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext);*/ }

%%

extern int syntax;

void yyerror(char* msg) {
    syntax++;
    SyntaxError("%s : \n\t%s\n\t^", msg,yytext);
}

static Node_t* add_node_text(char * content,char *text,int len) {
    Node_t * cur = tree->Node_alloc(content,yylloc.first_line);
    strncpy(cur->text,text,len);
    // printf("%s %s\n",content,text);
    return cur;
}



// #ifdef SCANNER_DEBUG
// int main(int argc,char* argv[]) {
//     if(argc > 1) {
//         if(!(yyin = fopen(argv[1],"r"))) {
//             perror(argv[1]);
//             return 1;
//         }
//     }
//     while (yylex() != 0);
//     return 0;
// }
// #endif

%{
    #include <stdio.h>
    #include "debug.h"
    #include "syntax.tab.h"

    struct YYLTYPE {
        int first_line;
        int first_column;
        int last_line;
        int last_column;
    };

    struct YYLTYPE yylloc = {0};

    int yycolumn = 1;
    #define YY_USER_ACTION \
                yylloc.first_line = yylloc.last_line = yylineno; \
                yylloc.first_column = yycolumn; \
                yylloc.last_column = yycolumn + yyleng - 1; \
                yycolumn += yyleng;
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
{INT}     { unput(yytext[yyleng-1]); yycolumn--,yytext[yyleng-1] = 0; Log("TYPE = INT:( %s )",yytext); return INT;}
{FLOAT}   { Log("TYPE = FLOAT: %s",yytext); return FLOAT;}
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
[ \t]       { ;}
[\n]        { yycolumn += 1; }
.           { printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext); }

%%

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

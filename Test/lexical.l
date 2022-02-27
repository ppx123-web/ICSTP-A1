%{
    #include <stdio.h>
    #include "../Code/debug.h"

%}

INT (([+-]?[0-9])|([+-]?[1-9][0-9]+)|(0[xX][0-9A-Fa-f]+)|(0[0-9]+))[^.]
FLOAT   [+-]?([0-9]+(\.)?[0-9]*([eE][+-]?[0-9]+)?|[0-9]+\.)
ID  [A-Za-z_]+[0-9_A-za-z]*
SEMI    ;
COMMA	,
ASSIGNOP	=
RELOP	>|<|>=|<=|==|!=
STAR	\*
DIV \/
AND	&&
OR	\|\|
DOT	[.]
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
struct  { ELEMENT("STRUCT");  }
return  { ELEMENT("RETURN");  }
if      { ELEMENT("IF");  }
else    { ELEMENT("ELSE");  }
while   { ELEMENT("WHILE");  }
int|float    { ELEMENT("TYPE");  }
{FLOAT}   { Log("TYPE = FLOAT:( %s )",yytext); }
{INT}     { unput(yytext[yyleng-1]); Log("TYPE = INT:( %d )",atoi(yytext));  }
{ID}      { ELEMENT("ID");  }
{SEMI}	  { ELEMENT("SEMI"); }
{COMMA}   { ELEMENT("COMMA"); }
{ASSIGNOP}	{ ELEMENT("ASSIGNOP"); }
"+"	        { ELEMENT("PLUS"); }
"-"	        { ELEMENT("MINUS"); }
"*"	    { ELEMENT("STAR"); }
"/"	    { ELEMENT("DIV"); }
"&&"	        { ELEMENT("AND"); }
\|\|  	    { ELEMENT("OR"); }
"."	        { ELEMENT("DOT"); }
{NOT} 	    { ELEMENT("NOT"); }
{LP}	    { ELEMENT("LP"); }
{RP}	    { ELEMENT("RP"); }
{LB}	    { ELEMENT("LB"); }
{RB}	    { ELEMENT("RB"); }
{LC}	    { ELEMENT("LC"); }
{RC}	    { ELEMENT("RC"); }
[ \t]         { ;}
[\n]        { ; }
.           { printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext); }

%%

#ifdef SCANNER_DEBUG
int main(int argc,char* argv[]) {
    if(argc > 1) {
        if(!(yyin = fopen(argv[1],"r"))) {
            perror(argv[1]);
            return 1;
        }
    }
    while (yylex() != 0);
    return 0;
}
#endif
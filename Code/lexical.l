%{
    #include <stdio.h>
    #include <debug.h>

%}

INT [+-]?[0-9]+|0x[0-9A-Fa-f]+
FLOAT   [+-]?[0-9]*\.[0-9]+e?[+-]?[0-9]+
ID  [A-Za-z]+[0-9_A-za-z]*
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
{STRUCT}  { ELEMENT("STRUCT"); }
{RETURN}  { ELEMENT("RETURN"); }
{IF}      { ELEMENT("IF"); }
{ELSE}    { ELEMENT("ELSE"); }
{WHILE}   { ELEMENT("WHILE"); }
{TYPE}    { ELEMENT("TYPE"); }
{INT}     { Log("TYPE = INT: %d",atoi(yytext)); }
{FLOAT}   { Log("TYPE = FLOAT: %s",yytext); }
{ID}      { ELEMENT("ID"); }
{SEMI}	  { ELEMENT("SEMI");}
{COMMA}   { ELEMENT("COMMA");}
{ASSIGNOP}	{ ELEMENT("ASSIGNOP");}
{PLUS}	    { ELEMENT("PLUS");}
{MINUS}	    { ELEMENT("MINUS");}
{STAR}	    { ELEMENT("STAR");}
{DIV}	    { ELEMENT("DIV");}
{AND}	    { ELEMENT("AND");}
{OR}  	    { ELEMENT("OR");}
{DOT}	    { ELEMENT("DOT");}
{NOT} 	    { ELEMENT("NOT");}
{LP}	    { ELEMENT("LP");}
{RP}	    { ELEMENT("RP");}
{LB}	    { ELEMENT("LB");}
{RB}	    { ELEMENT("RB");}
{LC}	    { ELEMENT("LC");}
{RC}	    { ELEMENT("RC");}
[ \t\n]         { ;}
.           { printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext); }

%%

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
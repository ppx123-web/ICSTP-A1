%{
    #include <stdio.h>
    #include "debug.h"
    #include "syntax.tab.h"
    #include "data.h"
    #include <string.h>

    int Iscomments = 0;
    extern int syntax;
    int last_lineno = 0,cur_lineno = 0;
    static Node_t* add_node_text(char * content,char *text,int len);
%}

FLOAT   ([0-9]+(\.)[0-9]+)|([0-9]*(\.)[0-9]+([eE][+-]?[0-9]+)|[0-9]+(\.)[0-9]*([eE][+-]?[0-9]+))
INT (([0-9])|([1-9][0-9]+)|(0[xX][0-9A-Fa-f]+)|(0[0-7]+))
ID  [A-Za-z_]+[0-9A-Za-z_]*
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
LINECOMMENT "//"
LCOMMENTS   "/*"   

%%
{STRUCT}    { ELEMENT("STRUCT"); return STRUCT; }
{RETURN}    { ELEMENT("RETURN"); return RETURN; }
{IF}        { ELEMENT("IF"); return IF; }
{ELSE}      { ELEMENT("ELSE"); return ELSE; }
{WHILE}     { ELEMENT("WHILE"); return WHILE; }
{TYPE}      { ELEMENT("TYPE"); return TYPE; }
{INT}       { ELEMENT("INT"); return INT;}
{FLOAT}     { ELEMENT("FLOAT"); return FLOAT;}
{ID}        { ELEMENT("ID"); return ID; }
{SEMI}	    { ELEMENT("SEMI"); return SEMI;}
{COMMA}     { ELEMENT("COMMA"); return COMMA;}
{ASSIGNOP}	{ ELEMENT("ASSIGNOP"); return ASSIGNOP;}
{RELOP}     { ELEMENT("RELOP"); return RELOP;}
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
[ ]         { strcat(linetext," ");}
[\t]        { yycolumn += 3; strcat(linetext,"\t");}
[\n\r]      { yycolumn = 1; yylineno += 1; memset(linetext,0,sizeof(linetext)); }
{LINECOMMENT} { char c = input(); while(c != '\n') { c = input(); } yylineno += 1; }
{LCOMMENTS} {   
                char c1 = input(),c2 = input();
                if (c1 == '\n') {
                    yylineno += 1;
                    yycolumn = 1;
                } else {
                    yycolumn += 1;
                }
                while (c1 != '*' || c2 != '/') {
                    if (c2 == '\n') {
                        yylineno += 1;
                        yycolumn = 1;
                    } else {
                        yycolumn += 1;
                    }
                    c1 = c2;
                    c2 = input();
                }
            }
.           { LexicalError("%s",yytext); syntax++; }

%%

extern int syntax;

void yyerror(char* msg) {
    syntax++;
    if(strcmp(msg,"syntax error") == 0) {
        SyntaxError("%s : %s\n\t%s\n",msg,yytext,linetext);
    } else {
        //SyntaxError("(from syntax.y): %s\n",msg);
    }
}

static Node_t* add_node_text(char * content,char *text,int len) {
    Node_t * cur = tree->Node_alloc(content,yylloc.first_line);
    strncpy(cur->text,text,len);
    Treedebug("\t\t%s %s\n",content,text);
    if(strcmp(content,"INT") == 0) {
        cur->syn = &Int_Type;
    }
    if(strcmp(content,"FLOAT") == 0) {
        cur->syn = &Float_Type;
    }
    return cur;
}



%locations

%{
    int yycolumn = 1;
    char linetext[128];
    #define YY_USER_ACTION \
                yylloc.first_line = yylloc.last_line = yylineno; \
                yylloc.first_column = yycolumn; \
                yylloc.last_column = yycolumn + yyleng - 1; \
                yycolumn += yyleng;
%}

%{
    #include <stdio.h>
    #include "lex.yy.c"
    #include "data.h"
    #include <stdarg.h>

    #define TERMINALS(A,B) A=tree->Node_alloc(B,yylloc.first_line)

    void yyerror(char* s);
    Node_t * Operator(Node_t * cur,char * content,int line,int argc,...);

    extern int syntax;
                
%}

/* declared types */
%union {
    struct __Tree_node_t * node;
}

/* declared tokens */
%token <node> INT
%token <node> FLOAT
%token <node> ID
%token <node> SEMI
%token <node> COMMA
%token <node> ASSIGNOP
%token <node> RELOP
%token <node> PLUS
%token <node> MINUS
%token <node> STAR
%token <node> DIV
%token <node> AND
%token <node> OR
%token <node> DOT
%token <node> NOT
%token <node> TYPE
/* TYPE -> int | float见下面 */
%token <node> LP
%token <node> RP
%token <node> LB
%token <node> RB
%token <node> LC
%token <node> RC
%token <node> STRUCT
%token <node> RETURN
%token <node> IF
%token <node> ELSE
%token <node> WHILE
%token <node> error 

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS
/* %left MINUS  取负在前，left可以考虑不写（推断） */
%left STAR
%left DIV
%right NOT
%right MINUS
%left DOT LB RB LP RP


%type <node> Program
%type <node> ExtDefList
%type <node> ExtDef
%type <node> ExtDecList
%type <node> Specifier
%type <node> StructSpecifier
%type <node> OptTag
%type <node> Tag
%type <node> VarDec
%type <node> FunDec
%type <node> VarList
%type <node> ParamDec
%type <node> CompSt
%type <node> StmtList
%type <node> Stmt
%type <node> DefList
%type <node> Def
%type <node> DecList
%type <node> Dec
%type <node> Exp
%type <node> Args


%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE



%%
/*High-level Definitions */

Program : ExtDefList                { tree->root = $$ = Operator($$,"Program",@$.first_line,1,$1); }
    ;
ExtDefList : ExtDef ExtDefList      { $$ = Operator($$,"ExtDefList",@$.first_line,2,$1,$2); }
    |                               { $$ = NULL; }
    ;
ExtDef : Specifier ExtDecList SEMI  { $$ = Operator($$,"ExtDef",@$.first_line,3,$1,$2,$3); }
    | Specifier SEMI                { $$ = Operator($$,"ExtDef",@$.first_line,2,$1,$2); }
    | Specifier FunDec CompSt       { $$ = Operator($$,"ExtDef",@$.first_line,3,$1,$2,$3); }
    ;
ExtDecList : VarDec                 { $$ = Operator($$,"ExtDecList",@$.first_line,1,$1); }
    | VarDec COMMA ExtDecList       { $$ = Operator($$,"ExtDecList",@$.first_line,3,$1,$2,$3); }
    ;

/*Specifier */

Specifier : TYPE                    { $$ = Operator($$,"Specifier",@$.first_line,1,$1); }
    | StructSpecifier               { $$ = Operator($$,"Specifier",@$.first_line,1,$1); }
    ;
StructSpecifier : STRUCT OptTag LC DefList RC   { $$ = Operator($$,"StructSpecifier",@$.first_line,5,$1,$2,$3,$4,$5); }
    | STRUCT Tag                    { $$ = Operator($$,"StructSpecifier",@$.first_line,2,$1,$2); }
    ;
OptTag : ID                         { $$ = Operator($$,"OptTag",@$.first_line,1,$1); }
    |                               { $$ = NULL; }
    ;
Tag : ID                            { $$ = Operator($$,"Tag",@$.first_line,1,$1); }

/*Declarators */

VarDec : ID                         { $$ = Operator($$,"VarDec",@$.first_line,1,$1); }
    | VarDec LB INT RB              { $$ = Operator($$,"VarDec",@$.first_line,4,$1,$2,$3,$4); }
    ;
FunDec : ID LP VarList RP           { $$ = Operator($$,"FunDec",@$.first_line,4,$1,$2,$3,$4); }
    | ID LP RP                      { $$ = Operator($$,"FunDec",@$.first_line,3,$1,$2,$3); }
    ;
VarList : ParamDec COMMA VarList    { $$ = Operator($$,"VarList",@$.first_line,3,$1,$2,$3); }
    | ParamDec                      { $$ = Operator($$,"ParamDec",@$.first_line,1,$1); }
    ;
ParamDec : Specifier VarDec         { $$ = Operator($$,"ParamDec",@$.first_line,2,$1,$2); }
    ;

/*Statements */

CompSt : LC DefList StmtList RC     { $$ = Operator($$,"CompSt",@$.first_line,4,$1,$2,$3,$4); }
    | error RC                      { $$ = Operator($$,"CompSt",@$.first_line,2,$1,$2);/* error */ }
    ;
StmtList : Stmt StmtList            { $$ = Operator($$,"StmtList",@$.first_line,2,$1,$2); }
    |                               { $$ = NULL; }
    ;
Stmt : Exp SEMI                     { $$ = Operator($$,"Stmt",@$.first_line,2,$1,$2); }
    | CompSt                        { $$ = Operator($$,"Stmt",@$.first_line,1,$1); }
    | RETURN Exp SEMI               { $$ = Operator($$,"Stmt",@$.first_line,3,$1,$2,$3); }
    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE   { $$ = Operator($$,"Stmt",@$.first_line,5,$1,$2,$3,$4,$5); }
    | IF LP Exp RP Stmt ELSE Stmt   { $$ = Operator($$,"Stmt",@$.first_line,7,$1,$2,$3,$4,$5,$6,$7); }
    | WHILE LP Exp RP Stmt          { $$ = Operator($$,"Stmt",@$.first_line,5,$1,$2,$3,$3,$4); }
    | error SEMI                    { $$ = Operator($$,"Stmt",@$.first_line,2,$1,$2);/* error */ }
    ;

/*Local Definitions */

DefList : Def DefList               { $$ = Operator($$,"DefList",@$.first_line,2,$1,$2); }
    |                               { $$ = NULL; }
    ;
Def : Specifier DecList SEMI        { $$ = Operator($$,"Def",@$.first_line,3,$1,$2,$3); }
    ;
DecList : Dec                       { $$ = Operator($$,"DecList",@$.first_line,1,$1); }
    | Dec COMMA DecList             { $$ = Operator($$,"DecList",@$.first_line,3,$1,$2,$3); }
    ;
Dec : VarDec                        { $$ = Operator($$,"Dec",@$.first_line,1,$1); }
    | VarDec ASSIGNOP Exp           { $$ = Operator($$,"Dec",@$.first_line,3,$1,$2,$3); }
    ;

/*Expressions */

Exp : Exp ASSIGNOP Exp              { $$ = Operator($$,"Exp",@$.first_line,3,$1,$2,$3); }
    | Exp AND Exp                   { $$ = Operator($$,"Exp",@$.first_line,3,$1,$2,$3); }
    | Exp OR Exp                    { $$ = Operator($$,"Exp",@$.first_line,3,$1,$2,$3); }
    | Exp RELOP Exp                 { $$ = Operator($$,"Exp",@$.first_line,3,$1,$2,$3); }
    | Exp PLUS Exp                  { $$ = Operator($$,"Exp",@$.first_line,3,$1,$2,$3); }
    | Exp MINUS Exp                 { $$ = Operator($$,"Exp",@$.first_line,3,$1,$2,$3); }
    | Exp STAR Exp                  { $$ = Operator($$,"Exp",@$.first_line,3,$1,$2,$3); }
    | Exp DIV Exp                   { $$ = Operator($$,"Exp",@$.first_line,3,$1,$2,$3); }
    | LP Exp RP                     { $$ = Operator($$,"Exp",@$.first_line,3,$1,$2,$3); }
    | MINUS Exp                     { $$ = Operator($$,"Exp",@$.first_line,2,$1,$2); }
    | NOT Exp                       { $$ = Operator($$,"Exp",@$.first_line,2,$1,$2); }
    | ID LP Args RP                 { $$ = Operator($$,"Exp",@$.first_line,4,$1,$1,$3,$4); }
    | ID LP RP                      { $$ = Operator($$,"Exp",@$.first_line,3,$1,$2,$3); }
    | Exp LB Exp RB                 { $$ = Operator($$,"Exp",@$.first_line,4,$1,$2,$3,$4); }
    | Exp DOT ID                    { $$ = Operator($$,"Exp",@$.first_line,3,$1,$2,$3); }
    | ID                            { $$ = Operator($$,"Exp",@$.first_line,1,$1); }
    | INT                           { $$ = Operator($$,"Exp",@$.first_line,1,$1); }
    | FLOAT                         { $$ = Operator($$,"Exp",@$.first_line,1,$1); }
    | Exp PLUS error                { $$ = NULL; }
    ;
Args : Exp COMMA Args               { $$ = Operator($$,"Args",@$.first_line,3,$1,$2,$3); }
    | Exp                           { $$ = Operator($$,"Args",@$.first_line,1,$1); }
    | error Args                    { $$ = Operator($$,"Args",@$.first_line,2,$1,$2);/* error */ }
    ;

%%


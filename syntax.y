%{
    #include "lex.yy.c"
    #include "node.hpp"

    #define YYSTYPE Node *

    void yyerror(const char*);
%}
%locations

%token INT FLOAT CHAR
%token ID TYPE
%token STRUCT
%token IF ELSE
%token WHILE FOR
%token BREAK CONTINUE
%token RETURN
%token DOT SEMI COMMA
%token ASSIGN
%token AR
%token LT LE GT GE NE EQ
%token PLUS MINUS MUL DIV AND OR NOT
%token LP LB LC
%token RP RB RC

%%
Program:
      ExtDefList                    {}
    ;
ExtDefList:
      /*empty*/                     {}
    | ExtDef ExtDefList             {}
    ;
ExtDef:
      Specifier ExtDecList SEMI     {}
    | Specifier SEMI                {}
    | Specifier FunDec CompSt       {}
    ;
ExtDecList:
      VarDec                        {}
    | VarDec COMMA ExtDecList       {}
    ;

/* specifier */
Specifier:
      TYPE                          {}
    | StructSpecifier               {}
    ;
StructSpecifier:
      STRUCT ID LC DefList RC       {}
    | STRUCT ID                     {}
    ;
/* declarator */
VarDec:
      ID                            {}
    | VarDec LB INT RB              {}
    ;
FunDec:
      ID LP VarList RP              {}
    | ID LP RP                      {}
    ;
VarList:
      ParamDec COMMA VarList        {}
    | ParamDec                      {}
    ;
ParamDec:
      Specifier VarDec              {}
    ;

/* statement */
CompSt:
      LC DefList StmtList RC        {}
    ;
StmtList:
      /*empty*/                     {}
    | Stmt StmtList                 {}
    ;
Stmt:
      Exp SEMI                      {}
    | CompSt                        {}
    | RETURN Exp SEMI               {}
    | IF LP Exp RP Stmt             {}
    | IF LP Exp RP Stmt ELSE Stmt   {}
    | WHILE LP Exp RP Stmt          {}
    ;

/* local definition */
DefList:
      /*empty*/                     {}
    | Def DefList                   {}
    ;
Def:
    Specifier DecList SEMI          {}
    ;
DecList:
      Dec                           {}
    | Dec COMMA DecList             {}
    ;
Dec:
      VarDec                        {}
    | VarDec ASSIGN Exp             {}
    ;

/* Expression */
Exp:
      Exp ASSIGN Exp                {}
    | Exp AND Exp                   {}
    | Exp OR Exp                    {}
    | Exp LT Exp                    {}
    | Exp LE Exp                    {}
    | Exp GT Exp                    {}
    | Exp GE Exp                    {}
    | Exp NE Exp                    {}
    | Exp EQ Exp                    {}
    | Exp PLUS Exp                  {}
    | Exp MINUS Exp                 {}
    | Exp MUL Exp                   {}
    | Exp DIV Exp                   {}
    | LP Exp RP                     {}
    | MINUS Exp                     {}
    | NOT Exp                       {}
    | ID LP Args RP                 {}
    | ID LP RP                      {}
    | Exp LB Exp RB                 {}
    | Exp DOT ID                    {}
    | ID                            {}
    | INT                           {}
    | FLOAT                         {}
    | CHAR                          {}
    ;
Args:
      Exp COMMA Args                {}
    | Exp                           {}
    ;
%%
void yyerror(const char *s) {
    fprintf(stderr, "%s\n", s);
}
int main() {
    yyparse();
}

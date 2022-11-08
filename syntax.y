%{
    #include "lex.yy.c"
    #include "compiler/node.h"

    #define YYINITDEPTH 40960
    #define YYSTACK_ALLOC

    void yyerror(const char*);

    extern Node* root;
    int errCount = 0;
%}
%locations

%token INT FLOAT CHAR STRING
%token ID TYPE ERR POINTER
%token STRUCT
%token IF ELSE
%token WHILE FOR
%token BREAK CONTINUE
%token RETURN
%token DOT SEMI COMMA
%token ASSIGN
%token AR
%token LT LE GT GE NE EQ
%token DEREF INCREASE DECREASE
%token PLUS MINUS MUL DIV AND OR NOT
%token LP LB LC
%token RP RB RC

%right ASSIGN
%left DEREF
%left INCREASE DECREASE
%left OR
%left AND
%left EQ NE
%left LT LE GT GE
%left PLUS MINUS
%left MUL DIV
%right NOT AR
%left LP RP LB RB DOT

%nonassoc LOWER_ELSE
%nonassoc ELSE

%%
Program:
      ExtDefList                            { $$ = Node::createNodeWithChildren("Program", @$.first_line, DataType::PROD, {$1}); root = $$; }
    ;       
ExtDefList:
      /*empty*/                             { $$ = Node::createNodeWithChildren("ExtDefList", @$.first_line, DataType::PROD, {}); }
    | ExtDef ExtDefList                     { $$ = Node::createNodeWithChildren("ExtDefList", @$.first_line, DataType::PROD, {$1, $2}); }
    ;       
ExtDef:
      Specifier ExtDecList SEMI             { $$ = Node::createNodeWithChildren("ExtDef", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | Specifier SEMI                        { $$ = Node::createNodeWithChildren("ExtDef", @$.first_line, DataType::PROD, {$1, $2}); }
    | Specifier FunDec CompSt               { $$ = Node::createNodeWithChildren("ExtDef", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | Specifier ExtDecList error            { outputFile << "Error type B at Line " << @$.first_line << ": Missing semicolon ';'" << std::endl; errCount++; }
    | Specifier error                       { outputFile << "Error type B at Line " << @$.first_line << ": Missing semicolon ';'" << std::endl; errCount++; }
    ;
ExtDecList:
      VarDec                                { $$ = Node::createNodeWithChildren("ExtDecList", @$.first_line, DataType::PROD, {$1}); }
    | VarDec COMMA ExtDecList               { $$ = Node::createNodeWithChildren("ExtDecList", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    ;

/* specifier */       
Specifier:
      TYPE                                  { $$ = Node::createNodeWithChildren("Specifier", @$.first_line, DataType::PROD, {$1}); }
    | StructSpecifier                       { $$ = Node::createNodeWithChildren("Specifier", @$.first_line, DataType::PROD, {$1}); }
    | Specifier MUL                         { $$ = Node::createNodeWithChildren("Specifier", @$.first_line, DataType::PROD, {$1, $2}); }
    ;
StructSpecifier:
      STRUCT ID LC DefList RC               { $$ = Node::createNodeWithChildren("StructSpecifier", @$.first_line, DataType::PROD, {$1, $2, $3, $4, $5}); }
    | STRUCT ID                             { $$ = Node::createNodeWithChildren("StructSpecifier", @$.first_line, DataType::PROD, {$1, $2}); }
    ;
/* declarator */        
VarDec:
      ID                                    { $$ = Node::createNodeWithChildren("VarDec", @$.first_line, DataType::PROD, {$1}); }
    | VarDec LB INT RB                      { $$ = Node::createNodeWithChildren("VarDec", @$.first_line, DataType::PROD, {$1, $2, $3, $4}); }
    | VarDec LB INT error                   { outputFile << "Error type B at Line " << @$.first_line << ": Missing closing brackets ']'" << std::endl; errCount++; }
    | ERR                                   { outputFile << "Error type B at Line " << @$.first_line << ": bad identifier\n" << std::endl; errCount++; }
    ;
FunDec:
      ID LP VarList RP                      { $$ = Node::createNodeWithChildren("FunDec", @$.first_line, DataType::PROD, {$1, $2, $3, $4}); }
    | ID LP RP                              { $$ = Node::createNodeWithChildren("FunDec", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | ID LP error                           { outputFile << "Error type B at Line " << @$.first_line << ": Missing closing parenthesis ')'" << std::endl; errCount++; }
    | ID LP VarList error                   { outputFile << "Error type B at Line " << @$.first_line << ": Missing closing parenthesis ')'" << std::endl; errCount++; }
    ;
VarList:        
      ParamDec COMMA VarList                { $$ = Node::createNodeWithChildren("VarList", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | ParamDec                              { $$ = Node::createNodeWithChildren("VarList", @$.first_line, DataType::PROD, {$1}); }
    ;
ParamDec:       
      Specifier VarDec                      { $$ = Node::createNodeWithChildren("ParamDec", @$.first_line, DataType::PROD, {$1, $2}); }
    ;

/* statement */
CompSt:
      LC StmtList RC                        { $$ = Node::createNodeWithChildren("CompSt", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    ;
StmtList:
      /*empty*/                             { $$ = Node::createNodeWithChildren("StmtList", @$.first_line, DataType::PROD, {}); }
    | Def StmtList                          { $$ = Node::createNodeWithChildren("StmtList", @$.first_line, DataType::PROD, {$1, $2}); }
    | Stmt StmtList                         { $$ = Node::createNodeWithChildren("StmtList", @$.first_line, DataType::PROD, {$1, $2}); }
    ;
Stmt:
      Exp SEMI                              { $$ = Node::createNodeWithChildren("Stmt", @$.first_line, DataType::PROD, {$1, $2}); }
    | CompSt                                { $$ = Node::createNodeWithChildren("Stmt", @$.first_line, DataType::PROD, {$1}); }
    | RETURN Exp SEMI                       { $$ = Node::createNodeWithChildren("Stmt", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | IF LP Exp RP Stmt %prec LOWER_ELSE    { $$ = Node::createNodeWithChildren("Stmt", @$.first_line, DataType::PROD, {$1, $2, $3, $4, $5}); }
    | IF LP Exp RP Stmt ELSE Stmt           { $$ = Node::createNodeWithChildren("Stmt", @$.first_line, DataType::PROD, {$1, $2, $3, $4, $5, $6, $7}); }
    | WHILE LP Exp RP Stmt                  { $$ = Node::createNodeWithChildren("Stmt", @$.first_line, DataType::PROD, {$1, $2, $3, $4, $5}); }
    | FOR LP DefOrExp SEMI Exp SEMI MultiExp RP Stmt  { $$ = Node::createNodeWithChildren("Stmt", @$.first_line, DataType::PROD, {$1, $2, $3, $4, $5, $6, $7, $8, $9}); }
    | Exp error                             { outputFile << "Error type B at Line " << @$.first_line << ": Missing semicolon ';'" << std::endl; errCount++; }
    | RETURN Exp error                      { outputFile << "Error type B at Line " << @$.first_line << ": Missing semicolon ';'" << std::endl; errCount++; }
    | WHILE LP Exp error Stmt               { outputFile << "Error type B at Line " << @$.first_line << ": Missing closing parenthesis ')'" << std::endl; errCount++; }
    | IF LP Exp error Stmt %prec LOWER_ELSE { outputFile << "Error type B at Line " << @$.first_line << ": Missing closing parenthesis ')'" << std::endl; errCount++; }
    | IF LP Exp error Stmt ELSE Stmt        { outputFile << "Error type B at Line " << @$.first_line << ": Missing closing parenthesis ')'" << std::endl; errCount++; }
    ;

/* local definition */
DefList:
      /*empty*/                             { $$ = Node::createNodeWithChildren("DefList", @$.first_line, DataType::PROD, {}); }
    | Def DefList                           { $$ = Node::createNodeWithChildren("DefList", @$.first_line, DataType::PROD, {$1, $2}); }
    ;
Def:
      Specifier DecList SEMI                { $$ = Node::createNodeWithChildren("Def", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | Specifier DecList error               { outputFile << "Error type B at Line " << @$.first_line << ": Missing semicolon ';'" << std::endl; errCount++; }
    | error DecList SEMI                    { outputFile << "Error type B at Line " << @$.first_line << ": Missing specifier" << std::endl; errCount++; }
    ;
DecList:
      Dec                                   { $$ = Node::createNodeWithChildren("DecList", @$.first_line, DataType::PROD, {$1}); }
    | Dec COMMA DecList                     { $$ = Node::createNodeWithChildren("DecList", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    ;
Dec:
      VarDec                                { $$ = Node::createNodeWithChildren("Dec", @$.first_line, DataType::PROD, {$1}); }
    | VarDec ASSIGN Exp                     { $$ = Node::createNodeWithChildren("Dec", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    ;

DefOrExp:
    Specifier DecList                       { $$ = Node::createNodeWithChildren("DecOrExp", @$.first_line, DataType::PROD, {$1, $2}); }
    | Exp                                   { $$ = Node::createNodeWithChildren("DecOrExp", @$.first_line, DataType::PROD, {$1}); }

MultiExp:
     Exp                                    { $$ = Node::createNodeWithChildren("MultiExp", @$.first_line, DataType::PROD, {$1}); }
    | MultiExp COMMA Exp                    { $$ = Node::createNodeWithChildren("MultiExp", @$.first_line, DataType::PROD, {$1, $2, $3}); }

/* Expression */
Exp:
      Exp ASSIGN Exp                        { $$ = Node::createNodeWithChildren("Exp", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | Exp AND Exp                           { $$ = Node::createNodeWithChildren("Exp", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | Exp OR Exp                            { $$ = Node::createNodeWithChildren("Exp", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | Exp LT Exp                            { $$ = Node::createNodeWithChildren("Exp", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | Exp LE Exp                            { $$ = Node::createNodeWithChildren("Exp", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | Exp GT Exp                            { $$ = Node::createNodeWithChildren("Exp", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | Exp GE Exp                            { $$ = Node::createNodeWithChildren("Exp", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | Exp NE Exp                            { $$ = Node::createNodeWithChildren("Exp", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | Exp EQ Exp                            { $$ = Node::createNodeWithChildren("Exp", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | Exp PLUS Exp                          { $$ = Node::createNodeWithChildren("Exp", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | Exp INCREASE                          { $$ = Node::createNodeWithChildren("Exp", @$.first_line, DataType::PROD, {$1, $2}); }
    | Exp DECREASE                          { $$ = Node::createNodeWithChildren("Exp", @$.first_line, DataType::PROD, {$1, $2}); }
    | Exp MINUS Exp                         { $$ = Node::createNodeWithChildren("Exp", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | Exp MUL Exp                           { $$ = Node::createNodeWithChildren("Exp", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | Exp DIV Exp                           { $$ = Node::createNodeWithChildren("Exp", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | LP Exp RP                             { $$ = Node::createNodeWithChildren("Exp", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | MINUS Exp                             { $$ = Node::createNodeWithChildren("Exp", @$.first_line, DataType::PROD, {$1, $2}); }
    | NOT Exp                               { $$ = Node::createNodeWithChildren("Exp", @$.first_line, DataType::PROD, {$1, $2}); }
    | ID LP Args RP                         { $$ = Node::createNodeWithChildren("Exp", @$.first_line, DataType::PROD, {$1, $2, $3, $4}); }
    | ID LP RP                              { $$ = Node::createNodeWithChildren("Exp", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | Exp LB Exp RB                         { $$ = Node::createNodeWithChildren("Exp", @$.first_line, DataType::PROD, {$1, $2, $3, $4}); }
    | Exp DOT ID                            { $$ = Node::createNodeWithChildren("Exp", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | Exp DEREF ID                          { $$ = Node::createNodeWithChildren("Exp", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | ID                                    { $$ = Node::createNodeWithChildren("Exp", @$.first_line, DataType::PROD, {$1}); }
    | INT                                   { $$ = Node::createNodeWithChildren("Exp", @$.first_line, DataType::PROD, {$1}); }
    | FLOAT                                 { $$ = Node::createNodeWithChildren("Exp", @$.first_line, DataType::PROD, {$1}); }
    | CHAR                                  { $$ = Node::createNodeWithChildren("Exp", @$.first_line, DataType::PROD, {$1}); }
    | STRING                                { $$ = Node::createNodeWithChildren("Exp", @$.first_line, DataType::PROD, {$1}); }
    | AR Exp                                { $$ = Node::createNodeWithChildren("Exp", @$.first_line, DataType::PROD, {$1, $2}); }
    | LP Specifier RP Exp                   { $$ = Node::createNodeWithChildren("Exp", @$.first_line, DataType::PROD, {$1, $2}); }
    | MUL Exp                               { $$ = Node::createNodeWithChildren("Exp", @$.first_line, DataType::PROD, {$1, $2}); }
    | BREAK                                 { $$ = Node::createNodeWithChildren("Exp", @$.first_line, DataType::PROD, {$1}); }
    | CONTINUE                              { $$ = Node::createNodeWithChildren("Exp", @$.first_line, DataType::PROD, {$1}); }
    | LP Exp error                          { outputFile << "Error type B at Line " << @$.first_line << ": Missing closing parenthesis ')'" << std::endl; errCount++; }
    | ID LP Args error                      { outputFile << "Error type B at Line " << @$.first_line << ": Missing closing parenthesis ')''" << std::endl; errCount++; }
    | ID LP error                           { outputFile << "Error type B at Line " << @$.first_line << ": Missing closing parenthesis ')''" << std::endl; errCount++; }
    ;
Args:
      Exp COMMA Args                        { $$ = Node::createNodeWithChildren("Args", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | Exp                                   { $$ = Node::createNodeWithChildren("Args", @$.first_line, DataType::PROD, {$1}); }
    ;
%%

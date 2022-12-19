%{
    #include "lex.yy.c"
    #include "compiler/Node.h"
    #include "compiler/Exp.h"

    #define YYINITDEPTH 40960
    #define YYSTACK_ALLOC

    void yyerror(const char*);

    extern Node* root;
    int errCount = 0;
    Node** yystack;
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
%token ADDROF
%token LT LE GT GE NE EQ
%token PTRACS INCREASE DECREASE
%token PLUS MINUS MUL DIV AND OR NOT
%token LP LB LC
%token RP RB RC

%right ASSIGN
%left INCREASE DECREASE
%left OR
%left AND
%left EQ NE
%left LT LE GT GE
%left PLUS MINUS
%left MUL DIV
%right NOT ADDROF
%left PTRACS DOT
%left LP RP LB RB

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
      ID LP VarList RP                      { yystack = yyvsp; $$ = Node::createNodeWithChildren("FunDec", @$.first_line, DataType::PROD, {$1, $2, $3, $4}); }
    | ID LP RP                              { yystack = yyvsp; $$ = Node::createNodeWithChildren("FunDec", @$.first_line, DataType::PROD, {$1, $2, $3}); }
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
      Exp SEMI                              { $$ = Node::createStmtNodeWithChildren("Stmt", @$.first_line, StmtType::SINGLE, {$1, $2}); }
    | CompSt                                { $$ = Node::createStmtNodeWithChildren("Stmt", @$.first_line, StmtType::COMP, {$1}); }
    | RETURN Exp SEMI                       { $$ = Node::createStmtNodeWithChildren("Stmt", @$.first_line, StmtType::RETURN, {$1, $2, $3}); }
    | RETURN SEMI                           { $$ = Node::createStmtNodeWithChildren("Stmt", @$.first_line, StmtType::RETURN, {$1, $2}); }
    | CONTINUE SEMI                         { yystack = yyvsp; $$ = Node::createStmtNodeWithChildren("Stmt", @$.first_line, StmtType::CONTINUE, {$1, $2}); }
    | BREAK SEMI                            { yystack = yyvsp; $$ = Node::createStmtNodeWithChildren("Stmt", @$.first_line, StmtType::BREAK, {$1, $2}); }
    | IF LP Exp RP Stmt %prec LOWER_ELSE    { $$ = Node::createStmtNodeWithChildren("Stmt", @$.first_line, StmtType::IF, {$1, $2, $3, $4, $5}); }
    | IF LP Exp RP Stmt ELSE Stmt           { $$ = Node::createStmtNodeWithChildren("Stmt", @$.first_line, StmtType::IF_ELSE, {$1, $2, $3, $4, $5, $6, $7}); }
    | WHILE LP Exp RP Stmt                  { $$ = Node::createStmtNodeWithChildren("Stmt", @$.first_line, StmtType::WHILE, {$1, $2, $3, $4, $5}); }
    | FOR LP DefOrExp SEMI Exp SEMI MultiExp RP Stmt  { $$ = Node::createStmtNodeWithChildren("Stmt", @$.first_line, StmtType::FOR, {$1, $2, $3, $4, $5, $6, $7, $8, $9}); }
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
    Specifier DecList                       { $$ = Node::createNodeWithChildren("DefOrExp", @$.first_line, DataType::PROD, {$1, $2}); }
    | Exp                                   { $$ = Node::createNodeWithChildren("DefOrExp", @$.first_line, DataType::PROD, {$1}); }
    ;

MultiExp:
     Exp                                    { $$ = Node::createNodeWithChildren("MultiExp", @$.first_line, DataType::PROD, {$1}); }
    | MultiExp COMMA Exp                    { $$ = Node::createNodeWithChildren("MultiExp", @$.first_line, DataType::PROD, {$1, $2, $3}); }

/* Expression */
Exp:
      Exp ASSIGN Exp                        { yystack = yyvsp; $$ = Node::createExpNodeWithChildren("Exp", @$.first_line, ExpType::ASSIGN, {$1, $2, $3}); }
    | Exp AND Exp                           { yystack = yyvsp; $$ = Node::createExpNodeWithChildren("Exp", @$.first_line, ExpType::AND, {$1, $2, $3}); }
    | Exp OR Exp                            { yystack = yyvsp; $$ = Node::createExpNodeWithChildren("Exp", @$.first_line, ExpType::OR, {$1, $2, $3}); }
    | NOT Exp                               { yystack = yyvsp; $$ = Node::createExpNodeWithChildren("Exp", @$.first_line, ExpType::NOT, {$1, $2}); }
    | Exp LT Exp                            { yystack = yyvsp; $$ = Node::createExpNodeWithChildren("Exp", @$.first_line, ExpType::LT, {$1, $2, $3}); }
    | Exp LE Exp                            { yystack = yyvsp; $$ = Node::createExpNodeWithChildren("Exp", @$.first_line, ExpType::LE, {$1, $2, $3}); }
    | Exp GT Exp                            { yystack = yyvsp; $$ = Node::createExpNodeWithChildren("Exp", @$.first_line, ExpType::GT, {$1, $2, $3}); }
    | Exp GE Exp                            { yystack = yyvsp; $$ = Node::createExpNodeWithChildren("Exp", @$.first_line, ExpType::GE, {$1, $2, $3}); }
    | Exp NE Exp                            { yystack = yyvsp; $$ = Node::createExpNodeWithChildren("Exp", @$.first_line, ExpType::NE, {$1, $2, $3}); }
    | Exp EQ Exp                            { yystack = yyvsp; $$ = Node::createExpNodeWithChildren("Exp", @$.first_line, ExpType::EQ, {$1, $2, $3}); }
    | Exp PLUS Exp                          { yystack = yyvsp; $$ = Node::createExpNodeWithChildren("Exp", @$.first_line, ExpType::PLUS, {$1, $2, $3}); }
    | Exp MINUS Exp                         { yystack = yyvsp; $$ = Node::createExpNodeWithChildren("Exp", @$.first_line, ExpType::MINUS, {$1, $2, $3}); }
    | Exp MUL Exp                           { yystack = yyvsp; $$ = Node::createExpNodeWithChildren("Exp", @$.first_line, ExpType::MUL, {$1, $2, $3}); }
    | Exp DIV Exp                           { yystack = yyvsp; $$ = Node::createExpNodeWithChildren("Exp", @$.first_line, ExpType::DIV, {$1, $2, $3}); }
    | Exp INCREASE                          { yystack = yyvsp; $$ = Node::createExpNodeWithChildren("Exp", @$.first_line, ExpType::INCREASE, {$1, $2}); }
    | Exp DECREASE                          { yystack = yyvsp; $$ = Node::createExpNodeWithChildren("Exp", @$.first_line, ExpType::DECREASE, {$1, $2}); }
    | MINUS Exp                             { yystack = yyvsp; $$ = Node::createExpNodeWithChildren("Exp", @$.first_line, ExpType::NEGATIVE_SIGN, {$1, $2}); }
    | LP Exp RP                             { yystack = yyvsp; $$ = Node::createExpNodeWithChildren("Exp", @$.first_line, ExpType::SCOPE, {$1, $2, $3}); }
    | ID LP Args RP                         { yystack = yyvsp; $$ = Node::createExpNodeWithChildren("Exp", @$.first_line, ExpType::FUNC_INVOKE, {$1, $2, $3, $4}); }
    | ID LP RP                              { yystack = yyvsp; $$ = Node::createExpNodeWithChildren("Exp", @$.first_line, ExpType::FUNC_INVOKE, {$1, $2, $3}); }
    | Exp LB Exp RB                         { yystack = yyvsp; $$ = Node::createExpNodeWithChildren("Exp", @$.first_line, ExpType::ARRAY_INDEX, {$1, $2, $3, $4}); }
    | ID                                    { yystack = yyvsp; $$ = Node::createExpNodeWithChildren("Exp", @$.first_line, ExpType::IDENTIFIER, {$1}); }
    | INT                                   { yystack = yyvsp; $$ = Node::createExpNodeWithChildren("Exp", @$.first_line, ExpType::LITERAL_INT, {$1}); }
    | FLOAT                                 { yystack = yyvsp; $$ = Node::createExpNodeWithChildren("Exp", @$.first_line, ExpType::LITERAL_FLOAT, {$1}); }
    | CHAR                                  { yystack = yyvsp; $$ = Node::createExpNodeWithChildren("Exp", @$.first_line, ExpType::LITERAL_CHAR, {$1}); }
    | STRING                                { yystack = yyvsp; $$ = Node::createExpNodeWithChildren("Exp", @$.first_line, ExpType::LITERAL_STRING, {$1}); }
    | Exp DOT ID                            { yystack = yyvsp; $$ = Node::createExpNodeWithChildren("Exp", @$.first_line, ExpType::DOT_ACCESS, {$1, $2, $3}); }
    | Exp PTRACS ID                         { yystack = yyvsp; $$ = Node::createExpNodeWithChildren("Exp", @$.first_line, ExpType::PTR_ACCESS, {$1, $2, $3}); }
    | ADDROF Exp                            { yystack = yyvsp; $$ = Node::createExpNodeWithChildren("Exp", @$.first_line, ExpType::ADDRESS_OF, {$1, $2}); }
    | MUL Exp                               { yystack = yyvsp; $$ = Node::createExpNodeWithChildren("Exp", @$.first_line, ExpType::DEREF, {$1, $2}); }
    | LP Specifier RP Exp                   { yystack = yyvsp; $$ = Node::createExpNodeWithChildren("Exp", @$.first_line, ExpType::TYPE_CAST, {$1, $2, $3, $4}); }
    | LP Exp error                          { outputFile << "Error type B at Line " << @$.first_line << ": Missing closing parenthesis ')'" << std::endl; errCount++; }
    | ID LP Args error                      { outputFile << "Error type B at Line " << @$.first_line << ": Missing closing parenthesis ')''" << std::endl; errCount++; }
    | ID LP error                           { outputFile << "Error type B at Line " << @$.first_line << ": Missing closing parenthesis ')''" << std::endl; errCount++; }
    ;
Args:
      Exp COMMA Args                        { $$ = Node::createNodeWithChildren("Args", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | Exp                                   { $$ = Node::createNodeWithChildren("Args", @$.first_line, DataType::PROD, {$1}); }
    ;
%%

%{
    #include "lex.yy.c"
    #include "node.hpp"

    #define YYSTYPE Node *

    #define YYINITDEPTH 40960
    #define YYSTACK_ALLOC

    void yyerror(const char*);

    Node *root;
%}
%locations

%token INT FLOAT CHAR STRING
%token ID TYPE ERR
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

%right ASSIGN
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
      ExtDefList                            { $$ = Node::create_node_with_children("Program", @$.first_line, DataType::PROD, {$1}); root = $$; }
    ;       
ExtDefList:
      /*empty*/                             { $$ = Node::create_node_with_children("ExtDefList", @$.first_line, DataType::PROD, {}); }
    | ExtDef ExtDefList                     { $$ = Node::create_node_with_children("ExtDefList", @$.first_line, DataType::PROD, {$1, $2}); }
    ;       
ExtDef:
      Specifier ExtDecList SEMI             { $$ = Node::create_node_with_children("ExtDef", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | Specifier SEMI                        { $$ = Node::create_node_with_children("ExtDef", @$.first_line, DataType::PROD, {$1, $2}); }
    | Specifier FunDec CompSt               { $$ = Node::create_node_with_children("ExtDef", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | Specifier ExtDecList error            { fprintf(output_file, "Error type B at Line %d: Missing semicolon ';'\n", @$.first_line); err_count++; }
    | Specifier error                       { fprintf(output_file, "Error type B at Line %d: Missing semicolon ';'\n", @$.first_line); err_count++; }
    ;
ExtDecList:
      VarDec                                { $$ = Node::create_node_with_children("ExtDecList", @$.first_line, DataType::PROD, {$1}); }
    | VarDec COMMA ExtDecList               { $$ = Node::create_node_with_children("ExtDecList", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    ;

/* specifier */       
Specifier:
      TYPE                                  { $$ = Node::create_node_with_children("Specifier", @$.first_line, DataType::PROD, {$1}); }
    | StructSpecifier                       { $$ = Node::create_node_with_children("Specifier", @$.first_line, DataType::PROD, {$1}); }
    ;
StructSpecifier:
      STRUCT ID LC DefList RC               { $$ = Node::create_node_with_children("StructSpecifier", @$.first_line, DataType::PROD, {$1, $2, $3, $4, $5}); }
    | STRUCT ID                             { $$ = Node::create_node_with_children("StructSpecifier", @$.first_line, DataType::PROD, {$1, $2}); }
    ;
/* declarator */        
VarDec:
      ID                                    { $$ = Node::create_node_with_children("VarDec", @$.first_line, DataType::PROD, {$1}); }
    | VarDec LB INT RB                      { $$ = Node::create_node_with_children("VarDec", @$.first_line, DataType::PROD, {$1, $2, $3, $4}); }
    | VarDec LB INT error                   { fprintf(output_file, "Error type B at Line %d: Missing closing brackets ']'\n", @$.first_line); err_count++; }
    | ERR                                   { fprintf(output_file, "Error type B at Line %d: bad identifier\n", @$.first_line); err_count++; }
    ;
FunDec:
      ID LP VarList RP                      { $$ = Node::create_node_with_children("FunDec", @$.first_line, DataType::PROD, {$1, $2, $3, $4}); }
    | ID LP RP                              { $$ = Node::create_node_with_children("FunDec", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | ID LP error                           { fprintf(output_file, "Error type B at Line %d: Missing closing parenthesis ')'\n", @$.first_line); err_count++; }
    | ID LP VarList error                   { fprintf(output_file, "Error type B at Line %d: Missing closing parenthesis ')'\n", @$.first_line); err_count++; }
    ;
VarList:        
      ParamDec COMMA VarList                { $$ = Node::create_node_with_children("VarList", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | ParamDec                              { $$ = Node::create_node_with_children("VarList", @$.first_line, DataType::PROD, {$1}); }
    ;
ParamDec:       
      Specifier VarDec                      { $$ = Node::create_node_with_children("ParamDec", @$.first_line, DataType::PROD, {$1, $2}); }
    ;

/* statement */
CompSt:
      LC DefList StmtList RC                { $$ = Node::create_node_with_children("CompSt", @$.first_line, DataType::PROD, {$1, $2, $3, $4}); }
    ;
StmtList:
      /*empty*/                             { $$ = Node::create_node_with_children("StmtList", @$.first_line, DataType::PROD, {}); }
    | Stmt StmtList                         { $$ = Node::create_node_with_children("StmtList", @$.first_line, DataType::PROD, {$1, $2}); }
    ;
Stmt:
      Exp SEMI                              { $$ = Node::create_node_with_children("Stmt", @$.first_line, DataType::PROD, {$1, $2}); }
    | CompSt                                { $$ = Node::create_node_with_children("Stmt", @$.first_line, DataType::PROD, {$1}); }
    | RETURN Exp SEMI                       { $$ = Node::create_node_with_children("Stmt", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | IF LP Exp RP Stmt %prec LOWER_ELSE    { $$ = Node::create_node_with_children("Stmt", @$.first_line, DataType::PROD, {$1, $2, $3, $4, $5}); }
    | IF LP Exp RP Stmt ELSE Stmt           { $$ = Node::create_node_with_children("Stmt", @$.first_line, DataType::PROD, {$1, $2, $3, $4, $5, $6, $7}); }
    | WHILE LP Exp RP Stmt                  { $$ = Node::create_node_with_children("Stmt", @$.first_line, DataType::PROD, {$1, $2, $3, $4, $5}); }
    | FOR LP Exp SEMI Exp SEMI Exp RP Stmt  { $$ = Node::create_node_with_children("Stmt", @$.first_line, DataType::PROD, {$1, $2, $3, $4, $5, $6, $7, $8, $9}); }
    | Exp error                             { fprintf(output_file, "Error type B at Line %d: Missing semicolon ';'\n", @$.first_line); err_count++; }
    | RETURN Exp error                      { fprintf(output_file, "Error type B at Line %d: Missing semicolon ';'\n", @$.first_line); err_count++; }
    | WHILE LP Exp error Stmt               { fprintf(output_file, "Error type B at Line %d: Missing closing parenthesis ')'\n", @$.first_line); err_count++; }
    | IF LP Exp error Stmt %prec LOWER_ELSE { fprintf(output_file, "Error type B at Line %d: Missing closing parenthesis ')'\n", @$.first_line); err_count++; }
    | IF LP Exp error Stmt ELSE Stmt        { fprintf(output_file, "Error type B at Line %d: Missing closing parenthesis ')'\n", @$.first_line); err_count++; }
    ;

/* local definition */
DefList:
      /*empty*/                             { $$ = Node::create_node_with_children("DefList", @$.first_line, DataType::PROD, {}); }
    | Def DefList                           { $$ = Node::create_node_with_children("DefList", @$.first_line, DataType::PROD, {$1, $2}); }
    ;
Def:
      Specifier DecList SEMI                { $$ = Node::create_node_with_children("Def", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | Specifier DecList error               { fprintf(output_file, "Error type B at Line %d: Missing semicolon ';'\n", @$.first_line); err_count++; }
    | error DecList SEMI                    { fprintf(output_file, "Error type B at Line %d: Missing specifier\n", @$.first_line); err_count++; }
    ;
DecList:
      Dec                                   { $$ = Node::create_node_with_children("DecList", @$.first_line, DataType::PROD, {$1}); }
    | Dec COMMA DecList                     { $$ = Node::create_node_with_children("DecList", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    ;
Dec:
      VarDec                                { $$ = Node::create_node_with_children("Dec", @$.first_line, DataType::PROD, {$1}); }
    | VarDec ASSIGN Exp                     { $$ = Node::create_node_with_children("Dec", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    ;

/* Expression */
Exp:
      Exp ASSIGN Exp                        { $$ = Node::create_node_with_children("Exp", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | Exp AND Exp                           { $$ = Node::create_node_with_children("Exp", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | Exp OR Exp                            { $$ = Node::create_node_with_children("Exp", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | Exp LT Exp                            { $$ = Node::create_node_with_children("Exp", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | Exp LE Exp                            { $$ = Node::create_node_with_children("Exp", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | Exp GT Exp                            { $$ = Node::create_node_with_children("Exp", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | Exp GE Exp                            { $$ = Node::create_node_with_children("Exp", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | Exp NE Exp                            { $$ = Node::create_node_with_children("Exp", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | Exp EQ Exp                            { $$ = Node::create_node_with_children("Exp", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | Exp PLUS Exp                          { $$ = Node::create_node_with_children("Exp", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | Exp MINUS Exp                         { $$ = Node::create_node_with_children("Exp", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | Exp MUL Exp                           { $$ = Node::create_node_with_children("Exp", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | Exp DIV Exp                           { $$ = Node::create_node_with_children("Exp", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | LP Exp RP                             { $$ = Node::create_node_with_children("Exp", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | MINUS Exp                             { $$ = Node::create_node_with_children("Exp", @$.first_line, DataType::PROD, {$1, $2}); }
    | NOT Exp                               { $$ = Node::create_node_with_children("Exp", @$.first_line, DataType::PROD, {$1, $2}); }
    | ID LP Args RP                         { $$ = Node::create_node_with_children("Exp", @$.first_line, DataType::PROD, {$1, $2, $3, $4}); }
    | ID LP RP                              { $$ = Node::create_node_with_children("Exp", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | Exp LB Exp RB                         { $$ = Node::create_node_with_children("Exp", @$.first_line, DataType::PROD, {$1, $2, $3, $4}); }
    | Exp DOT ID                            { $$ = Node::create_node_with_children("Exp", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | ID                                    { $$ = Node::create_node_with_children("Exp", @$.first_line, DataType::PROD, {$1}); }
    | INT                                   { $$ = Node::create_node_with_children("Exp", @$.first_line, DataType::PROD, {$1}); }
    | FLOAT                                 { $$ = Node::create_node_with_children("Exp", @$.first_line, DataType::PROD, {$1}); }
    | CHAR                                  { $$ = Node::create_node_with_children("Exp", @$.first_line, DataType::PROD, {$1}); }
    | STRING                                { $$ = Node::create_node_with_children("Exp", @$.first_line, DataType::PROD, {$1}); }
    | AR Exp                                { $$ = Node::create_node_with_children("Exp", @$.first_line, DataType::PROD, {$1, $2}); }
    | LP TYPE RP Exp                        { $$ = Node::create_node_with_children("Exp", @$.first_line, DataType::PROD, {$1, $2}); }
    | MUL Exp                               { $$ = Node::create_node_with_children("Exp", @$.first_line, DataType::PROD, {$1, $2}); }
    | BREAK                                 { $$ = Node::create_node_with_children("Exp", @$.first_line, DataType::PROD, {$1}); }
    | CONTINUE                              { $$ = Node::create_node_with_children("Exp", @$.first_line, DataType::PROD, {$1}); }
    | LP Exp error                          { fprintf(output_file, "Error type B at Line %d: Missing closing parenthesis ')'\n", @$.first_line); err_count++; }
    | ID LP Args error                      { fprintf(output_file, "Error type B at Line %d: Missing closing parenthesis ')'\n", @$.first_line); err_count++; }
    | ID LP error                           { fprintf(output_file, "Error type B at Line %d: Missing closing parenthesis ')'\n", @$.first_line); err_count++; }
    ;
Args:
      Exp COMMA Args                        { $$ = Node::create_node_with_children("Args", @$.first_line, DataType::PROD, {$1, $2, $3}); }
    | Exp                                   { $$ = Node::create_node_with_children("Args", @$.first_line, DataType::PROD, {$1}); }
    ;
%%
void yyerror(const char *s) {
    fprintf(stderr, "%s\n", s);
}

int main(int argc, char **argv) {
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        exit(-1);
    }
    else if(!(yyin = fopen(argv[1], "r"))) {
        perror(argv[1]);
        exit(-1);
    }
    init_args(argv[1]);

    yyparse();

    if (err_count > 0)
    {
        printf("Error Occur\n");
        fflush(output_file);
        fclose(output_file);
    }
    else
    {
        fclose(output_file);
        printf("%p\n", root);
        output_tree(root);
    }
    return 0;
}

%{
    #define EXIT_OK 0
    #define EXIT_FAIL 1

    #include "node.hpp"

    int yycolno = 1;
    #define YY_USER_ACTION \
        yylloc.first_line = yylineno; \
        yylloc.last_line = yylineno; \
        yylloc.first_column = yycolno; \
        yylloc.last_column = yycolno + yyleng;\
        yycolno += yyleng;

    extern "C" int yylex();

    // global variables
    int identifiers = 0;
    int lines = 1;
    int keywords = 0;
    int constants = 0;
    int operators = 0;
    char c;

    int count_occu(const char * str, const char ch) {
        int cnt = 0;
        while(*str != '\0') {
            if (*str == ch) cnt++;
            str++;
        }
        return cnt;
    }
%}

%option yylineno

letter  [a-zA-Z]
letter_ {letter}|_
digit   [0-9]
hex     [a-fA-F0-9]

ws      [ \t\r\n]+
ws0     [ \t\r]+

identifier      {letter_}({letter_}|{digit})*

decimal-literal [0-9]+
hex-literal     0x[a-fA-F0-9]+
binary-literal  0b[01]+

integersuffix   (L|LL|U|UL|LU|ULL|LLU)
integer-literal ({decimal-literal}|{hex-literal}|{binary-literal}){integersuffix}?

float-literal   {digit}+([fd]|(\.{digit}+[fd]?))[fd]?
char        \'(.|(\\x{hex}{1,2})|(\\.))\'

%%
\n                  { lines++; yycolno = 1; }    
{ws0}               { /**/ }

"//" { while((c = yyinput()) != '\n'); unput(c); }
"/*" {
    c = yyinput();
    while(1) {
        char tmp = yyinput();
        if (c == '*' && tmp == '/') break;
        c = tmp;
        if (c == '\n') lines++;
    }
}

{integer-literal}   { yylval = new Node("INT", yylloc.first_line, DataType::INT);       return INT; }
{float-literal}     { yylval = new Node("FLOAT", yylloc.first_line, DataType::FLOAT);   return FLOAT; }
{char}              {
    if (yytext[yyleng - 2] =='\\' && yytext[yyleng - 1] != '\'') {
        yyless(yyleng - 1);
        yymore();
    } else {
        yylval = new Node("CHAR", yylloc.first_line, DataType::CHAR);
        return CHAR;
    }
}

(int|float|char)({ws}?\*)? {
                      yylval = new Node("TYPE", yylloc.first_line, DataType::DTYPE, yytext);    return TYPE; }

struct              { yylval = new Node("STRUCT", yylloc.first_line, DataType::OTHER);          return STRUCT; }
if                  { yylval = new Node("IF", yylloc.first_line, DataType::OTHER);              return IF; }
else                { yylval = new Node("ELSE", yylloc.first_line, DataType::OTHER);            return ELSE; }
while               { yylval = new Node("WHILE", yylloc.first_line, DataType::OTHER);           return WHILE; }
for                 { yylval = new Node("FOR", yylloc.first_line, DataType::OTHER);             return FOR; }
break               { yylval = new Node("BREAK", yylloc.first_line, DataType::OTHER);           return BREAK; }
continue            { yylval = new Node("CONTINUE", yylloc.first_line, DataType::OTHER);        return CONTINUE; }
return              { yylval = newEOL Node("RETURN", yylloc.first_line, DataType::OTHER);       return RETURN; }

"."                 { yylval = new Node("DOT", yylloc.first_line, DataType::OTHER);             return DOT; }
";"                 { yylval = new Node("SEMI", yylloc.first_line, DataType::OTHER);            return SEMI; }
","                 { yylval = new Node("COMMA", yylloc.first_line, DataType::OTHER);           return COMMA; }

"="                 { yylval = new Node("ASSIGN", yylloc.first_line, DataType::OTHER);          return ASSIGN; }
"<"                 { yylval = new Node("LT", yylloc.first_line, DataType::OTHER);              return LT; }
"<="                { yylval = new Node("LE", yylloc.first_line, DataType::OTHER);              return LE; }
">"                 { yylval = new Node("GT", yylloc.first_line, DataType::OTHER);              return GT; }
">="                { yylval = new Node("GE", yylloc.first_line, DataType::OTHER);              return GE; }
"!="                { yylval = new Node("NE", yylloc.first_line, DataType::OTHER);              return NE; }
"=="                { yylval = new Node("EQ", yylloc.first_line, DataType::OTHER);              return EQ; }
"+"                 { yylval = new Node("PLUS", yylloc.first_line, DataType::OTHER);            return PLUS; }
"-"                 { yylval = new Node("MINUS", yylloc.first_line, DataType::OTHER);           return MINUS; }
"*"                 { yylval = new Node("MUL", yylloc.file_line, DataType::OTHER);              return MUL; }
"/"                 { yylval = new Node("DIV", yylloc.first_line, DataType::OTHER);             return DIV; }
"&"                 { yylval = new Node("AR", yylloc.first_line, DataType::OTHER);              return AR; }
"&&"                { yylval = new Node("AND", yylloc.first_line, DataType::OTHER);             return AND; }
"||"                { yylval = new Node("OR", yylloc.first_line, DataType::OTHER);              return OR; }
"!"                 { yylval = new Node("NOT", yylloc.first_line, DataType::OTHER);             return NOT; }
"("                 { yylval = new Node("LP", yylloc.first_line, DataType::OTHER);              return LP; }
")"                 { yylval = new Node("RP", yylloc.first_line, DataType::OTHER);              return RP; }
"["                 { yylval = new Node("LB", yylloc.first_line, DataType::OTHER);              return LB; }
"]"                 { yylval = new Node("RB", yylloc.first_line, DataType::OTHER);              return RB; }
"{"                 { yylval = new Node("LC", yylloc.first_line, DataType::OTHER);              return LC; }
"}"                 { yylval = new Node("RC", yylloc.first_line, DataType::OTHER);              return RC; }

{identifier}        { yylval = new Node("ID", yylloc.first_line, DataType::ID);                 return ID; }

<<EOF>> {}

%%
int main(int argc, char **argv){
    char *file_path;
    if(argc < 2){
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        return EXIT_FAIL;
    } else if(argc == 2){
        file_path = argv[1];
        if(!(yyin = fopen(file_path, "r"))){
            perror(argv[1]);
            return EXIT_FAIL;
        }
        yylex();
        return EXIT_OK;
    } else{
        fputs("Too many arguments! Expected: 2.\n", stderr);
        return EXIT_FAIL;
    }
}


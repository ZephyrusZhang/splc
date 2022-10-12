%{
    #define EXIT_OK 0
    #define EXIT_FAIL 1

    #ifndef Debug
        #define YYSTYPE Node *

        #include "node.hpp"
        #include "syntax.tab.h"

        int yycolno = 1;
        #define YY_USER_ACTION \
            yylloc.first_line = yylineno; \
            yylloc.first_column = yycolno; \
            yylloc.last_line = yylineno; \
            yylloc.last_column = yycolno + yyleng; \
            yycolno += yyleng;
    #endif

    #ifdef Debug
        #define ACT(build, debug) debug
    #else
        #define ACT(build, debug) build
    #endif


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
%option noyywrap

letter  [a-zA-Z]
letter_ {letter}|_
digit   [0-9]
hex     [a-fA-F0-9]

ws      [ \t\t\r\n]+
ws0     [ \t\t\r]+

identifier      {letter_}({letter_}|{digit})*

decimal-literal [0-9]+
hex-literal     0x[a-fA-F0-9]+
binary-literal  0b[01]+

string \"[^\"]*\"
integersuffix   (L|LL|U|UL|LU|ULL|LLU)
integer-literal ({decimal-literal}|{hex-literal}|{binary-literal}){integersuffix}?
float-literal   {digit}+([fd]|(\.{digit}+[fd]?))[fd]?
char        \'(.|(\\x{hex}{1,2})|(\\.))\'

%%
\n                  { ACT(yycolno = 1;,) }    
{ws0}               { /**/ }

"//" { while((c = yyinput()) != '\n'); unput(c); }
"/*" {
    c = yyinput();
    while(1) {
        char tmp = yyinput();
        if (c == '*' && tmp == '/') break;
        c = tmp;
    }
}

{string}            {
    if (yytext[yyleng-2]=='\\') {
        yyless(yyleng - 1);
        yymore();
    } else {
        constants++;
        printf("line %d: str: \t\t%s\n", lines, yytext); 
    }
}

{integer-literal}   { ACT(yylval = new Node("INT", yylloc.first_line, DataType::INT);               return INT;,        printf("line %d: int: \t\t%s\n", yylineno, yytext);) }
{float-literal}     { ACT(yylval = new Node("FLOAT", yylloc.first_line, DataType::FLOAT);           return FLOAT;,      printf("line %d: float: \t%s\n", yylineno, yytext);) }
{char}              {
    if (yytext[yyleng - 2] =='\\' && yytext[yyleng - 1] != '\'') {
        yyless(yyleng - 1);
        yymore();
    } else {
        ACT(yylval = new Node("CHAR", yylloc.first_line, DataType::CHAR);return CHAR;, printf("line %d: char: \t\t%s\n", yylineno, yytext);)
    }
}

(int|float|char)({ws}?\*)? {
                      ACT(yylval = new Node("TYPE", yylloc.first_line, DataType::DTYPE, yytext);    return TYPE;,       printf("line %d: type: \t\t%s\n", yylineno, yytext);) }

struct              { ACT(yylval = new Node("STRUCT", yylloc.first_line, DataType::OTHER);          return STRUCT;,     printf("line %d: kw: \t\t%s\n", yylineno, yytext);) }
if                  { ACT(yylval = new Node("IF", yylloc.first_line, DataType::OTHER);              return IF;,         printf("line %d: kw: \t\t%s\n", yylineno, yytext);) }
else                { ACT(yylval = new Node("ELSE", yylloc.first_line, DataType::OTHER);            return ELSE;,       printf("line %d: kw: \t\t%s\n", yylineno, yytext);) }
while               { ACT(yylval = new Node("WHILE", yylloc.first_line, DataType::OTHER);           return WHILE;,      printf("line %d: kw: \t\t%s\n", yylineno, yytext);) }
for                 { ACT(yylval = new Node("FOR", yylloc.first_line, DataType::OTHER);             return FOR;,        printf("line %d: kw: \t\t%s\n", yylineno, yytext);) }
break               { ACT(yylval = new Node("BREAK", yylloc.first_line, DataType::OTHER);           return BREAK;,      printf("line %d: kw: \t\t%s\n", yylineno, yytext);) }
continue            { ACT(yylval = new Node("CONTINUE", yylloc.first_line, DataType::OTHER);        return CONTINUE;,   printf("line %d: kw: \t\t%s\n", yylineno, yytext);) }
return              { ACT(yylval = new Node("RETURN", yylloc.first_line, DataType::OTHER);          return RETURN;,     printf("line %d: kw: \t\t%s\n", yylineno, yytext);) }

"."                 { ACT(yylval = new Node("DOT", yylloc.first_line, DataType::OTHER);             return DOT;,        printf("line %d: dot: \t\t%s\n", yylineno, yytext);) }
";"                 { ACT(yylval = new Node("SEMI", yylloc.first_line, DataType::OTHER);            return SEMI;,       printf("line %d: semi: \t\t%s\n", yylineno, yytext);) }
","                 { ACT(yylval = new Node("COMMA", yylloc.first_line, DataType::OTHER);           return COMMA;,      printf("line %d: comma: \t\t%s\n", yylineno, yytext);) }

"="                 { ACT(yylval = new Node("ASSIGN", yylloc.first_line, DataType::OTHER);          return ASSIGN;,     printf("line %d: assign: \t%s\n", yylineno, yytext);) }
"<"                 { ACT(yylval = new Node("LT", yylloc.first_line, DataType::OTHER);              return LT;,         printf("line %d: lt: \t\t%s\n", yylineno, yytext);) }
"<="                { ACT(yylval = new Node("LE", yylloc.first_line, DataType::OTHER);              return LE;,         printf("line %d: le: \t\t%s\n", yylineno, yytext);) }
">"                 { ACT(yylval = new Node("GT", yylloc.first_line, DataType::OTHER);              return GT;,         printf("line %d: gt: \t\t%s\n", yylineno, yytext);) }
">="                { ACT(yylval = new Node("GE", yylloc.first_line, DataType::OTHER);              return GE;,         printf("line %d: ge: \t\t%s\n", yylineno, yytext);) }
"!="                { ACT(yylval = new Node("NE", yylloc.first_line, DataType::OTHER);              return NE;,         printf("line %d: ne: \t\t%s\n", yylineno, yytext);) }
"=="                { ACT(yylval = new Node("EQ", yylloc.first_line, DataType::OTHER);              return EQ;,         printf("line %d: eq: \t\t%s\n", yylineno, yytext);) }
"+"                 { ACT(yylval = new Node("PLUS", yylloc.first_line, DataType::OTHER);            return PLUS;,       printf("line %d: plus: \t\t%s\n", yylineno, yytext);) }
"-"                 { ACT(yylval = new Node("MINUS", yylloc.first_line, DataType::OTHER);           return MINUS;,      printf("line %d: minus: \t%s\n", yylineno, yytext);) }
"*"                 { ACT(yylval = new Node("MUL", yylloc.first_line, DataType::OTHER);             return MUL;,        printf("line %d: mul: \t\t%s\n", yylineno, yytext);) }
"/"                 { ACT(yylval = new Node("DIV", yylloc.first_line, DataType::OTHER);             return DIV;,        printf("line %d: div: \t\t%s\n", yylineno, yytext);) }
"&"                 { ACT(yylval = new Node("AR", yylloc.first_line, DataType::OTHER);              return AR;,         printf("line %d: ar: \t\t%s\n", yylineno, yytext);) }
"&&"                { ACT(yylval = new Node("AND", yylloc.first_line, DataType::OTHER);             return AND;,        printf("line %d: and: \t\t%s\n", yylineno, yytext);) }
"||"                { ACT(yylval = new Node("OR", yylloc.first_line, DataType::OTHER);              return OR;,         printf("line %d: or: \t\t%s\n", yylineno, yytext);) }
"!"                 { ACT(yylval = new Node("NOT", yylloc.first_line, DataType::OTHER);             return NOT;,        printf("line %d: not: \t\t%s\n", yylineno, yytext);) }
"("                 { ACT(yylval = new Node("LP", yylloc.first_line, DataType::OTHER);              return LP;,         printf("line %d: lp: \t\t%s\n", yylineno, yytext);) }
")"                 { ACT(yylval = new Node("RP", yylloc.first_line, DataType::OTHER);              return RP;,         printf("line %d: rp: \t\t%s\n", yylineno, yytext);) }
"["                 { ACT(yylval = new Node("LB", yylloc.first_line, DataType::OTHER);              return LB;,         printf("line %d: lb: \t\t%s\n", yylineno, yytext);) }
"]"                 { ACT(yylval = new Node("RB", yylloc.first_line, DataType::OTHER);              return RB;,         printf("line %d: rb: \t\t%s\n", yylineno, yytext);) }
"{"                 { ACT(yylval = new Node("LC", yylloc.first_line, DataType::OTHER);              return LC;,         printf("line %d: lc: \t\t%s\n", yylineno, yytext);) }
"}"                 { ACT(yylval = new Node("RC", yylloc.first_line, DataType::OTHER);              return RC;,         printf("line %d: rc: \t\t%s\n", yylineno, yytext);) }

{identifier}        { ACT(yylval = new Node("ID", yylloc.first_line, DataType::ID);                 return ID;,         printf("line %d: id: \t\t%s\n", yylineno, yytext);) }

<<EOF>>             { ACT( , yyterminate();) }

%%
#ifdef Debug
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
#endif


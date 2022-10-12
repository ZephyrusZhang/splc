%{
    #define EXIT_OK 0
    #define EXIT_FAIL 1

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

letter  [a-zA-Z]
letter_ {letter}|_
digit   [0-9]
hex     [a-fA-F0-9]

ws      [ \t\r\n]+
ws0     [ \t\r]+

crawtype        char|short|int|long|float|double|void
ctype           (const{ws}+)?((unsigned|signed){ws}+)?{crawtype}(const{ws}+)?({ws}?\*{ws}?(const{ws}+)?)*
identifier      {letter_}({letter_}|{digit})*
keywords        if|else|while|return|typedef|break|case|continue|default|do|for|goto|struct|switch
marco_nextline  {ws0}(.*(\\(\n|\r\n))?)*
marco           (define{marco_nextline})|undef|(if{marco_nextline})|endif|(ifndef{marco_nextline})|(ifdef{marco_nextline})|else|(include.*)|[a-zA-Z0-9_-]+{marco_nextline}

decimal-literal [0-9]+
hex-literal     0x[a-fA-F0-9]+
binary-literal  0b[01]+

integersuffix   (L|LL|U|UL|LU|ULL|LLU)
integer-literal ({decimal-literal}|{hex-literal}|{binary-literal}){integersuffix}?

float-literal   {digit}+([fd]|(\.{digit}+[fd]?))[fd]?

string      \"[^\"]*\"
char        \'(.|(\\x{hex}{1,2})|(\\.))\'
operators   =|!=|==|\<=|\>=|\<|\>|\+|\+\+|-|--|\*|\/|%|\!|&|&&|\||\|\||\[|\]|\.|\?|:
EOL         ;
comma       \,
scope       \{|\}|\(|\)
%%

\n                  { lines++; }    
{ws0}               { /**/ }
\#{marco}           {  printf("line %d: mcr: \t%s\n", lines, yytext); lines += count_occu(yytext, '\n'); }
"//" { 
  // ignore the following chars until seeing a newline character
  while((c = yyinput()) != '\n'); 
  // put the newline character back to the input buffer
  unput(c); 
}
"/*" {
    // printf("line %d: cmt: ", lines);
    c = yyinput();
    while(1) {
        char tmp = yyinput();
        if (c == '*' && tmp == '/') break;
        c = tmp;
        if (c == '\n') lines++;
        // printf("%c", c);
    }
    // printf("\n");
}
{operators}         { operators++;      printf("line %d: op: \t%s\n", lines, yytext); }
{string}            {
    if (yytext[yyleng-2]=='\\') {
        yyless(yyleng - 1);
        yymore();
    } else {
        constants++;
        printf("line %d: str: \t%s\n", lines, yytext); 
    }
}
{char}              {
    if (yytext[yyleng - 2] =='\\' && yytext[yyleng - 1] != '\'') {
        yyless(yyleng - 1);
        yymore();
    } else {
        constants++;
        printf("line %d: char: \t%s\n", lines, yytext);
    }
}
{keywords}          { keywords++;       printf("line %d: kw: \t%s\n", lines, yytext); }
{ctype}             { keywords++;       printf("line %d: type: \t%s\n", lines, yytext); }
{identifier}        { identifiers++;    printf("line %d: id: \t%s\n", lines, yytext); }
{integer-literal}   { constants++;      printf("line %d: int: \t%s\n", lines, yytext); }
{float-literal}     { constants++;      printf("line %d: flt: \t%s\n", lines, yytext); }
{scope}             {                   printf("line %d: scope \t%s\n", lines, yytext); }
{EOL}               {                   printf("line %d: end \t;\n", lines); }
{comma}             { /**/ }
.                   {                   printf("line %d: unmatched: \t%s\n", lines, yytext); }
<<EOF>> { printf("There are %d occurrences of valid identifiers\n", identifiers); yyterminate(); }

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
        // FlexLexer* lexer = new yyFlexLexer;
        // lexer->yylex();
        yylex();
        return EXIT_OK;
    } else{
        fputs("Too many arguments! Expected: 2.\n", stderr);
        return EXIT_FAIL;
    }
}


%{
    #define EXIT_OK 0
    #define EXIT_FAIL 1
    #define MAX_INCLUDE_DEPTH 8

    #include <string>
    #include <cstdlib>
    #include <cstdio>

    YY_BUFFER_STATE include_stack[MAX_INCLUDE_DEPTH];
    int include_stack_ptr = 0;

    extern "C"
    {
        int yylex(void);
    }
%}
%x INCLUDE

%%
([a-z]+)|([^a-z\n]*\n?) ECHO;

"#include"          BEGIN(INCLUDE);
<INCLUDE>[^ \t\n]+ {
    if (include_stack_ptr >= MAX_INCLUDE_DEPTH)
    {
    fprintf( stderr, "Includes nested too deeply" );
    exit(EXIT_FAIL);
    }

    include_stack[include_stack_ptr++] = YY_CURRENT_BUFFER;

    std::string text(yytext);
    std::string path = text.substr(1, text.length() - 2);

    yyin = fopen(path.c_str(), "r");
    if (!yyin)
    {
        fprintf(stderr, "No such file: \"%s\"\n", path);
        exit(EXIT_FAIL);
    }

    yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE));

    BEGIN(INITIAL);
}
<INCLUDE>[ \t]* { /*skip*/ }

<<EOF>> {
    if (--include_stack_ptr < 0)
    {
        yyterminate();
    }
    else
    {
        yy_delete_buffer(YY_CURRENT_BUFFER);
        yy_switch_to_buffer(include_stack[include_stack_ptr]);
    }
}

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
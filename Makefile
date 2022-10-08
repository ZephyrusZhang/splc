CC=gcc
FLEX=flex
BISON=bison

preprocessor:
	$(FLEX) preprocess.l
	$(CC) lex.yy.c -lfl -o preprocessor.out
lexer:
	$(BISON) -d syntax.y
	$(FLEX) lex.l
	$(CC) lex.yy.c -lfl -o lexer.out
parser:
	$(BISON) -d syntax.y
	$(FLEX) lex.l
	$(CC) syntax.tab.c -lfl -ly -o parser.out
clean:
	@rm -f lex.yy.c *.out
.PHONY: lexer
CC=gcc
FLEX=flex

lexer:
	$(FLEX) lex.l
	$(CC) lex.yy.c -lfl -o lexer.out
clean:
	@rm -f lex.yy.c *.out
.PHONY: lexer
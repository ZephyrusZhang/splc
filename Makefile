CC=gcc
CXX=g++
FLEX=flex
BISON=bison

CXX_FLAGS += -DDebug

pre:
	$(FLEX) preprocess.l
	$(CXX) lex.yy.c -lfl -o preprocessor.out
lex:
	$(BISON) -d syntax.y
	$(FLEX) lex.l
	$(CXX) lex.yy.c -lfl -o lexer.out
parse:
	$(BISON) -d syntax.y
	$(FLEX) lex.l
	$(CC) syntax.tab.c -lfl -ly -o parser.out
clean:
	@rm -f lex.yy.* *.out syntax.tab.*
.PHONY: lexer
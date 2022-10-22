CC=gcc
CXX=g++
FLEX=flex
BISON=bison
OUT_NAME=parser

CXX_FLAGS=-lfl -ly

.y: syntax.y
	$(BISON) -d syntax.y --report all
.l: lex.l
	$(FLEX) lex.l
release: .y .l
	mkdir -p bin && $(CXX) $(CXX_FLAGS) syntax.tab.c -o bin/$(OUT_NAME)
debug: .y .l
	mkdir -p bin && $(CXX) -DDebug $(CXX_FLAGS) syntax.tab.c -o bin/$(OUT_NAME)
clean:
	@rm -rf lex.yy.* syntax.yy.* bin out
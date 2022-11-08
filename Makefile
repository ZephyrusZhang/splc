CC=gcc
CXX=g++
FLEX=flex
BISON=bison
OUT_NAME=splc

ADDRESS_SANITIZER = -O0 -g -fsanitize=address -fno-omit-frame-pointer
CXX_FLAGS = -Wl,-z stack-size=16777216 -lfl -ly

release:
	$(FLEX) lex.l
	$(BISON) -d syntax.y --report all -Wcounterexamples -Wconflicts-sr -Wconflicts-rr
	mkdir -p bin
	$(CXX) $(CXX_FLAGS) syntax.tab.c compiler/node.cpp compiler/main.cpp -o bin/$(OUT_NAME)
debug: .y .l
	$(FLEX) lex.l
	$(BISON) -d syntax.y --report all -Wcounterexamples -Wconflicts-sr -Wconflicts-rr
	mkdir -p bin
	$(CXX) -DDebug $(CXX_FLAGS) syntax.tab.c -o bin/$(OUT_NAME)
clean:
	@rm -rf lex.yy.* syntax.tab.* bin out *.out *.output
	@rm -rf test/*.out
all: release
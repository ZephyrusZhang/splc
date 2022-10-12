FLEX=flex
CXX=g++

.lex: lex.l
	$(FLEX) lex.l
test: .lex
	mkdir -p bin && $(CXX) -DDebug lex.yy.c -lfl -o bin/debug
clean:
	@rm -rf lex.yy.c bin
all: test

CC=gcc
CXX=g++
FLEX=flex
BISON=bison
OUT_NAME=bin/splc
BUILDDIR=build
SRCS=syntax.tab compiler/Container compiler/main compiler/Node compiler/Specifier compiler/Scope compiler/Dec compiler/Def compiler/Exp
OBJS=$(SRCS:%=$(BUILDDIR)/%.o)

ADDRESS_SANITIZER = -O0 -g -fsanitize=address -fno-omit-frame-pointer
CXXFLAGS := -g $(ADDRESS_SANITIZER)
CXXFLAGS += -std=c++20
LDFLAGS  = -lfl -ly $(ADDRESS_SANITIZER)

.DEFAULT_GOAL := release

build/lex.yy.c: lex.l
	@echo "[flex] $<"
	@$(FLEX) -o build/lex.yy.c lex.l

build/syntax.tab.c: syntax.y build/lex.yy.c
	@echo "[bison] $<"
	@$(BISON) -o build/syntax.tab.c -d syntax.y --report all -Wcounterexamples -Wconflicts-sr -Wconflicts-rr

build/syntax.tab.o: build/syntax.tab.c
	@echo "[g++] syntax.tab.c"
	@$(CXX) $(CXXFLAGS) -I. -c build/syntax.tab.c -o build/syntax.tab.o

$(BUILDDIR)/%.o : %.c build/syntax.tab.o
	@echo "[gcc] $<"
	@$(CXX) $(CXXFLAGS) -Ibuild -c $< -o $@

$(BUILDDIR)/%.o : %.cpp build/syntax.tab.o
	@echo "[g++] $<"
	@$(CXX) $(CXXFLAGS) -Ibuild -c $< -o $@

$(OUT_NAME): $(OBJS)
	@mkdir -p bin
	@echo "[ld] linking splc"
	@$(CXX) $(LDFLAGS) $(OBJS) -o $(OUT_NAME)

release: $(OUT_NAME)

clean:
	@rm -rf lex.yy.* syntax.tab.* bin out *.out *.output
	@rm -rf test/*.out
	@rm -rf build
	@mkdir -p build
	@mkdir -p build/compiler

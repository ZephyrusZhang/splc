CC=gcc
CXX=g++
FLEX=flex
BISON=bison
OUT_NAME=splc
BUILDDIR=build
SRCS=syntax.tab compiler/Container compiler/main compiler/Node compiler/Specifier compiler/Scope compiler/SymbolTable
OBJS=$(SRCS:%=$(BUILDDIR)/%.o)

ADDRESS_SANITIZER = -O0 -g -fsanitize=address -fno-omit-frame-pointer
CXX_FLAGS = -std=c++17 -g
LD_FLAGS  = -lfl -ly

.DEFAULT_GOAL := release

syntax.tab.c: syntax.y lex.l
	@echo "[flex] $<"
	@$(FLEX) lex.l
	@echo "[bison] $<"
	@$(BISON) -d syntax.y --report all -Wcounterexamples -Wconflicts-sr -Wconflicts-rr

$(BUILDDIR)/%.o : %.c syntax.tab.c
	@echo "[gcc] $<"
	@$(CXX) $(CXX_FLAGS) -c $< -o $@

$(BUILDDIR)/%.o : %.cpp syntax.tab.c
	@echo "[g++] $<"
	@$(CXX) $(CXX_FLAGS) -c $< -o $@

$(OUT_NAME): $(OBJS)
	@mkdir -p bin
	@echo "[ld] linking splc"
	@$(CXX) $(LD_FLAGS) $(OBJS) -o bin/$(OUT_NAME)

release: $(OUT_NAME)

clean:
	@rm -rf lex.yy.* syntax.tab.* bin out *.out *.output
	@rm -rf test/*.out
	@rm -rf build
	@mkdir -p build
	@mkdir -p build/compiler

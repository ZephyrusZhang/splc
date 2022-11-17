CC=gcc
CXX=g++
FLEX=flex
BISON=bison
OUT_NAME=splc
BUILDDIR=build
SRCS=syntax.tab compiler/Container compiler/main compiler/Node compiler/Specifier compiler/Scope compiler/Dec compiler/Def compiler/Exp
OBJS=$(SRCS:%=$(BUILDDIR)/%.o)

ADDRESS_SANITIZER = -O0 -g -fsanitize=address -fno-omit-frame-pointer
CXXFLAGS := -g $(ADDRESS_SANITIZER)
CXXFLAGS += -std=c++20
LDFLAGS  = -lfl -ly $(ADDRESS_SANITIZER)

.DEFAULT_GOAL := release

bison: syntax.y lex.l
	@echo "[flex] $<"
	@$(FLEX) -o build/lex.yy.c lex.l
	@echo "[bison] $<"
	@$(BISON) -o build/syntax.tab.c -d syntax.y --report all -Wcounterexamples -Wconflicts-sr -Wconflicts-rr

build/syntax.tab.o: bison
	@echo "[g++] syntax.tab.c"
	@$(CXX) $(CXXFLAGS) -I. -c build/syntax.tab.c -o build/syntax.tab.o

$(BUILDDIR)/%.o : %.c bison
	@echo "[gcc] $<"
	@$(CXX) $(CXXFLAGS) -Ibuild -c $< -o $@

$(BUILDDIR)/%.o : %.cpp bison
	@echo "[g++] $<"
	@$(CXX) $(CXXFLAGS) -Ibuild -c $< -o $@

$(OUT_NAME): $(OBJS)
	@mkdir -p bin
	@echo "[ld] linking splc"
	@$(CXX) $(LDFLAGS) $(OBJS) -o bin/$(OUT_NAME)

release: $(OUT_NAME)

clean:
	@rm -rf lex.yy.* syntax.tab.* bin out *.out *.output
	@rm -rf test/*.out
	@rm -rf build
	@mkdir -p build
	@mkdir -p build/compiler

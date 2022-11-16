CC=gcc
CXX=g++
FLEX=flex
BISON=bison
OUT_NAME=splc
BUILDDIR=build
SRCS=syntax.tab compiler/Container compiler/main compiler/Node compiler/Specifier compiler/Scope compiler/Dec compiler/Def
OBJS=$(SRCS:%=$(BUILDDIR)/%.o)

ADDRESS_SANITIZER = -O0 -g -fsanitize=address -fno-omit-frame-pointer
CXXFLAGS := -g $(ADDRESS_SANITIZER)
CXXFLAGS += -std=c++20
LDFLAGS  = -lfl -ly $(ADDRESS_SANITIZER)

.DEFAULT_GOAL := release

syntax.tab.c: syntax.y lex.l
	@echo "[flex] $<"
	@$(FLEX) lex.l
	@echo "[bison] $<"
	@$(BISON) -d syntax.y --report all -Wcounterexamples -Wconflicts-sr -Wconflicts-rr

$(BUILDDIR)/%.o : %.c syntax.tab.c
	@echo "[gcc] $<"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILDDIR)/%.o : %.cpp syntax.tab.c
	@echo "[g++] $<"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

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

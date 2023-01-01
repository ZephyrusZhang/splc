CC=gcc
CXX=g++
FLEX=flex
BISON=bison
OUT_NAME=bin/splc
BUILDDIR=build
SOURCEDIR := compiler/
C_SOURCES := $(shell find $(SOURCEDIR) -name '*.c') syntax.tab.c
CPP_SOURCES := $(shell find $(SOURCEDIR) -name '*.cpp')
HEADERS := $(shell find $(SOURCEDIR) -name '*.h') $(shell find $(SOURCEDIR) -name '*.hpp')
OBJS=$(C_SOURCES:%.c=$(BUILDDIR)/%.o) $(CPP_SOURCES:%.cpp=$(BUILDDIR)/%.o)

ADDRESS_SANITIZER = -O0 -g -fsanitize=address -fno-omit-frame-pointer
CXXFLAGS := -g $(ADDRESS_SANITIZER)
CXXFLAGS += -std=c++20
LDFLAGS  = -lfl -ly $(ADDRESS_SANITIZER)

.DEFAULT_GOAL := release

build/lex.yy.c: lex.l $(HEADERS)
	@echo "[flex] $<"
	@$(FLEX) -o build/lex.yy.c lex.l

build/syntax.tab.c: syntax.y build/lex.yy.c $(HEADERS)
	@echo "[bison] $<"
	@$(BISON) -o build/syntax.tab.c -d syntax.y --report all -Wcounterexamples -Wconflicts-sr -Wconflicts-rr

build/syntax.tab.o: build/syntax.tab.c $(HEADERS)
	@echo "[g++] syntax.tab.c"
	@$(CXX) $(CXXFLAGS) -I. -c build/syntax.tab.c -o build/syntax.tab.o

$(BUILDDIR)/%.o : %.c build/syntax.tab.o $(HEADERS)
	@echo "[gcc] $<"
	@$(CXX) $(CXXFLAGS) -Ibuild -c $< -o $@

$(BUILDDIR)/%.o : %.cpp build/syntax.tab.o $(HEADERS)
	@echo "[g++] $<"
	@$(CXX) $(CXXFLAGS) -Ibuild -c $< -o $@

$(OUT_NAME): $(OBJS)
	@mkdir -p bin
	@echo "[ld] linking splc"
	@$(CXX) $(LDFLAGS) $(OBJS) -o $(OUT_NAME)

release: $(OUT_NAME)

clean:
	@rm -rf lex.yy.* syntax.tab.* bin out *.out *.output test/*.ir
	@rm -rf test/*.out
	@rm -rf build
	@mkdir -p build
	@mkdir -p build/compiler

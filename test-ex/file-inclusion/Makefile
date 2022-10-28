FLEX=flex
CXX=g++

main:
	$(FLEX) preprocess.l
	$(CXX) lex.yy.c -lfl -o splc-pre
clean:
	@rm -rf lex.yy.c pre main.out splc-pre

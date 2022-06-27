TARGET = tinytypescript

.PHONY: all
all: ${TARGET}

${TARGET}: ast.o ${TARGET}_parser.o ${TARGET}_lexer.o main.o
	g++ --std=c++11 -g -o $@ bin/ast.o bin/${TARGET}_parser.o bin/${TARGET}_lexer.o bin/main.o

ast.o: src/ast.cpp
	g++ -std=c++11 -g -c -o $@ $<
	mkdir -p build bin
	mv ast.o bin

main.o: src/main.cpp
	g++ --std=c++11 -g -c -o $@ $<
	mv main.o bin

${TARGET}_lexer.o: ${TARGET}_lexer.cpp
	g++ --std=c++11 -g -c -o $@ build/$<
	mv ${TARGET}_lexer.o bin

${TARGET}_lexer.cpp: src/${TARGET}.l
	flex -d -o $@ $<
	mv ${TARGET}_lexer.cpp build

${TARGET}_parser.o: ${TARGET}_parser.cpp
	g++ --std=c++11 -g -c -o $@ build/$<
	mv ${TARGET}_parser.o bin

${TARGET}_parser.cpp: src/${TARGET}.y
	bison --defines=tokens.h --debug -t -o $@ $<	
	mv tokens.h ${TARGET}_parser.cpp build

clean:
	rm -f *.o
	rm -f *.asm
	rm -f ${TARGET}
	rm -rf ./build
	rm -rf ./bin
	rm -rf ./*.dSYM
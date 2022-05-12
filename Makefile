TARGET = tinytypescript

.PHONY: all
all: ${TARGET}

${TARGET}: ${TARGET}_parser.o ${TARGET}_lexer.o main.o
	g++ --std=c++11 -g -o $@ bin/${TARGET}_parser.o bin/${TARGET}_lexer.o bin/main.o

main.o: src/main.cpp
	g++ --std=c++11 -g -c -o $@ $<
	mv main.o bin

${TARGET}_lexer.o: ${TARGET}_lexer.cpp
	g++ --std=c++11 -g -c -o $@ build/$<
	mv ${TARGET}_lexer.o bin

${TARGET}_lexer.cpp: src/${TARGET}.l
	flex -o $@ $<
	mv ${TARGET}_lexer.cpp build

${TARGET}_parser.o: ${TARGET}_parser.cpp
	g++ --std=c++11 -g -c -o $@ build/$<
	mv ${TARGET}_parser.o bin

${TARGET}_parser.cpp: src/${TARGET}.y
	mkdir -p build bin
	bison --defines=tokens.h -o $@ $<	
	mv tokens.h ${TARGET}_parser.cpp build

clean:
	rm -f *.o
	rm -f ${TARGET}
	rm -rf ./build
	rm -rf ./bin
	rm -rf ./*.dSYM
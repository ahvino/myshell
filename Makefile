CFLAGS = -Wall -Werror -pedantic -ansi

all: bin rshell

bin:
		[ ! -d $bin ] && mkdir bin

rshell:
		g++ $(CFLAGS) src/hw3.cpp -o ./bin/rshell


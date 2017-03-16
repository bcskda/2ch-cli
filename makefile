CC = gcc

all: default

default:
	mkdir -p log/
	$(CC) src/2ch-cli.c -std=c11 -Wno-format -fPIC -lcurl -lncursesw -o 2ch-cli
clean:
	rm -rf log/ 2ch-cli

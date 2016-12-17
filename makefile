CC = cc

all: default

default:
	mkdir -p log/
	$(CC) src/2ch-cli.c -d -std=c99 -fPIC -lcurl -lncursesw -o 2ch-cli
clean:
	rm -rf log/ 2ch-cli src/*.o

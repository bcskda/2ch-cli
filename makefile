CC = g++
LIBS = -lcurl -ljson -lncursesw -lcaca
OPTS= -std=c++11 -Wno-format -fPIC

all: default

default:
	mkdir -p log/
	$(CC)                src/2ch-cli.c ${OPTS} ${LIBS} -o 2ch-cli
captcha-test:
	$(CC) -DCAPTCHA_TEST src/2ch-cli.c ${OPTS}  ${LIBS} -o 2ch-cli
clean:
	rm -rf log/ 2ch-cli

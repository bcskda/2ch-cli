CC = gcc
LIBS = -lcurl -lncursesw -lcaca

all: default

default:
	$(CC) -std=c11       src/2ch-cli.c -std=c11 -Wno-format -fPIC ${LIBS} -o 2ch-cli
captcha-test:
	$(CC) -DCAPTCHA_TEST src/2ch-cli.c -std=c11 -Wno-format -fPIC ${LIBS} -o 2ch-cli
clean:
	rm -rf 2ch-cli

CC = gcc
LIBS = -lcurl -lncursesw -lcaca

all: default

default:
	mkdir -p log/
	#$(CC) src/2ch-cli.c -std=c11 -Wno-format -fPIC -lcurl -lncursesw -lcaca -o 2ch-cli
	$(CC)                src/2ch-cli.c -std=c11 -Wno-format -fPIC ${LIBS} -o 2ch-cli
captcha-test:
	$(CC) -DCAPTCHA_TEST src/2ch-cli.c -std=c11 -Wno-format -fPIC ${LIBS} -o 2ch-cli
posting-test:
	$(CC) -DPOSTING_TEST src/2ch-cli.c -std=c11 -Wno-format -fPIC ${LIBS} -o 2ch-cli
clean:
	rm -rf log/ 2ch-cli

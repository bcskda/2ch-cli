CC = g++
LIBS = -lm -lcurl -ljsoncpp -lncursesw -lcaca
OPTS = -std=c++11 -fPIC -Wno-pointer-arith
LOPTS = -Xlinker -z -Xlinker muldefs
DEBUG_OPTS= -g -ggdb
CURL_USERAGENT = "\"Linux x86_64\""
CONFIG = -DCURL_UA=$(CURL_USERAGENT) -DDEFAULT_EDITOR="\"nano\""
TARGETS = src/{2ch-cli.cpp,makaba.c,parser.cpp,external.cpp}

all: default

default: prepare link strip
config-test: prepare
	$(CC) -DCONFIG_TEST      $(TARGETS) ${CONFIG} ${LOPTS} ${OPTS} ${LIBS} -o 2ch-cli
captcha-test-cpp: prepare
	$(CC) -DCAPTCHA_TEST_CPP $(TARGETS) ${CONFIG} ${LOPTS} ${OPTS} ${LIBS} -o 2ch-cli

link: makaba.o parser.o external.o 2ch-cli.o
	$(CC) ${OPTS} ${LOPTS} ${LIBS} build/makaba.o build/parser.o build/external.o build/2ch-cli.o -o 2ch-cli
makaba.o:
	$(CC) src/makaba.c     ${CONFIG} ${OPTS} -c -o build/makaba.o
parser.o:
	$(CC) src/parser.cpp   ${CONFIG} ${OPTS} -c -o build/parser.o
external.o:
	$(CC) src/external.cpp ${CONFIG} ${OPTS} -c -o build/external.o
2ch-cli.o:
	$(CC) src/2ch-cli.cpp  ${CONFIG} ${OPTS} -c -o build/2ch-cli.o
strip:
	strip --strip-unneeded 2ch-cli

prepare:
	[ -d build ] || mkdir build
clean:
	rm -rf 2ch-cli build-log build

CC = g++
LIBS = -lm -lcurl -ljsoncpp -lncursesw -lcaca
OPTS = -std=c++11 -Wno-format -fPIC
LOPTS = -Xlinker -z -Xlinker muldefs
DEBUG_OPTS= -g -ggdb
CURL_USERAGENT = "\"Linux x86_64\""
CONFIG = -DCURL_UA=$(CURL_USERAGENT) -DDEFAULT_EDITOR="\"nano\""
TARGETS = src/{2ch-cli.cpp,makaba.c,parser.cpp,external.cpp,external.cpp}

all: default

release:
	$(CC)                    $(TARGETS) ${CONFIG} ${LOPTS} ${OPTS} ${LIBS} -o 2ch-cli 2>build-log
default:
	$(CC)      $(DEBUG_OPTS) $(TARGETS) ${CONFIG} ${LOPTS} ${OPTS} ${LIBS} -o 2ch-cli
config-test:
	$(CC) -DCONFIG_TEST      $(TARGETS) ${CONFIG} ${LOPTS} ${OPTS} ${LIBS} -o 2ch-cli
captcha-test-cpp:
	$(CC) -DCAPTCHA_TEST_CPP $(TARGETS) ${CONFIG} ${LOPTS} ${OPTS} ${LIBS} -o 2ch-cli
clean:
	rm -f 2ch-cli build-log

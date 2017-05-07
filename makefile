CC = g++
LIBS = -lm -lcurl -ljsoncpp -ljson -lncursesw -lcaca
OPTS = -std=c++11 -Wno-format -fPIC
LOPTS = -Xlinker -z -Xlinker muldefs
CURL_USERAGENT = "\"Linux x86_64\""
CONFIG = -DCURL_UA=$(CURL_USERAGENT)
TARGETS = src/{2ch-cli.cpp,makaba.c,parser.cpp,image.c}

all: default

release:
	$(CC)                    $(TARGETS) ${CONFIG} ${LOPTS} ${OPTS} ${LIBS} -o 2ch-cli 2>build-log
default:
	$(CC)                    $(TARGETS) ${CONFIG} ${LOPTS} ${OPTS} ${LIBS} -o 2ch-cli
config-test:
	$(CC) -DCONFIG_TEST      $(TARGETS) ${CONFIG} ${LOPTS} ${OPTS} ${LIBS} -o 2ch-cli
captcha-test-cpp:
	$(CC) -DCAPTCHA_TEST_CPP $(TARGETS) ${CONFIG} ${LOPTS} ${OPTS} ${LIBS} -o 2ch-cli
clean:
	rm -f 2ch-cli build-log

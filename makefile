CC = g++
LIBS = -lcurl -ljson -lncursesw -lcaca
OPTS = -std=c++11 -Wno-format -fPIC
CURL_USERAGENT = "\"Linux x86_64\""
CONFIG = -DCURL_UA=$(CURL_USERAGENT)
MAIN=src/2ch-cli.cpp

all: default

default:
	mkdir -p log/
	$(CC)                    $(MAIN) ${CONFIG} ${OPTS} ${LIBS} -o 2ch-cli
config-test:
	$(CC) -DCONFIG_TEST      $(MAIN) ${CONFIG} ${OPTS} ${LIBS} -o 2ch-cli
captcha-test-cpp:
	$(CC) -DCAPTCHA_TEST_CPP $(MAIN) ${CONFIG} ${OPTS} ${LIBS} -o 2ch-cli
clean:
	rm -rf log/ 2ch-cli

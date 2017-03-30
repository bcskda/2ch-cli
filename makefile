CC = g++
LIBS = -lcurl -ljson -lncursesw -lcaca
OPTS = -std=c++11 -Wno-format -fPIC
CURL_USERAGENT = "\"Linux x86_64\""
CONFIG = -DCURL_UA=$(CURL_USERAGENT)

all: default

default:
	mkdir -p log/
	$(CC)                    src/2ch-cli.c ${CONFIG} ${OPTS} ${LIBS} -o 2ch-cli
config-test:
	$(CC) -DCONFIG_TEST      src/2ch-cli.c ${CONFIG} ${OPTS} ${LIBS} -o 2ch-cli
captcha-test-cpp:
	$(CC) -DCAPTCHA_TEST_CPP src/2ch-cli.c ${CONFIG} ${OPTS} ${LIBS} -o 2ch-cli
clean:
	rm -rf log/ 2ch-cli

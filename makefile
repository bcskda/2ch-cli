default:
	if [ ! -d build ]; then mkdir build; fi
	if [ ! -d log ]; then mkdir log; fi
	gcc src/2ch-cli.c -std=c99 -d -fdump-rtl -fPIC -lcurl -lncursesw -o build/2ch-cli \
		2>"log/build-log"
clean:
	rm -r build/ log/

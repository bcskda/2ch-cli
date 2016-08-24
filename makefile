default:
	if [ ! -d build ]; then mkdir build; fi
	gcc src/2ch-cli.c -std=c99 -fPIC -lcurl -lncurses -o build/makaba \
		2>"build/build-log"
clean:
	rm -r build

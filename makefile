default:
	if [ ! -d build ]; then mkdir build; fi
	gcc src/2ch-cli.c -std=c99 -d -fdump-rtl -fPIC -lcurl -lncursesw -o build/2ch-cli \
		2>"build/build-log"
clean:
	rm -r build

default:
	mkdir -p log/
	gcc src/2ch-cli.c -std=gnu99 -d -fdump-rtl -fPIC -lcurl -lncursesw -o 2ch-cli 
clean:
	rm -rf log/ 2ch-cli

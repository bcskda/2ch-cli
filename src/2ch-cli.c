// ========================================
// File: 2ch-cli.c
// A CLI-client for 2ch.hk imageboard written on C
// (Implementation)
// ========================================

#include "2ch-cli.h"
	//инклады в хедере
void pomogite()
{
        printf("2ch-cli v0.3 - консольный клиент двача\n");
        printf("Использование:\n");
        printf(" -h - помощь\n");
        printf(" -s - запуск\n");
        printf(" -p - задать пасскод\n");
}

int main (int argc, char **argv)
{
	setlocale (LC_ALL, "");

	//getopt
	int opt; //man 3 getopt
	char *optarg;

	while (( opt = getopt(argc, argv, "hp:s") ) != -1) //пока то что в скобках не равно -1
	{
		switch (opt)
		{
			case 'p':
				strcpy(passcode, optarg);
				printf("Разраб хуй, ещё не запилил\n");
				return 0;
			case 's':
				break;
			default:
				pomogite();
				return 0;
		}
	}
	if (opt == -1) {
		pomogite(); //если анон запустил прогу без аргументов, то пишет помощь.
		return -1;
	}

	makabaSetup();
	char* thread_ch = (char*) calloc (Thread_size, sizeof(char));
	if (thread_ch == NULL) {
		fprintf(stderr, "[main]! Error: 'thread_ch' memory allocation\n");
		return ERR_MEMORY;
	}
	thread_ch = memcpy(
					thread_ch,
					getThreadJSON ("abu", 42375, false),
					Thread_size
					);
	struct thread* thread = initThread(thread_ch, strlen(thread), true);

	initscr();
	raw();
	keypad (stdscr, TRUE);
	noecho();
	printw ("Push [c] to clear screen, anything else to print another post\n");
	for (int i = 0; i < thread->nposts; i++) {
		bool done = 0;
		while (! done)
			switch (getch()) { 
				case 'C': case 'c':
					clear();
					printw ("Push [c] to clear screen, anything else to print another post\n");
					break;
				default:
					fprintf(stderr, "Printing post #%d\n", i);
					printPost(thread->posts[i], true, true);
					refresh();
					done = 1;
					break;
			}
	}
	printw("\nPush a key to exit\n");
	getch();
	endwin();
	
	freeThread(thread);
	free(thread_ch);
	makabaCleanup();
	fprintf(stderr, "Cleanup done, exiting\n");

	return 0;
}


int printPost (struct post* post,const bool show_email,const bool show_files) {
	if (show_email && (post->email != NULL)) {
		printw ("[=== %s (%s) #%d %s ===]\n%s\n\n",
			post->name, post->email, post->num, post->date, post->comment->text);
	} else {
		printw ("[=== %s #%d %s ===]\n%s\n\n",
			post->name, post->num, post->date, post->comment->text);
	}
	return 0;
}

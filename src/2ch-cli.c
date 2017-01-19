// ========================================
// File: 2ch-cli.c
// A CLI-client for 2ch.hk imageboard written on C
// (Implementation)
// ========================================

#include "2ch-cli.h"

void pomogite() // Справка
{
	printf("2ch-cli "VERSION" - консольный клиент двача\n");
	printf("Использование:\n");
	printf(" -h - помощь\n");
	printf(" -s - запуск\n");
	printf(" -p - задать пасскод\n");
	printf(" -b - задать борду\n");
	printf(" -n - заранее задать номер поста\n");
}

int main (int argc, char **argv)
{
    char passcode[32] = "пасскода нет нихуя :("; // Пасскод
    char board_name[10] = "b"; // Имя борды
    long long post_number = 0; // Номер треда в борде

	//getopt
	int opt;
	bool start_or_not = false; // Проверяем, есть ли '-s' и нужно ли запускать

	if (argc == 1) /* Если аргументов нет, вывод помощи */ {
		pomogite();
		return RET_NOARGS;
	}
	while (( opt = getopt(argc, argv, "hp:b:n:s") ) != -1)
	{
		switch (opt)
		{
			case 'p':
				memset(passcode, '\0', sizeof(passcode));
                if ( sizeof(optarg) > sizeof(passcode) ) { //проверка
					printf("Не шути так больше\n");
					return ERR_ARGS;
				}
				memcpy(passcode, optarg, sizeof(passcode));
                printf("Разраб хуй, ещё не запилил\n");
				printf("Уже что-то могу: %s!\n", passcode);
				break;
			case 'b':
				memset(board_name, '\0', sizeof(board_name));
                if ( sizeof(optarg) > sizeof(board_name) ) { //проверка
					printf("Не шути так больше\n");
					return ERR_ARGS;
				}
				memcpy(board_name, optarg, sizeof(board_name));
				break;
			case 'n':
				post_number = atoi(optarg);
				break;
			case 's':
				start_or_not = true;
				break;
			default:
				pomogite();
				return ERR_ARGS;
		}
	}

	if (start_or_not == false) return RET_OK;

	setlocale (LC_ALL, "");

	makabaSetup();
	
	//printf( "Testing: %s\n", get2chaptchaPicURL(parse2chaptchaId(get2chaptchaIdJSON("abu","49946",true),true),true) );
	//printf( "Testing: %s\n", getCaptchaSettingsJSON("abu",true) );

	//return RET_PREEXIT;


	long int threadsize = 0;
	char* thread_recv_ch = getThreadJSON (board_name, post_number, &threadsize, true); // Получаем указатель на скачанный тред
	fprintf(stderr, "threadsize = %u\n", threadsize);
	char* thread_ch = (char*) calloc (threadsize+1, sizeof(char)); // Заказываем память под собственный буфер треда
	if (thread_ch == NULL) {
		fprintf(stderr, "[main]! Error: 'thread_ch' memory allocation\n");
		return ERR_MEMORY;
	}
	thread_ch = memcpy( // Копируем скачанный тред из буфера загрузок, т.к. хранить там ненадежно
					thread_ch, 
					thread_recv_ch,
					threadsize
					);
	thread_ch[threadsize] = 0;
	fprintf(stderr, "thread_ch = %p\n", thread_ch);

	/*
	printf("%c%c%c%c%c%c%c%c%c%c\n",
			thread_ch[threadsize-10],
			thread_ch[threadsize-9 ],
			thread_ch[threadsize-8 ],
			thread_ch[threadsize-7 ],
			thread_ch[threadsize-6 ],
			thread_ch[threadsize-5 ],
			thread_ch[threadsize-4 ],
			thread_ch[threadsize-3 ],
			thread_ch[threadsize-2 ],
			thread_ch[threadsize+1 ]);
	*/

	FILE *thread_o = fopen( "log/thread_o", "w" );
	fprintf(thread_o, "%s\n", thread_ch);
	fprintf(stderr, "Get OK\n");

	struct thread* thread = initThread( thread_ch, threadsize, true );

	fprintf(stderr, "Init OK\n");

	initscr();
	raw();
	keypad (stdscr, TRUE);
	noecho();
	printw ("Push [c] to clear screen, [q] to exit, anything else to print another post\n");
	for (int i = 0; i < thread->nposts; i++) {
		bool done = 0;
		while (! done)
			switch (getch())
			{ 
				case 'C': case 'c':
					clear();
					printw ("Push [c] to clear screen, [q] to exit, anything else to print another post\n");
					break;
				case 'Q': case 'q':
					done = 1;
					i = thread->nposts;
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

	return RET_OK;
}


int printPost (struct post* post,const bool show_email,const bool show_files) {
	if (post->comment == NULL) {
		fprintf(stderr, "! ERROR @printPost: Null comment in struct post\n");
		return ERR_BROKEN_POST;
	}
	if (post->comment->text == NULL) {
		fprintf(stderr, "! ERROR @printPost: Null text in struct comment\n");
		return ERR_BROKEN_POST;
	}
	if (post->num == NULL) {
		fprintf(stderr, "! ERROR @printPost: Null num in struct post\n");
		return ERR_BROKEN_POST;
	}
	if (post->date == NULL) {
		fprintf(stderr, "! ERROR @printPost: Null date in struct post\n");
		return ERR_BROKEN_POST;
	}

	if (show_email && (post->email != NULL)) {
		printw ("[=== %s (%s) #%d %s ===]\n%s\n\n",
			post->name, post->email, post->num, post->date, post->comment->text);
	}
	else {
		printw ("[=== %s #%d %s ===]\n%s\n\n",
			post->name, post->num, post->date, post->comment->text);
	}
	return 0;
}

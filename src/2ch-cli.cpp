// ========================================
// File: 2ch-cli.cpp
// A CLI-client for 2ch.hk imageboard written on C/C++
// (Implementation)
// ========================================

#include "2ch-cli.h"

void pomogite() // Справка
{
	printf("2ch-cli " VERSION " - консольный клиент двача\n");
	printf("Использование:\n");
	printf(" -h - помощь\n");
	printf(" -s - отправить пост\n");
	printf(" -p - задать пасскод\n");
	printf(" -b - задать борду\n");
	printf(" -n - задать номер треда\n");
	printf(" -c - задать комментарий\n");
}

int main (int argc, char **argv)
{
	freopen("/tmp/2ch-cli.log", "w", stderr);

	#ifdef CONFIG_TEST
	printf("CURL_UA = \"" CURL_UA "\"\n");
	return RET_PREEXIT;
	#endif

	char passcode[32] = "пасскода нет :("; // Пасскод
    char board_name[10] = "b"; // Имя борды
    long long thread_number = 0; // Номер треда в борде
	char *comment = NULL; // Комментарий - не занимать память, если не указан

	//getopt
	bool send_post = false;
	if (argc == 1) /* Если аргументов нет, вывод помощи */ {
		pomogite();
		return RET_ARGS;
	}
	parse_argv(argc, (const char **)argv, board_name, &thread_number, &comment, passcode, &send_post);
	printf("board_name = %s\n", board_name);
	printf("comment = %s\n", comment);

	setlocale (LC_ALL, "");
	makabaSetup();

	#ifdef CAPTCHA_TEST_CPP
	makaba_2chaptcha captcha;
	prepareCaptcha_cpp(captcha, board_name, thread_number);
	printf("id = \"%s\"\nresult = \"%d\"\nurl = \"%s\"\n",
		captcha.id, captcha.result, captcha.png_url);
	char shcmd[40];
	sprintf(shcmd, "cat %s", CaptchaUtfFilename);
	system(shcmd);
	return RET_PREEXIT;
	#endif

	if (send_post == true) {
		makaba_2chaptcha captcha;
		if (prepareCaptcha_cpp(captcha, board_name, thread_number)) {
			fprintf(stderr, "[main] ! Error @ prepareCaptcha_cpp\n");
			return RET_INTERNAL;
		}
		char shcmd[40];
		sprintf(shcmd, "cat %s", CaptchaUtfFilename);
		system(shcmd);
		printf("Ответ на капчу: ");
		scanf("%s", captcha.value);
		long long answer_length;
		char email[] = "";
		sendPost(board_name, lint2str(thread_number),
			comment, NULL, NULL, email,
			captcha.id, captcha.value, &answer_length);
		printf("Ответ API: %s\n", CURL_BUFF_BODY); // По-хорошему так делать не надо
		makabaCleanup();
		return RET_OK;
	}


	long int threadsize = 0;
	char *thread_recv_ch = getThreadJSON(board_name, thread_number, &threadsize, false); // Получаем указатель на скачанный тред
	fprintf(stderr, "threadsize = %ld\n", threadsize);
	char *thread_ch = (char *) calloc(threadsize + 1, sizeof(char)); // Заказываем память под собственный буфер треда
	if (thread_ch == NULL) {
		fprintf(stderr, "[main]! Error: 'thread_ch' memory allocation\n");
		return RET_MEMORY;
	}
	// Копируем скачанный тред из буфера загрузок, т.к. хранить там ненадежно
	thread_ch = (char *) memcpy(thread_ch, thread_recv_ch, threadsize);
	thread_ch[threadsize] = 0;
	fprintf(stderr, "Get OK\n");

	makaba_thread_cpp thread;
	if (initThread_cpp(thread, thread_ch, threadsize, true)) {
		fprintf(stderr, "[main] ! Error @ initThread_cpp()\n");
		free(thread_ch);
		return RET_PARSE;
	};
	free(thread_ch);

	ncurses_init();
	ncurses_print_help();
	bool should_exit = false;
	ncurses_print_post(thread, 0);
	for (int cur_post = 0; should_exit == false; ) {
		bool done = 0;
		while (done == false)
			switch (getch())
			{
				case 'Q': case 'q':
					done = 1;
					should_exit = true;
					break;
				case 'P': case 'p':
					printw(">>> Открой капчу в другом терминале. Прости.\n");
					break;
				case 'H': case 'h':
					ncurses_print_help();
					break;
				case KEY_RIGHT: case KEY_DOWN:
					if (cur_post < thread.nposts - 1) {
						cur_post ++;
						ncurses_print_post(thread, cur_post);
					}
					else {
						printw(">>> Последний пост\n");
					}
					done = 1;
					break;
				case KEY_LEFT: case KEY_UP:
					if (cur_post > 0) {
						cur_post --;
						ncurses_print_post(thread, cur_post);
					}
					else {
						printw(">>> Первый пост\n");
					}
					done = 1;
					break;
				case KEY_NPAGE: // PageDown
					if (cur_post < thread.nposts - 1) {
						cur_post += Skip_on_PG;
						if (cur_post > thread.nposts - 1)
							cur_post = thread.nposts - 1;
						ncurses_print_post(thread, cur_post);
					}
					break;
				case KEY_PPAGE: // PageUp
					if (cur_post > 0) {
						cur_post -= Skip_on_PG;
						if (cur_post < 0)
							cur_post = 0;
						ncurses_print_post(thread, cur_post);
					}
					break;
				case KEY_HOME:
					if (cur_post > 0) {
						cur_post = 0;
					ncurses_print_post(thread, cur_post);
					}
					break;
				case KEY_END:
					if (cur_post < thread.nposts - 1) {
						cur_post = thread.nposts - 1;
						ncurses_print_post(thread, cur_post);
					}
					break;
			}
	}
	ncurses_exit();

	freeThread(thread);
	makabaCleanup();
	if (comment != NULL)
		free(comment);
	fprintf(stderr, "Cleanup done, exiting\n");

	return RET_OK;
}

int printPost (const makaba_post_cpp &post, const bool show_email, const bool show_files) {
	if (post.comment == NULL) {
		fprintf(stderr, "! ERROR @printPost: Null comment in struct post\n");
		makaba_errno = ERR_POST_FORMAT;
		return 1;
	}
	if (post.num == NULL) {
		fprintf(stderr, "! ERROR @printPost: Null num in struct post\n");
		makaba_errno = ERR_POST_FORMAT;
		return 1;
	}
	if (post.date == NULL) {
		fprintf(stderr, "! ERROR @printPost: Null date in struct post\n");
		makaba_errno = ERR_POST_FORMAT;
		return 1;
	}

	// Заголовок отдельно
	if (show_email == true && strlen(post.email) > 0) {
		printw ("[=== %s (%s) #%d %s ===]\n",
			post.name, post.email, post.num, post.date);
	}
	else {
		printw ("[=== %s #%d %s ===]\n",
			post.name, post.num, post.date);
	}
	// Комментарий
	printw("%s\n\n", post.comment);

	return 0;
}

int prepareCaptcha_cpp (makaba_2chaptcha &captcha, const char *board, const long long thread) {
	if (initCaptcha_cpp(captcha, board, thread)) {
		fprintf(stderr, "[prepareCaptcha_cpp] ! Error @ initCaptcha_cpp: %d\n", makaba_errno);
		return 1;
	}

	long int pic_size;
	char *captcha_png = get2chaptchaPicPNG(captcha.png_url, &pic_size);
	if (captcha_png == NULL) {
		fprintf(stderr, "[prepareCaptcha_cpp] ! Error @ get2chaptchaPicPNG: %d\n", makaba_errno);
		return 1;
	}

	FILE *captcha_png_file = fopen(CaptchaPngFilename, "w");
	fwrite(captcha_png, sizeof(char), pic_size, captcha_png_file);
	fclose(captcha_png_file);
	convert_img(CaptchaPngFilename, CaptchaUtfFilename, true);

	return 0;
}

void parse_argv(const int argc, const char **argv,
	char *board_name, long long *thread_number, char **comment, char *passcode, bool *send_post)
{
	int opt;
	while (( opt = getopt(argc, (char * const *)argv, "hp:b:n:sc:") ) != -1)
	{
		switch (opt)
		{
			case 'p':
				memset(passcode, '\0', sizeof(passcode));
                if ( sizeof(optarg) > sizeof(passcode) ) { //проверка
					printf("Не шути так больше\n");
					exit(RET_ARGS);
				}
				memcpy(passcode, optarg, sizeof(passcode));
                printf("Разраб хуй, ещё не запилил пасскоды\n");
				printf("Уже что-то могу: %s!\n", passcode);
				break;
			case 'b':
				memset(board_name, '\0', sizeof(board_name));
                if ( sizeof(optarg) > sizeof(board_name) ) { //проверка
					printf("Не шути так больше\n");
					exit(RET_ARGS);
				}
				memcpy(board_name, optarg, sizeof(board_name));
				break;
			case 'n':
				*thread_number = atoi(optarg);
				break;
			case 'c':
				if (*comment == NULL) {
					if ( sizeof(optarg) > Max_comment_len ) {
						printf("Комментарий не длиннее 15к знаков\n");
						exit(RET_ARGS);
					}
					printf("[%d] %s\n", strlen(optarg), optarg);
					*comment = (char *) calloc(strlen(optarg) + 1, sizeof(char));
					memcpy(*comment, optarg, strlen(optarg));
				}
				else {
					printf("Дважды указан комментарий\n");
					exit(RET_ARGS);
				}
				break;
			case 's':
				*send_post = true;
				break;
			default:
				printf("Неизвестная опция %c\n", opt);
				pomogite();
				exit(RET_ARGS);
		}
	}
}

void ncurses_init() {
	initscr();
	raw();
	keypad (stdscr, TRUE);
	noecho();
}

void ncurses_exit() {
	endwin();
}

void ncurses_print_help() {
	printw(">>> [LEFT] / [UP] предыдущий пост, [RIGHT] / [DOWN] следующий пост\n");
	printw(">>> [PageUp] - %d постов назад, [PageDown] - %d постов вперёд\n", Skip_on_PG, Skip_on_PG);
	printw(">>> [Home] - первый пост, [End] - последний пост\n");
	printw(">>> [H] помощь, [Q] выход\n");
}

void ncurses_print_post(const makaba_thread_cpp &thread, const int postnum) {
	clear();
	fprintf(stderr, "Printing post #%d\n", postnum);
	printPost(thread.posts[postnum], true, true);
	refresh();
}

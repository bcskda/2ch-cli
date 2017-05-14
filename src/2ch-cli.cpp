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
	printf(" -b - задать борду\n");
	printf(" -n - задать номер треда\n");
	printf(" -s - отправить пост\n");
	printf(" -c - задать комментарий\n");
	printf(" -C - очистить кэш тредов\n");
	printf(" -v - подробный лог (для разработчиков)\n");
	printf(" -p - задать пасскод (не поддерживается)\n");
}

int main (int argc, const char **argv)
{
	freopen("/tmp/2ch-cli.log", "w", stderr);

	#ifdef CONFIG_TEST
	printf("CURL_UA = \"" CURL_UA "\"\n");
	return RET_PREEXIT;
	#endif

	std::string passcode = "пасскода нет :("; // Пасскод
    std::string board = "b"; // Имя борды
    long long thread_number = 0; // Номер треда в борде
	std::string comment = "";
	bool send_post = false;
	bool verbose = false;
	bool clean_cache = false;

	//getopt
	if (argc == 1) /* Если аргументов нет, вывод помощи */ {
		pomogite();
		return RET_ARGS;
	}
	parse_argv(argc, argv,
		board, thread_number, comment, passcode,
		send_post, verbose, clean_cache);
	fprintf(stderr, "board_name = %s\n", board.data());
	fprintf(stderr, "comment = %s\n", comment.data());
	if (clean_cache == true) {
		if (cleanJsonCache()) {
			fprintf(stderr, "[main] ! Error @ cleanJsonCache()\n");
			return RET_INTERNAL;
		}
		else {
			printf("Кэш тредов очищен\n");
		}
		return RET_OK;
	}

	setlocale (LC_ALL, "");
	makabaSetup();
	if (initJsonCache() == -1) {
		fprintf(stderr, "[main]! Error @ initJsonCache(): %d\n",
			makaba_errno);
		return RET_INTERNAL;
	}

	#ifdef CAPTCHA_TEST_CPP
	makaba_2chaptcha captcha(board, thread_number);
	captcha.get_png();
	char shcmd[40];
	sprintf(shcmd, "cat %s", CaptchaUtfFilename);
	system(shcmd);
	return RET_PREEXIT;
	#endif

	if (send_post == true) {
		makaba_post post(comment.data(), "",
						 "", "",
						 "", "");
		if (sendPost(post, board, thread_number)) {
			fprintf(stderr, "[main] Error: sendPost failed: %d\n", makaba_errno);
			return RET_INTERNAL;
		}
		return RET_OK;
	}

	makaba_thread thread(board, thread_number);
	if (makaba_errno) {
		switch(makaba_errno) {
			case ERR_CURL_PERFORM:
				printf("Ошибка соединения с сервером\n");
                break;
			case ERR_JSON_PARSE:
				printf("Ошибка обработки ответа сервера\n");
                break;
		}
		makabaCleanup();
		return RET_INTERNAL;
	}

	if (thread.nposts == 0) {
		printf(">>> Smth strange with thread at %s/%lld: doen\'t exist, exiting\n",
			board.data(), thread_number);
		makabaCleanup();
		return RET_OK;
	}

	ncurses_init();
	ncurses_print_help();
	bool should_exit = false;
	ncurses_print_post(thread, 0);
	int ret = RET_OK;
	makaba_post dummy_post("", "",
						   "", "",
						   "", "");
	dummy_post.email = "sage";

	std::string api_result;

	for (int cur_post = 0; should_exit == false; ) {
		bool done = 0;
		int int_input = 0;
		while (done == false) {
			int nposts_old = thread.nposts;
			switch (getch())
			{
				case 'Q': case 'q':
					done = 1;
					should_exit = true;
					break;
				case 'c':
					ncurses_exit();
					edit(dummy_post.comment, Task_comment);
					fprintf(stderr, "[main] Note: fork-read comment \"%s\"\n",
							dummy_post.comment.data());
					ncurses_init();
					ncurses_print_post(thread, cur_post);
					break;
				case 's':
					if (dummy_post.email.length()) {
						dummy_post.email = "";
						ncurses_print_error("Sage: off");
					}
					else {
						dummy_post.email = "sage";
						ncurses_print_error("Sage: on");
					}
					break;
				case 'S':
					ncurses_exit();
					api_result = sendPost(dummy_post, thread.board, thread.num);
					ncurses_init();
					ncurses_print_post(thread, cur_post);
					ncurses_print_error(api_result.data());
					break;
				case 'h':
					ncurses_print_help();
					break;
				case 'C':
					ncurses_print_post(thread, cur_post);
					break;
				case 'G':
					printw("Перейти к посту: ");
					refresh();
					echo();
					scanw("%d", &int_input);
					noecho();
					if (int_input < 1) {
						int_input = 1;
					}
					if (int_input > thread.nposts) {
						int_input = thread.nposts;
					}
					cur_post = int_input - 1;
					ncurses_print_post(thread, cur_post);
					break;
				case 'u':
					printw(">>> Обновление треда ...");
					refresh();
					if (thread.update()) {
						fprintf(stderr, "[main] Error @ thread::update()\n");
						printw(" ошибка\n");
						ncurses_print_error(makaba_strerror(makaba_errno));
					}
					else {
					printw(" готово, %d новых постов\n",
						thread.nposts - nposts_old);
					}
					refresh();
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
	}
	ncurses_exit();

	disarmJsonCache(thread);
	makabaCleanup();
	fprintf(stderr, "Cleanup done, exiting\n");

	return ret;
}

const char *sendPost(const makaba_post &post,
					 const std::string &board,
					 const long long &threadnum)
{
	makaba_2chaptcha captcha(board, threadnum);
	if (captcha.isNull()) {
		fprintf(stderr, "[main] Error: "
						"captcha_2chaptcha::captcha_2chaptcha(const std::string &, const long long &)\n"
						"  error = %d\n"
						"  description = %s\n", makaba_errno, makaba_strerror(makaba_errno));
		return NULL;
	}
	if (captcha.get_png()) {
		fprintf(stderr, "[main] Error: captcha_2chaptcha::get_png()\n"
						"  error = %d\n"
						"  description = %s\n", makaba_errno, makaba_strerror(makaba_errno));
		return NULL;
	}

	caca_display_t *display = show_img(CaptchaUtfFilename);
	caca_canvas_t *canvas = caca_get_canvas(display);
	caca_put_str(canvas,
				 1, Converter_height_i,
			     "Ответ на капчу (секурность уровня sudo): ");
	caca_refresh_display(display);
	std::cin >> captcha.value;
	caca_free_display(display);

	return sendPost(board.data(), threadnum,
		post.comment.data(), post.subject.data(), post.name.data(), post.email.data(),
		captcha.id.data(), captcha.value.data());
}

int printThreadHeader(const makaba_thread &thread)
{
	move(Head_pos_y, Head_pos_x);
	printw("%s", Headers_pref);
	printw("/%s %s, %ld постов\n",
		thread.board.data(), thread.posts[0].subject.data(), thread.nposts);
	return 0;
}

int printPost (const makaba_post &post, const bool show_email, const bool show_files) {
	if (post.comment.length() == 0) {
		fprintf(stderr, "! ERROR @printPost: Null comment in struct post\n");
		makaba_errno = ERR_POST_FORMAT;
		return 1;
	}
	if (post.date.length() == 0) {
		fprintf(stderr, "! ERROR @printPost: Null date in struct post\n");
		makaba_errno = ERR_POST_FORMAT;
		return 1;
	}

	char header_1[150] = "";
	char header_2[150] = "";
	sprintf(header_1, "%s", post.name.data());
	bool sage = false;
	if (show_email == true && post.email.length() > 0) {
		if (strcmp(post.email.data(), "mailto:sage"))
			sprintf(header_1, "%s @ %s", header_1, post.email.data());
		else
			sage = true;
	}
	sprintf(header_2, "№%lld (%lld) %s", post.num, post.rel_num, post.date.data());
	// @TODO Выравнивать строки заголовка по ширине
	printw("%s", Headers_pref);
	if (sage)
		attron(A_UNDERLINE);
	printw("%s\n", header_1);
	attroff(A_UNDERLINE);
	printw("%s", Headers_pref);
	printw("%s\n", header_2);
	// Выводим комментарий
	printw("\n%s\n\n", post.comment.data());

	return 0;
}

void ncurses_init() {
	initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();
}

void ncurses_exit() {
	endwin();
}

void ncurses_print_help() {
	printw("\n"
	       ">>> u - обновить тред\n"
	       ">>> s - вкл/выкл sage\n"
	       ">>> c - изменить текст поста\n"
	       ">>> S - отправить пост\n"
	       "\n"
	       ">>> [LEFT] / [UP] предыдущий пост, [RIGHT] / [DOWN] следующий пост\n"
	       ">>> [PageUp] - %d постов назад, [PageDown] - %d постов вперёд\n"
	       ">>> [Home] - первый пост, [End] - последний пост\n"
	       ">>> G - перейти по отн. номеру поста\n"
	       "\n"
	       ">>> C - очистить экран\n"
	       ">>> h - помощь, q - выход\n\n",
	       Skip_on_PG, Skip_on_PG);
}

void ncurses_print_post(const makaba_thread &thread, const long long num) {
	clear();
	printThreadHeader(thread);
	printPost(thread.posts[num], true, true);
	refresh();
}

void ncurses_print_error(const char *mesg) {
	int oldx = -1, oldy = -1;
	getyx(stdscr, oldy, oldx);
	attron(A_STANDOUT);
	mvprintw(Err_pos_y , Err_pos_x, mesg);
	attroff(A_STANDOUT);
	move(oldy, oldx);
}

void ncurses_clear_error() {
	
}

void parse_argv(const int argc, const char **argv,
	std::string &board, long long &thread_number, std::string &comment, std::string &passcode,
	bool &send_post, bool &verbose, bool &clean_cache)
{
	int opt;
	while (( opt = getopt(argc, (char * const *)argv, "hp:b:n:sc:vC") ) != -1)
	{
		switch (opt)
		{
			case 'p':
				passcode = optarg;
                printf("Разраб ещё не запилил пасскоды\n");
				printf("Уже что-то могу: %s!\n", passcode.data());
				break;
			case 'b':
				if (strlen(optarg) > 20) {
					printf("Толсто.\n");
					exit(RET_ARGS);
				}
				board = optarg;
				break;
			case 'n':
				thread_number = atoi(optarg);
				break;
			case 'c':
				if (comment.length() == 0) {
					if ( sizeof(optarg) > COMMENT_LEN_MAX ) {
						printf("Комментарий не длиннее 15к символов\n");
						exit(RET_ARGS);
					}
					comment = optarg;
				}
				else {
					printf("Дважды указан комментарий\n");
					exit(RET_ARGS);
				}
				break;
			case 's':
				send_post = true;
				break;
			case 'v':
				verbose = true;
				break;
			case 'C':
				clean_cache = true;
				break;
			default:
				printf("Неизвестная опция %c\n", opt);
				pomogite();
				exit(RET_ARGS);
		}
	}
}

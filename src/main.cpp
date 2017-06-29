// ========================================
// File: main.cpp
// Main
// ========================================

#include "error.h"
#include "makaba.h"
#include "cache.h"
#include "app.h"
#include <locale.h>
#include <getopt.h>

const int RET_OK = 0;
const int RET_ARGS = 1;
const int RET_PREEXIT = 2;
const int RET_MEMORY = 3;
const int RET_PARSE = 4;
const int RET_INTERNAL = 5; 


int main (int argc, const char **argv)
{
	freopen("/tmp/2ch-cli.log", "w", stderr);
	std::cerr << "[main] Makaba::NullThread at " << &Makaba::NullThread << std::endl;
	
	std::string passcode = "пасскода нет :("; // Пасскод
	std::string board = "b"; // Имя борды
	long long thread_number = 0; // Номер треда в борде
	std::string comment = "";
	bool verbose = false;
	bool clean_cache = false;
	
	//getopt
	if (argc == 1) /* Если аргументов нет, вывод помощи */ {
		pomogite();
		return RET_ARGS;
	}
	parse_argv(
		argc, argv,
		board, thread_number,
		comment, passcode,
		verbose, clean_cache);
	fprintf(stderr, "board_name = %s\n", board.data());
	fprintf(stderr, "comment = %s\n", comment.data());
	
	if (initJsonCache() == -1) {
		fprintf(stderr, "[main]! Error @ initJsonCache(): %d\n",
			makaba_errno);
		return RET_INTERNAL;
	}
	
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
	
	std::cerr << "[main] Calling wrapper\n";
	Makaba::Thread &thread = * thread_init_wrapper(board, thread_number);
	if (makaba_errno) {
		fprintf(stderr, "[main] Warning: error during thread_init_wrapper(): %s\n",
				makaba_strerror(makaba_errno));
		bool should_exit = false;
		switch(makaba_errno) {
			case ERR_UNKNOWN:
			case ERR_POST_FORMAT:
			case ERR_REF_FORMAT:
			case ERR_GENERAL_FORMAT:
			case ERR_INTERNAL:
				should_exit = true;
				break;
			default:
				break;
		}
		if (thread.nposts == 0)
			should_exit = true;
		if (should_exit) {
			printf("Ошибка: %s\n", makaba_strerror(makaba_errno));
			delete &thread;
			makabaCleanup();
			return RET_INTERNAL;
		}
	}
	std::cerr << "[main] Exited wrapper\n";

	if (thread.nposts == 0) {
		printf(">>> !!!Smth strange with thread at %s/%lld: doen\'t exist, exiting!!! <<<\n",
			board.data(), thread_number);
		delete &thread;
		makabaCleanup();
		return RET_OK;
	}

	ncurses_init();
	ncurses_print_help();
	bool should_exit = false;
	ncurses_print_post(thread, 0);
	int ret = RET_OK;
	Makaba::Post dummy_post("", "",
							"", "",
							"", "");
	dummy_post.email = Sage_on ? "sage" : DEFAULT_EMAIL;
	
	std::string api_result = "";
	
	for (int cur_post = 0; should_exit == false; ) {
		bool done = 0;
		int int_input = 0;
		long long search_result = -1;
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
					Sage_on = ! Sage_on;
					ncurses_clear_errors();
					dummy_post.email = Sage_on ? "sage" : DEFAULT_EMAIL;
					ncurses_print_error(Sage_on ? "Sage: on" : "Sage: off");
					break;
				case 'S':
					ncurses_exit();
					api_result = thread.send_post(dummy_post);
					ncurses_init();
					ncurses_print_post(thread, cur_post);
					if (api_result.length())
						printw(">>> Запрос выполнен, ответ API: %s\n", api_result.data());
					else
						ncurses_print_error(makaba_strerror(makaba_errno));
					break;
				case 'h':
					ncurses_print_help();
					break;
				case 'C':
					ncurses_print_post(thread, cur_post);
					break;
				case 'G':
					printw("Перейти по номеру в треде: ");
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
					refresh();
					break;
				case 'g':
					printw("Перейти по номеру на доске: ");
					refresh();
					echo();
					scanw("%d", &int_input);
					noecho();
					if (int_input < 1) {
						int_input = thread[0].num;
					}
					search_result = thread.find(int_input);
					if (search_result == -1) {
						ncurses_clear_errors();
						ncurses_print_error("Пост не найден в треде\n");
					}
					else {
						cur_post = search_result;
					}
					ncurses_print_post(thread, cur_post);
					refresh();
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
	delete &thread;
	makabaCleanup();
	fprintf(stderr, "[main] Cleanup done, exiting\n");

	return ret;
}

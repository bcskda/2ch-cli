// ========================================
// File: main.cpp
// Main
// ========================================

#include <iostream>
#include <string>
#include <vector>
#include <locale.h>
#include <getopt.h>

using std::cout;
using std::clog;
using std::endl;
using std::string;
using std::vector;

#include "error.h"
#include "makaba.h"
#include "cache.h"
#include "app.h"

const int RET_OK = 0;
const int RET_ARGS = 1;
const int RET_PREEXIT = 2;
const int RET_MEMORY = 3;
const int RET_PARSE = 4;
const int RET_INTERNAL = 5; 


int main (int argc, const char **argv)
{
    freopen("/tmp/2ch-cli.log", "w", stderr);

    string passcode = "пасскода нет :(";
    string board = "abu";
    long long thread_number = 42375; // API-тред
    bool verbose = false;
    bool clean_cache = false;
    // getopt
    if (argc == 1) /* Если аргументов нет, вывод помощи */ {
        pomogite();
        return RET_ARGS;
    }
    parse_argv(argc, argv,
        board, thread_number,
        passcode,
        verbose, clean_cache);
    
    if (initJsonCache() == -1) {
        clog << "Error: " << makaba_strerror(makaba_errno) << endl;
        cout << "Ошибка при инициализации кэша" << endl;
        return RET_INTERNAL;
    }
    
    if (clean_cache == true) {
        if (cleanJsonCache()) {
            clog << "Error @ cleanJosCache()\n";
            cout << "Ошибка при очистке кэша\n";
            return RET_INTERNAL;
        }
        cout << "Кэш тредов очищен\n";
        return RET_OK;
    }
    
    setlocale (LC_ALL, "");
    makabaSetup();
    
    Makaba::Thread &thread = * thread_init_wrapper(board, thread_number);
    if (makaba_errno) {
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
            cout << "Ошибка: " << makaba_strerror(makaba_errno) << endl;
            delete &thread;
            makabaCleanup();
            return RET_INTERNAL;
        }
    }

    ncurses_init();
    int ret = RET_OK;
    bool should_exit = false;
    refresh();

    ThreadView tview(Wmain, thread);

    Makaba::Post dummy_post("", "", "", "", "", ""); // Для постинга
    dummy_post.email = Sage_on ? "sage" : DEFAULT_EMAIL;
    
    string api_result;
    
    for (int cur_post = 0; should_exit == false; ) {
        refresh();
        int nposts_old = thread.nposts;
        switch (getch()) {
            case 'Q': case 'q':
                should_exit = true;
                break;
            case 'c':
                ncurses_exit();
                edit(dummy_post.comment, Task_comment);
                ncurses_init();
                tview.print();
                break;
            case 's':
                Sage_on = ! Sage_on;
                ncurses_clear_errors();
                dummy_post.email = Sage_on ? "sage" : DEFAULT_EMAIL;
                ncurses_print_error(Sage_on ? "Sage: on\n" : "Sage: off\n");
                break;
            case KEY_ENTER:
            case '\n':
                ncurses_exit();
                thread.captcha = captcha_init_wrapper(thread);
                if (thread.captcha == NULL) {
                    ncurses_init();
                    tview.print();
                    ncurses_print_error(makaba_strerror(makaba_errno));
                }
                else {
                    api_result = thread_send_post_wrapper(thread, dummy_post);
                    clog << "[main] API answer: " << api_result << endl;
                    delete thread.captcha;
                    ncurses_init();
                    ncurses_print_post(thread, cur_post);
                    if (api_result.size())
                       Wlog << ("Запрос выполнен, ответ API: " + api_result);
                    else
                        ncurses_print_error(makaba_strerror(makaba_errno));
                    wrefresh(Wlog);
                }
                break;
            case 'f':
                wprintw(Wlog, "Поиск в треде по подстроке: ");
                wrefresh(Wlog);
                {
                    echo();
                    char substr[250];
                    wgetstr(Wlog, substr);
                    noecho();
                    const vector<const Makaba::Post *> results = thread.find(substr);
                    if (results.empty())
                        wprintw(Wlog, "Ничего не найдено.\n");
                    else {
                        wprintw(Wlog, "Найдено в:\n{ ");
                        for (auto p : results)
                            wprintw(Wlog, "№%d ", p->num);
                        wprintw(Wlog, " }\n");
                    }
                    wrefresh(Wlog);
                }
                break;
            case 'F':
                /* поиск по regex */
                break;
            case 'h':
                ncurses_print_help();
                break;
            case 'C':
                ncurses_clear_errors();
                break;
            case 'G':
                wprintw(Wlog, "Перейти по номеру в треде: ");
                wrefresh(Wlog);
                {
                    echo();
                    int num;
                    wscanw(Wlog, "%d", &num);
                    noecho();
                    if (num < 1) {
                        num = 1;
                    }
                    if (num > thread.nposts) {
                        num = thread.nposts;
                    }
                    cur_post = num - 1;
                }
                tview.print();
                wrefresh(Wmain);
                break;
            case 'g':
                wprintw(Wlog, "Перейти по номеру на доске: ");
                wrefresh(Wlog);
                {
                    echo();
                    int num;
                    wscanw(Wlog, "%d", &num);
                    noecho();
                    if (num < 1) {
                        num = thread[0].num;
                    }
                    long long search_result = thread.find(num);
                    if (search_result == -1) {
                        ncurses_clear_errors();
                        ncurses_print_error("Пост не найден в треде\n");
                    }
                    else {
                        cur_post = search_result;
                    }
                }
                ncurses_print_post(thread, cur_post);
                wrefresh(Wmain);
                break;
            case 'u':
                wprintw(Wlog, "Обновление треда ...");
                wrefresh(Wlog);
                if (thread.update()) {
                    fprintf(stderr, "[main] Error @ thread::update()\n");
                    wprintw(Wlog, " ошибка\n");
                    ncurses_print_error(makaba_strerror(makaba_errno));
                }
                else {
                wprintw(Wlog, " готово, %d новых постов\n",
                    thread.nposts - nposts_old);
                }
                    wrefresh(Wlog);
                    break;
            case KEY_RIGHT:
            case KEY_DOWN:
                tview.scroll(1);
                tview.print();
                break;
            case KEY_LEFT:
            case KEY_UP:
                tview.scroll(-1);
                tview.print();
                break;
            case KEY_NPAGE: // PageDown
                tview.scroll(Skip_on_PG);
                tview.print();
                break;
            case KEY_PPAGE: // PageUp
                tview.scroll(-Skip_on_PG);
                tview.print();
                break;
            case KEY_HOME:
                tview.scroll_to(0);
                tview.print();
                break;
            case KEY_END:
                tview.scroll_to(SCROLL_END);
                tview.print();
                break;
        }
    }
    ncurses_exit();

    disarmJsonCache(thread);
    delete &thread;
    makabaCleanup();
    fprintf(stderr, "[main] Cleanup done, exiting\n");

    return ret;
}

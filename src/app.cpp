// ========================================
// File: app.cpp
// Interface, wrappers, etc.
// (Implementation)
// ========================================

#include "app.h"

// Global defs

const int Skip_on_PG = 20;

const int Head_pos_x = 0;
const int Head_pos_y = 0;
const int Err_pos_x = 50;
const int Err_pos_y = 0;

WINDOW *Wmain = NULL;
WINDOW *Wlog = NULL;

const std::string Headers_pref = "| ";
const std::string Headers_suff = " |";
const std::string Endl = "\n";
const std::string Help =
    "u - Обновить тред\n"
    "s - вкл/выкл sage\n"
    "c - изменить текст поста\n"
    "Enter - отправить пост\n"
    "f - поиск по подстроке\n"
    //"F - поиск по регулярному выражению\n"
    "\n"
    "[LEFT] / [UP] предыдущий пост\n"
    "[RIGHT] / [DOWN] следующий пост\n"
    "[PageUp] - " + std::to_string(Skip_on_PG) + " постов назад\n"
    "[PageDown] - " + std::to_string(Skip_on_PG) + " постов вперёд\n"
    "[Home] - первый пост, [End] - последний пост\n"
    "g - перейти по номеру на доске\n"
    "G - перейти по номеру в треде\n"
    "\n"
    "C - очистить экран\n"
    "h - помощь, q - выход\n\n";


bool Sage_on = false;



// End Global defs


int printThreadHeader(const Makaba::Thread &thread)
{
    refresh();
    wmove(Wmain, Head_pos_y, Head_pos_x);
    std::string header = Headers_pref + "/" + thread.board + " " + thread[0].subject
                         + std::to_string(thread.nposts) + "постов" + Endl;
    Wmain << header;
    return 0;
}

int printPost(
    const Makaba::Post &post,
    const bool show_email,
    const bool show_files)
{
    refresh();
    if (post.date.length() == 0) {
        fprintf(stderr, "! ERROR @printPost: Null date in struct post\n");
        makaba_errno = ERR_POST_FORMAT;
        return 1;
    }

    std::string header;
    bool sage = false;
    if (show_email == true && post.email.length() > 0) {
        if (post.email == "mailto:sage")
	    header = post.name + "@" + post.email;
        else {
	    header = post.name;
            sage = true;
        }
    }
    // @TODO Выравнивать строки заголовка по ширине
    
    Wmain << Headers_pref;
    if (sage)
        wattron(Wmain, A_UNDERLINE);
    Wmain << header << Endl; //
    wattroff(Wmain, A_UNDERLINE);
    
    header = "№" + std::to_string(post.num) + " (" + std::to_string(post.rel_num) + ") "  + post.date;

    Wmain << Headers_pref << header << Endl;

    if (show_files && ! post.files.empty()) {
        for (Json::Value item : post.files) {
	    Wmain << Headers_pref;
	    header = "File: \"" + item["displayname"].asString() + "\" "
	             + BASE_URL + item["path"].asString() + Endl + Endl;
            Wmain << header;
        }
    }
    
    Wmain << post.comment << Endl << Endl;

    return 0;
}

void ncurses_init() {
    initscr();
    raw();
    if (Wmain == NULL) { // Upper, 4/5 height, full width
        refresh();
        Wmain = newwin(4 * LINES / 5, COLS, 0, 0);
        wrefresh(Wmain);
    }
    if (Wlog == NULL) { // Lower, 1/5 height, full width
        refresh();
        Wlog = newwin(LINES / 5, COLS, 4 * LINES / 5, 0);
        wrefresh(Wlog);
    }
    keypad(stdscr, true);
    noecho();
    nodelay(stdscr, false);
    nl();
}

void ncurses_exit() {
    if (Wmain != NULL) {
        delwin(Wmain);
        Wmain = NULL;
    }
    if (Wlog != NULL) {
        delwin(Wlog);
        Wlog = NULL;
    }
    endwin();
}

void ncurses_print_help() {
    int height = 15;
    int width = 60;
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;
    refresh();
    WINDOW *Whelp = newwin(starty, startx, height, width);
    Whelp << Help;
    wrefresh(Whelp);
    getch();
    werase(Whelp);
    wrefresh(Whelp);
    delwin(Whelp);
}

void ncurses_print_post(
    const Makaba::Thread &thread,
    const long long num)
{
    werase(Wmain);
    printThreadHeader(thread);
    printPost(thread[num], true, true);
    wrefresh(Wmain);
}

void ncurses_print_error(const std::string &mesg) {
    wattron(Wlog, A_STANDOUT);
    Wlog << mesg;
    wattroff(Wlog, A_STANDOUT);
    wrefresh(Wlog);
}

void ncurses_print_error(const char *mesg) {
    wattron(Wlog, A_STANDOUT);
    wprintw(Wlog, "%s", mesg);
    wattroff(Wlog, A_STANDOUT);
    wrefresh(Wlog);
}

void ncurses_clear_errors()
{
    werase(Wlog);
    wrefresh(Wlog);
}


WINDOW *operator<<(WINDOW *win, const std::string &value) {
    wprintw(win, value.c_str());
    return win;
}

void parse_argv(
    const int argc, const char **argv,
    std::string &board, long long &thread_number,
    std::string &passcode,
    bool &verbose, bool &clean_cache)
{
    int opt;
    while (( opt = getopt(argc, (char * const *)argv, "hp:b:n:vC") ) != -1)
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
            case 'v':
                verbose = true;
                break;
            case 'C':
                clean_cache = true;
                break;
            case 'h':
                pomogite();
                exit(RET_OK);
            default:
                printf("Неизвестная опция %c\n", opt);
                pomogite();
                exit(RET_ARGS);
        }
    }
}

Makaba::Thread *thread_init_wrapper (
    const std::string &vboard,
    const long long &vnum)
{
    std::cerr << "[[ " << __PRETTY_FUNCTION__ << " ]] Constructing thread\n";
    Makaba::Thread *thread = new Makaba::Thread(vboard, vnum, false);
    thread->set_hook(thread, thread_hook_on_update);
    bool fallback = false;
    if (Json_cache_dir == NULL) {
        if (initJsonCache()) {
            fprintf(stderr, "[%s] Error @ initJsonCache()\n",
                    __PRETTY_FUNCTION__);
            fprintf(stderr, "[%s] Warning: Fallback to getThread()\n",
                    __PRETTY_FUNCTION__);
            fallback = true;
        }
    }
    long long size;
    char *raw = NULL;
    std::cerr << "[thread_init_wrapper] Checking this thread in cache...\n";
    if (fallback == false && checkJsonCache(*thread)) { // Кэш в порядке и тред в нем есть
        std::cerr << "[thread_init_wrapper] Found in cache!\n";
        raw = readJsonCache(*thread, &size);
        if (raw == NULL) { // Проблема при чтении треда из кэша
            fprintf(stderr, "[%s] Warning: Fallback to getThread()\n",
                    __PRETTY_FUNCTION__);
            fallback = true;
        }
    }
    else {
        std::cerr << "[thread_init_wrapper] Not found in cache\n";
        fallback = true;
    }
    if (fallback == true) { // По какой-то причине тред нужно качать
        std::cerr << "[thread_init_wrapper] Downloading thread from server\n";
        raw = getThread(thread->board.data(), thread->num,
                        1, &size, false);
        if (raw == NULL) {
            fprintf(stderr, "[%s] Error @ getThread()\n",
                    __PRETTY_FUNCTION__);
            *thread = Makaba::NullThread;
            return thread;
        }
    }
    std::cerr << "[thread_init_wrapper] Appending cached posts\n";
    if (thread->append(raw)) {
        std::cerr << __PRETTY_FUNCTION__ << " Error at append\n";
        *thread = Makaba::NullThread;
        return thread;
    }
    std::cerr << "[thread_init_wrapper] Caching received posts\n";
    writeJsonCache(*thread, raw);
    /* Вопрос, нужно ли: долгая загрузка при плохом соединении,
    * интерфейса в этот момент нет. Индикатор прогресса в get*?
    * std::cerr << "[thread_init_wrapper] Checking for new posts\n";
    * thread.update(); // Ошибки обрабатывает main()
    */
    std::cerr << "[thread_init_wrapper] Exiting\n";
    return thread;
}

Makaba::Captcha_2ch *captcha_init_wrapper(const Makaba::Thread &thread)
{
    Makaba::Captcha_2ch *captcha = new Makaba::Captcha_2ch(thread);
    if (captcha->isNull()) {
        delete captcha;
        std::cerr << "[[ " << __PRETTY_FUNCTION__ << " ]] Error at captcha constructor\n";
        return NULL;
    }
    if (captcha->get_png()) {
        std::cerr << "[[ " << __PRETTY_FUNCTION__ << " ]] Error at captcha.get_png()\n";
        delete captcha;
        return NULL;
    };
    std::cerr << "[[ " << __PRETTY_FUNCTION__ << " ]] Captcha ready at " << captcha << std::endl;
    return captcha;
}

std::string thread_send_post_wrapper(
    Makaba::Thread &thread,
    const Makaba::Post &post)
{
    if (thread.captcha == NULL) {
        std::cerr << "[[ " << __PRETTY_FUNCTION__ << " ]] Error: null thread.captcha\n";
        return std::string(""); // TODO нормальная передача ошибок
    }
    convert_img(CaptchaPngFilename, CaptchaUtfFilename); // Гонка сигналов etc.
    caca_display_t *display = show_img(CaptchaUtfFilename);
    caca_canvas_t *canvas = caca_get_canvas(display);
    caca_put_str(canvas,
                1, Converter_height_i + 1,
                "Ответ на капчу (секурность уровня sudo): ");
    caca_refresh_display(display);
    std::cin >> thread.captcha->value;
    caca_free_display(display);
    return thread.send_post(post);
}

void *thread_hook_on_update(void *userdata, const char *raw)
{
    std::cerr << "[hook on_update] Called with \"" << raw << "\"\n";
    Makaba::Thread *thread = (Makaba::Thread *)userdata;
    if (writeJsonCache(*thread, raw) == -1) {
        fprintf(stderr, "[%s] Error: writeJsonCache() failed\n",
                __PRETTY_FUNCTION__);
        return NULL;
    }
    return userdata;
}

void pomogite() // Справка
{
    printf("2ch-cli " VERSION " - консольный клиент двача\n");
    printf("Использование:\n");
    printf(" -h - помощь\n");
    printf(" -b - задать борду\n");
    printf(" -n - задать номер треда\n");
    /* DEPRECATED
    * printf(" -s - отправить пост\n");
    */
    printf(" -c - задать комментарий\n");
    printf(" -C - очистить кэш тредов\n");
    printf(" -v - подробный лог (для разработчиков)\n");
    printf(" -p - задать пасскод (не поддерживается)\n");
}

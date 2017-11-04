// ========================================
// File: app.cpp
// Interface, wrappers, etc.
// (Implementation)
// ========================================

#include "app.h"


// Global defs

const int Skip_on_PG = 20;
const int SCROLL_END = INT_MAX;

const int Head_pos_x = 0;
const int Head_pos_y = 0;
const int Err_pos_x = 50;
const int Err_pos_y = 0;
const int Header_size = 2;

WINDOW *Wmain = NULL;
WINDOW *Wlog = NULL;

const string Headers_pref = "| ";
const string Headers_suff = " |";
const string Endl = "\n";
const string Help =
    "u - Обновить тред\n"
    "s - вкл/выкл sage\n"
    "c - изменить текст поста\n"
    "Enter - отправить пост\n"
    "f - поиск по подстроке\n"
    //"F - поиск по регулярному выражению\n"
    "\n"
    "[LEFT] / [UP] предыдущий пост\n"
    "[RIGHT] / [DOWN] следующий пост\n"
    "[PageUp] - " + to_string(Skip_on_PG) + " постов назад\n"
    "[PageDown] - " + to_string(Skip_on_PG) + " постов вперёд\n"
    "[Home] - первый пост, [End] - последний пост\n"
    "g - перейти по номеру на доске\n"
    "G - перейти по номеру в треде\n"
    "\n"
    "C - очистить экран\n"
    "h - помощь, q - выход\n\n";


bool Sage_on = false;



// End Global defs

BasicView::BasicView(WINDOW *win):
  win_(win)
{}

PostView::PostView(const Makaba::Post &post):
  BasicView(stdscr),
  post_(post)
{}

PostView::PostView(WINDOW *win, const Makaba::Post &post):
  BasicView(win),
  post_(post)
{}

string PostView::print_buf() const {
    if (post_.date.size() == 0) {
        clog << "[BUG] Empty date in Post at " << &post_ << endl;;
        return "";
    }

    string ret;
    ret = Headers_pref + post_.name;
    if (show_email_ && post_.email.size())
        ret += " (" + post_.email + ")";
    ret += Endl;
    
    ret += Headers_pref;
    ret += "№" + to_string(post_.num) + " (" + to_string(post_.rel_num) + ") "  + post_.date;
    ret += Endl;

    if (show_files_ && ! post_.files.empty()) {
        for (const Json::Value &item : post_.files) {
	    ret += Headers_pref;
            ret += "File: \"" + item["displayname"].asString() + "\" "
	             + BASE_URL + item["path"].asString();
            ret += Endl;
        }
    }

    ret += post_.comment;
    return ret;
}

void PostView::print() const {
    win_ << this->print_buf() << Endl;
}


ThreadView::ThreadView(WINDOW *win, const Makaba::Thread &thread):
  BasicView(win),
  thread_(thread),
  size_(thread.nposts),
  buffer_(),
  posts_(thread.nposts),
  pos_(0)
{
    clog << "New ThreadView\n";
    string post_buf;
    int maxy, maxx;
    getmaxyx(win_, maxy, maxx);
    int l = 0;
    for (long long i = 0; i < thread.nposts; i++) {
        post_buf = PostView(thread[i]).print_buf();
        clog << "Post # " << i << endl << endl;
        clog << "Text: \"\"\"\n" << post_buf << "\n\"\"\"" << endl;
        posts_[i].begin = buffer_.size();
        posts_[i].lines = 1;
        l = 0;
        for (string::size_type j = 0; j < post_buf.size(); j++)
            if (post_buf[j] == '\n' /*|| j - l >= maxx*/) {
                buffer_.push_back(post_buf.substr(l, j - l));
                clog << "New entry: " + buffer_.back() << Endl;
                posts_[i].lines++;
                l = j + 1;
            }
        if (l < int(post_buf.size())) {
            buffer_.push_back(post_buf.substr(l, post_buf.size() - l));
            posts_[i].lines++;
        }
        buffer_.push_back("");
        posts_[i].lines++;
    }
    buffer_.push_back("");
    clog << "New ThreadView ready, " << buffer_.size() << " entries for " << size_ << "posts\n";
}

void ThreadView::print_header() const {
    wmove(win_, Head_pos_y, Head_pos_x);
    win_ << Headers_pref
         << ("/" + thread_.board + " | ")
         << (thread_[0].subject + " | ")
         << (to_string(thread_.nposts) + " постов")
         << Headers_suff
         << Endl;
    win_ << ("-------------------------------------------------------" + Endl);
}

void ThreadView::print() const {
    this->print_header();
    int maxy, maxx;
    getmaxyx(win_, maxy, maxx);
    wclrtobot(win_);
    for (int i = pos_; i < int(buffer_.size()) && i - pos_ < maxy - Header_size; i++)
        win_ << buffer_[i] << Endl;
    wrefresh(win_);
}

void ThreadView::scroll(int count) {
    int maxy, maxx;
    getmaxyx(win_, maxy, maxx);
    pos_ += count;
    if (pos_ < 0)
        pos_ = 0;
    if (pos_ > int(buffer_.size()) - maxy + Header_size - 1)
        pos_ = int(buffer_.size()) - maxy + Header_size - 1;
}

void ThreadView::scroll_to(int line) {
    int maxy, maxx;
    getmaxyx(win_, maxy, maxx);
    pos_ = line;
    if (pos_ < 0)
        pos_ = 0;
    if (pos_ > int(buffer_.size()) - maxy + Header_size - 1)
        pos_ = buffer_.size() - maxy + Header_size - 1;
}


int printThreadHeader(const Makaba::Thread &thread)
{
    refresh();
    wmove(Wmain, Head_pos_y, Head_pos_x);
    string header = Headers_pref+ "/" + thread.board + " " + thread[0].subject
                         + " " + to_string(thread.nposts) + " постов" + Endl;
    Wmain << header;
    return 0;
}

int printPost(
    const Makaba::Post &post,
    const bool show_email,
    const bool show_files)
{
    refresh();
    if (post.date.size() == 0) {
        fprintf(stderr, "! ERROR @printPost: Null date in struct post\n");
        makaba_errno = ERR_POST_FORMAT;
        return 1;
    }

    string header;
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
    
    header = "№" + to_string(post.num) + " (" + to_string(post.rel_num) + ") "  + post.date;

    Wmain << Headers_pref << header << Endl;

    if (show_files && ! post.files.empty()) {
        for (Json::Value item : post.files) {
	    Wmain << Headers_pref;
	    header = "File: \"" + item["displayname"].asString() + "\" "
	             + BASE_URL + item["path"].asString() + Endl;
            Wmain << header;
        }
    }
    Wmain << Endl;
    
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

void ncurses_print_error(const string &mesg) {
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


WINDOW *operator<<(WINDOW *win, const string &value) {
    wprintw(win, value.c_str());
    return win;
}

void parse_argv(
    const int argc, const char **argv,
    string &board, long long &thread_number,
    string &passcode,
    bool &verbose, bool &clean_cache)
{
    int opt;
    while (( opt = getopt(argc, (char * const *)argv, "hp:b:n:vC") ) != -1)
    {
        switch (opt)
        {
            case 'p':
                passcode = optarg;
                printf("Еще не поддерживается\n");
                break;
            case 'b':
                board = optarg;
                break;
            case 'n':
                thread_number = atoll(optarg);
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
    const string &vboard,
    const long long &vnum)
{
    Makaba::Thread *thread = new Makaba::Thread(vboard, vnum, false);
    thread->set_hook(thread, thread_hook_on_update);
    bool need_dload = true;
    long long size;
    char *raw = NULL;
    if (checkJsonCache(*thread)) { // Тред в кэше
        raw = readJsonCache(*thread, &size);
        if (raw != NULL)
            need_dload = false;
        else
            clog << " Error reading cached\n";
    }
    if (need_dload) {
        raw = getThread(thread->board.c_str(), thread->num, 1, &size, false);
        if (raw == NULL) {
            *thread = Makaba::NullThread;
            return thread;
        }
    }
    if (thread->append(raw)) {
        *thread = Makaba::NullThread;
        return thread;
    }
    writeJsonCache(*thread, raw);
    return thread;
}

Makaba::Captcha_2ch *captcha_init_wrapper(const Makaba::Thread &thread)
{
    Makaba::Captcha_2ch *captcha = new Makaba::Captcha_2ch(thread);
    if (captcha->isNull()) {
        delete captcha;
        clog << "Captcha constructor failed\n";
        return NULL;
    }
    if (captcha->get_png()) {
        clog << "get_png() failed\n";
        delete captcha;
        return NULL;
    }
    return captcha;
}

string thread_send_post_wrapper(
    Makaba::Thread &thread,
    const Makaba::Post &post)
{
    if (thread.captcha == NULL) 
        return ""; // TODO нормальная передача ошибок
    convert_img(CaptchaPngFilename, CaptchaUtfFilename); // Гонка сигналов etc.
    caca_display_t *display = show_img(CaptchaUtfFilename);
    caca_canvas_t *canvas = caca_get_canvas(display);
    caca_put_str(canvas,
                1, Converter_height_i + 1,
                "Captcha answer (won't be echoed): "); // проблемы с локалью
    caca_refresh_display(display);
    cin >> thread.captcha->value;
    caca_free_display(display);
    return thread.send_post(post);
}

void *thread_hook_on_update(void *userdata, const char *raw)
{
    clog << "[hook on_update] Called with \"" << raw << "\"\n";
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
    printf(" -b [борда]\n");
    printf(" -n [номер треда]\n");
    printf(" -C - очистить кэш тредов\n");
    printf(" -v - подробный лог\n");
    printf(" -p [пасскод]  (не поддерживается)\n");
}

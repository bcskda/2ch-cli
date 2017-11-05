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
const int Log_off_x = 0;
const int Log_off_y = 5;
const int Err_pos_x = 50;
const int Err_pos_y = 0;
const int Header_size = 3;

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
const string Post_delim = "x-------------------------------------------------------";

bool Sage_on = false;



// End Global defs

BasicView::BasicView(WINDOW *win):
  win_(win),
  maxy_(0),
  maxx_(0)
{
    getmaxyx(win, maxy_, maxx_);
}


PostView::PostView(const Makaba::Post &post):
  BasicView(stdscr),
  post_(post),
  show_files_(true),
  show_email_(true)
{}

PostView::PostView(WINDOW *win, const Makaba::Post &post,
                   bool show_files, bool show_email):
  BasicView(win),
  post_(post),
  show_files_(show_files),
  show_email_(show_email)
{}

string PostView::print_buf() const {
    if (post_.date.size() == 0) {
        clog << "[BUG] Empty date in Post at " << &post_ << endl;;
        return "";
    }

    string ret = Post_delim + Endl + Headers_pref + post_.name;
    if (show_email_ && post_.email.size())
        ret += " (" + post_.email + ")";
    ret += Endl;
    
    ret += Headers_pref;
    ret += "№" + to_string(post_.num) + "   #" + to_string(post_.rel_num) + Endl;
    ret += Headers_pref + post_.date + Endl;

    if (show_files_ && ! post_.files.empty()) {
        for (const Json::Value &item : post_.files) {
	    ret += Headers_pref;
            ret += "File: \"" + item["displayname"].asString() + "\" "
	             + BASE_URL + item["path"].asString();
            ret += Endl;
        }
    }

    ret += Post_delim + Endl + post_.comment;
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
    this->append(0);
    clog << "New ThreadView ready, " << buffer_.size()
         << " entries for " << size_ << " posts\n";
}

void ThreadView::append(int from) {
    // Разбиваем UnicodeString по концам строк, копируем в другую UnicodeString и конвертим в std::string
    UnicodeString unis, unitmp;
    string stds;
    for (long long i = from; i < thread_.nposts; i++) {
        unis = UnicodeString::fromUTF8(PostView(thread_[i]).print_buf());
        posts_[i].begin = buffer_.size();
        posts_[i].lines = 1;
        int32_t l = 0;
        for (int32_t j = 0; j < unis.length(); j++)
            if (unis[j] == '\n' || j - l > maxx_ - 2) {
                unis.extractBetween(l, j, unitmp);
                unitmp.toUTF8String(stds);
                buffer_.push_back(stds);
                unitmp = "";
                stds = "";
                posts_[i].lines++;
                l = j;
                if (unis[j] == '\n')
                    l++;
            }
        if (l < unis.length()) {
            unis.extractBetween(l, unis.length(), unitmp);
            unitmp.toUTF8String(stds);
            buffer_.push_back(stds);
            unitmp = "";
            stds = "";
            posts_[i].lines++;
        }
        buffer_.push_back("");
        posts_[i].lines++;
    }
}

void ThreadView::update() {
    if (win_ != Wmain) {
        clog << "update win_" << endl;
        win_ = Wmain;
    }
    int y, x;
    getmaxyx(win_, y, x);
    if (x != maxx_ || y != maxy_) {
        clog << "new size (" << y << ";" << x << ")\n";
        if (x != maxx_) {
            clog << "rebuilding\n"; // FIXME Работает, но не совсем
            buffer_.clear();
            posts_.clear();
            this->append(0);
        }
        maxy_ = y;
        maxx_ = x;
    }
    else {
        this->append(size_);
        size_ = thread_.nposts;
    }
}

int ThreadView::size() const {
    return size_;
}

void ThreadView::print_header() const {
    wmove(win_, Head_pos_y, Head_pos_x);
    wclrtobot(win_);
    string header = Headers_pref + "/" + thread_.board + " | "
                    + thread_[0].subject + Endl;
    int cx = (maxx_ - header.size() + 1) / 2;
    mvwprintw(win_, 0, cx, header.c_str());
    header = to_string(thread_.nposts) + " постов |  "
             + to_string((pos_ + maxy_ - Header_size) * 100 / buffer_.size())
             + "%" + Endl;
    cx = (maxx_ - header.size() + 1) / 2;
    mvwprintw(win_, 1, cx, header.c_str());
    mvwprintw(win_, 2, 0, "x");
    for (int i = 1; i < maxx_ - 1; i++)
        wprintw(win_, "-");
    wprintw(win_, "x");
}

void ThreadView::print() const {
    this->print_header();
    wclrtobot(win_);
    for (int i = pos_; i < int(buffer_.size()) - 1 && i - pos_ < maxy_ - Header_size - 1; i++)
        win_ << buffer_[i] << Endl;
    wprintw(win_, "x");
    for (int i = 1; i < maxx_ - 1; i++)
        wprintw(win_, "-");
    wprintw(win_, "x");
    wrefresh(win_);
}

void ThreadView::scroll(long long count) {
    pos_ += count;
    if (pos_ < 0)
        pos_ = 0;
    if (pos_ > int(buffer_.size()) - maxy_ + Header_size)
        pos_ = int(buffer_.size()) - maxy_ + Header_size;
}

void ThreadView::scroll_to_line(long long num) {
    if (num < 0)
        num = 0;
    if (num > int(buffer_.size()) - maxy_ + Header_size)
        num = int(buffer_.size()) - maxy_ + Header_size;
    pos_ = num;
}

void ThreadView::scroll_to_post(long long num) {
    if (num < 0)
        num = 0;
    if (num > size_ - 1)
        num = size_ - 1;
    this->scroll_to_line(posts_[num].begin);
}

void ncurses_init() {
    initscr();
    raw();
    if (Wmain == NULL) {
        refresh();
        Wmain = newwin(LINES - Log_off_y, COLS, 0, 0);
        wrefresh(Wmain);
    }
    if (Wlog == NULL) {
        refresh();
        Wlog = newwin(Log_off_y, COLS, LINES - Log_off_y, 0);
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


WINDOW *operator<<(WINDOW *win, const string &s) {
    wprintw(win, s.c_str());
    return win;
}

WINDOW *operator<<(WINDOW *win, const UnicodeString &s) {
    string stds = "";
    wprintw(win, s.toUTF8String(stds).c_str());
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

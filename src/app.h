// ========================================
// File: app.h
// Interface, wrappers, etc.
// (Headers)
// ========================================

#pragma once

#include <cursesw.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <utility>

using std::cin;
using std::clog;
using std::endl;
using std::string;
using std::to_string;
using std::vector;

#include "error.h"
#include "external.h"
#include "makaba.h"
#include "cache.h"

#define VERSION "v0.4test9"

class BasicView {
    private:
      BasicView();
    protected:
      BasicView(WINDOW *win);
      WINDOW *win_;
    public:
      virtual void print() const {};
};

class PostView : protected BasicView {
    PostView();
    const Makaba::Post &post_;
    protected:
      bool show_email_;
      bool show_files_;
    public:
      PostView(const Makaba::Post &post);
      PostView(WINDOW *win, const Makaba::Post &post);
      void print() const override;
      string print_buf() const;
};

struct PostInfo {
    int begin;
    int lines;
};

class ThreadView : protected BasicView {
    ThreadView();
    const Makaba::Thread &thread_;
    long long size_;
    vector<string> buffer_;
    vector<struct PostInfo> posts_;
    long long pos_;
    public:
      ThreadView(const Makaba::Thread &thread);
      ThreadView(WINDOW *win, const Makaba::Thread &thread);
      void print_header() const;
      void print() const override;
      void scroll(int count);
      void scroll_to(int line);
      //void update();
};


extern const int RET_OK;
extern const int RET_ARGS;
extern const int RET_PREEXIT;
extern const int RET_MEMORY;
extern const int RET_PARSE;
extern const int RET_INTERNAL;

extern const int Skip_on_PG;
extern const int SCROLL_END;

extern const int Head_pos_x;
extern const int Head_pos_y;
extern const int Err_pos_x;
extern const int Err_pos_y;
extern const int Header_size;

extern WINDOW *Wmain;
extern WINDOW *Wlog;

extern const std::string Headers_pref;
extern const std::string Headers_suff;
extern const std::string Endl;
extern const std::string Help;

extern bool Sage_on;

int printThreadHeader(const Makaba::Thread &thread);
int printPost(
    const Makaba::Post &post,
    const bool show_email,
    const bool show_files);

void pomogite();
void parse_argv(const int argc, const char **argv,
    std::string &board, long long &thread_number,
    std::string &passcode,
    bool &verbose, bool &clean_cache);
void ncurses_init();
void ncurses_exit();
void ncurses_print_help();
void ncurses_print_post(const Makaba::Thread &thread, const long long num);
void ncurses_print_error(const char *mesg);
void ncurses_clear_errors();
WINDOW *operator<<(WINDOW *win, const std::string &value);

Makaba::Thread *thread_init_wrapper (
    const std::string &vboard,
    const long long &vnum
);
Makaba::Captcha_2ch *captcha_init_wrapper(const Makaba::Thread &thread);
std::string thread_send_post_wrapper(
    Makaba::Thread &thread,
    const Makaba::Post &post
);
void *thread_hook_on_update(void *userdata, const char *raw);

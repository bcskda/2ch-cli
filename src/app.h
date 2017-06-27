// ========================================
// File: app.h
// Interface, wrappers, etc.
// (Headers)
// ========================================

#pragma once

#include <ncurses.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>

#include "error.h"
#include "external.h"
#include "makaba.h"
#include "cache.h"

#define VERSION "v0.4test8"

extern const int RET_OK;
extern const int RET_ARGS;
extern const int RET_PREEXIT;
extern const int RET_MEMORY;
extern const int RET_PARSE;
extern const int RET_INTERNAL;

extern const int Skip_on_PG;

extern const int Head_pos_x;
extern const int Head_pos_y;
extern const int Err_pos_x;
extern const int Err_pos_y;

extern const char *Headers_pref;
extern const char *Headers_suff;

extern bool Sage_on;

int printThreadHeader(const Makaba::Thread &thread);
int printPost (const Makaba::Post &post, const bool show_email, const bool show_files);

void pomogite();
void parse_argv(const int argc, const char **argv,
	std::string &board, long long &thread_number, std::string &comment, std::string &passcode,
	bool &verbose, bool &clean_cache);
void ncurses_init();
void ncurses_exit();
void ncurses_print_help();
void ncurses_print_post(const Makaba::Thread &thread, const long long num);
void ncurses_print_error(const char *mesg);
void ncurses_clear_errors();

Makaba::Thread &thread_init_wrapper (
	const std::string &vboard,
	const long long &vnum
);
void *thread_hook_on_update(void *userdata, const char *raw);

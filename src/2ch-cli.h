// ========================================
// File: 2ch-cli.h
// A CLI-client for 2ch.hk imageboard written on C/C++
// (Headers)
// ========================================

#pragma once

#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <getopt.h>

#include "makaba.h"
#include "parser.h"
#include "external.h"
#include "error.h"

#define VERSION "v0.4test7"

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

const char *sendPost(const makaba_post &post,
					 const std::string &board,
					 const long long &threadnum = 0);

int printThreadHeader(const makaba_thread &thread);
int printPost (const makaba_post &post, const bool show_email, const bool show_files);

void parse_argv(const int argc, const char **argv,
	std::string &board, long long &thread_number, std::string &comment, std::string &passcode,
	bool &send_post, bool &verbose, bool &clean_cache);
void ncurses_init();
void ncurses_exit();
void ncurses_print_help();
void ncurses_print_post(const makaba_thread &thread, const long long num);
void ncurses_print_error(const char *mesg);
void ncurses_clear_errors();

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

const int RET_OK = 0;
const int RET_ARGS = 1;
const int RET_PREEXIT = 2;
const int RET_MEMORY = 3;
const int RET_PARSE = 4;
const int RET_INTERNAL = 5;

const int Skip_on_PG = 20;

const int Head_pos_x = 0;
const int Head_pos_y = 0;
const int Err_pos_x = 50;
const int Err_pos_y = 0;

const char Headers_pref[] = "| ";
const char Headers_suff[] = " |";

bool Sage_on = false;

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

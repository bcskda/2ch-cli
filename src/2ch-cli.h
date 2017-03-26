// ========================================
// File: 2ch-cli.h
// A CLI-client for 2ch.hk imageboard written on C
// (Headers)
// ========================================

#pragma once

#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#include "makaba.c"
#include "parser.c"
#include "image.c"

#define VERSION "v0.4test2"

const int ERR_ARGS = -1,
		  ERR_BROKEN_POST = -2;

const int RET_OK = 0,
		  RET_NOARGS = 1,
		  RET_PREEXIT = 2;

//const size_t Thread_size = 10000000; // Размер буфера с тредом

const char *CaptchaPngFilename = "captcha.png";
const char *CaptchaUtfFilename = "captcha.utf8";

int printPost (struct post *post, const bool show_email, const bool show_files);
char *prepareCaptcha(const char *board, const char *thread);

void parse_argv(const int argc, const char **arvg,
	char *board_name, long long *post_number, char *passcode, bool *start_or_not);
void ncurses_init();
void ncurses_exit();
void ncurses_print_help();

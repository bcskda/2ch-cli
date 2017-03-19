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
#include <stdbool.h>
#include <getopt.h>

#include "makaba.h"
#include "parser.h"
#include "image.h"

#define VERSION "v0.4test1"

const int ERR_ARGS = -1,
		  ERR_BROKEN_POST = -2;

const int RET_OK = 0,
		  RET_NOARGS = 1,
		  RET_PREEXIT = 2;

//const size_t Thread_size = 10000000; // Размер буфера с тредом

int printPost (struct post* post, const bool show_email, const bool show_files);

void ncurses_init();
void ncurses_exit();
void ncurses_print_help();

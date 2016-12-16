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

#define VERSION "vTesting"

const size_t Thread_size = 1000000;

int printPost (struct post* post,	const bool show_email, const bool show_files);



// ========================================
// File: image.h
// Image processing functions
// (Headers)
// ========================================

#pragma once

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <caca.h>
#include <stdbool.h>
#include "error.h"

#define Converter          "img2txt"
#define Converter_width_i  140
#define Converter_height_i 35
#define Converter_width_s  "140"
#define Converter_height_s "35"
#define Converter_format   "utf8"

const int Ret_show_OK = 0;
const int Ret_show_create_canvas = 1;
const int Ret_show_import_canvas = 2;
const int Ret_show_create_display = 3;

int show_img(const char *filename);
//int convert_img(const char *filename, const bool v);
void convert_img(const char *filename, const char *ofile, const bool v);
int perf_exec(const char **args);

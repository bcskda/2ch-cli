// ========================================
// File: image.h
// Image processing functions
// (Headers)
// ========================================

#pragma once

#include <stdio.h>
#include <unistd.h>
#include <caca.h>
#include <stdbool.h>

#define Converter          "img2txt"
#define Converter_width_i  200
#define Converter_height_i 50
#define Converter_width_s  "200"
#define Converter_height_s "50"
#define Converter_format   "ansi"

const int Ret_convert_OK = 0;

//int convert_img(const char *filename, const bool v);
void convert_img(const char *filename, const char *ofile, const bool v);
int perf_exec(const char *args);

//int show_img_with_caca(const char *filename, const bool v);

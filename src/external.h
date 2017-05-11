#pragma once

#include <iostream>
#include <fstream>
#include <streambuf>
#include <string>
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
#define Converter_format   "caca"

const int Ret_show_OK = 0;
const int Ret_show_create_canvas = 1;
const int Ret_show_import_canvas = 2;
const int Ret_show_create_display = 3;

caca_display_t *show_img(const char *filename);
void convert_img(const char *filename, const char *ofile, const bool v);
int perf_exec(const char **args);

const  std::string Comment_tmpmesg = "Здесь будет текст поста";

static std::string Comment_tmpfile = "";
static std::string Email_tmpfile   = "";
static std::string Name_tmpfile    = "";
static std::string Trip_tmpfile    = "";

std::string Env_HOME = "";
std::string Env_EDITOR = "";

enum edit_task {
	Task_comment,
	Task_email,
	Task_name,
	Task_trip
};

void setup_env();
void fork_and_edit(std::string &dest, const std::string &filename);
void edit(std::string &dest, const enum edit_task task);
void read(std::string &dest, const enum edit_task task);

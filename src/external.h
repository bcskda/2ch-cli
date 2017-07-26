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

extern const char *Converter;
extern const char *Converter_format;
extern const char *Converter_width_s;
extern const char *Converter_height_s;
extern const int   Converter_width_i;
extern const int   Converter_height_i;

extern const char *CaptchaUtfFilename;

extern const int Ret_show_OK;
extern const int Ret_show_create_canvas;
extern const int Ret_show_import_canvas;
extern const int Ret_show_create_display;

caca_display_t *show_img(const char *filename);
void convert_img(const char *filename, const char *ofile, const bool v);
int perf_exec(const char **args);

extern const  std::string Comment_tmpmesg;

extern std::string Comment_tmpfile;
extern std::string Email_tmpfile;
extern std::string Name_tmpfile;
extern std::string Trip_tmpfile;

extern std::string Env_HOME;
extern std::string Env_EDITOR;

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

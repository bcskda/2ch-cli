// ========================================
// File: parser.h
// API answer parsing functions
// (Headers)
// ========================================

#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <cstddef>
#include <memory>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <json.h>
#include <jsoncpp/json/json.h>
#include "error.h"
#include "makaba.h"
#include "image.h"

struct post_cpp {
	bool isNull;
	bool banned; //bool
	bool closed; //bool
	std::string comment;
	std::string date;
	std::string email;
	Json::Value files;
	long long lasthit;
	std::string name;
	long long num;
	int op; //bool
	long long parent;
	bool sticky; //bool
	std::string subject;
	std::string tags;
	long long timestamp;
	std::string trip;
	std::string trip_type; // enum?
	long long unique_posters;
	long long rel_num;
	post_cpp();
	post_cpp(Json::Value &val);
};
typedef struct post_cpp makaba_post_cpp;

struct thread_cpp {
	bool isNull;
	long int num;
	long int nposts;
	std::string board;
	std::vector<makaba_post_cpp> posts;
	thread_cpp();
	thread_cpp(const char *raw, const char *board);
};
typedef struct thread_cpp makaba_thread_cpp;

struct captcha_2chaptcha {
	char *id;
	int result;
	char value[10] = "";
	char *png_url = NULL; // 108
};
typedef struct captcha_2chaptcha makaba_2chaptcha;

// ========================================
// JSON cache
// ========================================

const size_t Json_cache_buf_size = 2e6;
const char *Json_cache_suff_armed = "active";
char *Json_cache_buf = NULL;
char Json_cache_dir[50] = "";

// ========================================
// libjson
// ========================================

enum context_type {
    //boards_list,
    //board_page,
    //board_catalog,
    thread_new,
    //thread_upd,
    //captcha_settings,
    captcha_id,
    send_post
};
typedef enum context_type context_type;

enum parser_status_thread {
    in_post,
    in_files
};
typedef enum parser_status_thread parser_status_thread;

const int Status_default = -1;
const int Status_in_thread = 0;
const int Status_in_post = 1;
const int Status_in_files = 2;

const char *Key_banned = "banned";
const char *Key_closed = "closed";
const char *Key_comment = "comment";
const char *Key_date = "date";
const char *Key_email = "email";
const char *Key_files = "files";
const char *Key_lasthit = "lasthit";
const char *Key_name = "name";
const char *Key_num = "num";
const char *Key_op = "op";
const char *Key_parent = "parent";
const char *Key_sticky = "sticky";
const char *Key_subject = "subject";
const char *Key_tags = "tags";
const char *Key_timestamp = "timestamp";
const char *Key_trip = "trip";
const char *Key_trip_type = "trip_type";
const char *Key_unique_posters = "unique_posters";

const char *Key_id = "id";
const char *Key_result = "result";
const char *Key_type = "type";

const int Expect_banned = 1;
const int Expect_closed = 2;
const int Expect_comment = 3;
const int Expect_date = 4;
const int Expect_email = 5;
const int Expect_files = 6;
const int Expect_lasthit = 7;
const int Expect_name = 8;
const int Expect_num = 9;
const int Expect_op = 10;
const int Expect_parent = 11;
const int Expect_sticky = 12;
const int Expect_subject = 13;
const int Expect_tags = 14;
const int Expect_timestamp = 15;
const int Expect_trip = 16;
const int Expect_trip_type = 17;
const int Expect_unique_posters = 18;

const int Expect_id = 1;
const int Expect_result = 2;

struct json_context {
    context_type type;
    int status;
    int expect;
	bool verbose;
    void *memdest;
};
typedef struct json_context json_context;

// ========================================
// Ручной парсинг
// ========================================

const char *PATTERN_TAG_OPEN = "<";
const char *PATTERN_TAG_CLOSE = ">";

const char *PATTERN_HREF_OPEN = "<a href=\"";
const char *PATTERN_HREF_CLOSE = "</a>";
const char *PATTERN_REPLY_CLASS = "class=\"post-reply-link\"";
const char *PATTERN_REPLY_THREAD = "data-thread=\"";
const char *PATTERN_REPLY_NUM = "data-num=\"";
const char *PATTERN_NEWLINE = "<br>";
const char *PATTERN_LT = "&lt";
const char *PATTERN_GT = "&gt";
const char *PATTERN_SLASH = "&#47";
const char *PATTERN_BCKSLASH = "&#92";
const char *PATTERN_NBSP = "&nbsp";
const char *PATTERN_SINGLE_QUOT = "&#39";
const char *PATTERN_DOUBLE_QUOT = "&quot";
const char *PATTERN_AMP = "&amp";

const char *CaptchaPngFilename = "/tmp/2ch-captcha.png";
const char *CaptchaUtfFilename = "/tmp/2ch-captcha.utf8";

// ========================================
// Прототипы
// ========================================

int json_callback(void *userdata, int type, const char *data, uint32_t length); // Вызывается парсером при событиях
int fill_post_expected(json_context *context, const char *data); // Определяет текущую переменную JSON
int fill_post_value(makaba_post_cpp &post, const int expect, const char *data,  // Заполняет соотв. поле структуры
	const bool &verbose);
int fill_captcha_id_expected(json_context *context, const char *data);
int fill_captcha_id_value(makaba_2chaptcha *captcha, const int expect,
		const char *data);

int initThread_cpp(makaba_thread_cpp &thread, const char *thread_string,
	const long long &thread_lenght, const bool &verbose);
int updateThread_cpp(makaba_thread_cpp &thread, const bool &verbose);
int prepareThread_cpp(makaba_thread_cpp &thread, const char *board,
	const long long threadnum, const bool &verbose);
int initCaptcha_cpp(makaba_2chaptcha &captcha, const char *board,
	const long long thread, const bool &verbose);
int prepareCaptcha_cpp (makaba_2chaptcha &captcha, const char *board,
	const long long thread, const bool &verbose);

char *parseHTML (const char *raw, const long long  raw_len, const bool v);

int initJsonCache();
bool checkJsonCache(const makaba_thread_cpp &thread);
void armJsonCache(const makaba_thread_cpp &thread);
void disarmJsonCache(const makaba_thread_cpp &thread);
char *readJsonCache(const makaba_thread_cpp &thread, long long *threadsize);
int writeJsonCache(const makaba_thread_cpp &thread, const char *thread_ch);
int cleanJsonCache();

void freePost (makaba_thread_cpp &post);
void freeThread (makaba_thread_cpp &thread);

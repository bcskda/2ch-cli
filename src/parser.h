// ========================================
// File: parser.h
// API answer parsing functions
// (Headers)
// ========================================

#pragma once

#include <iostream>
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
#include "external.h"
#include "external.h"

struct post {
	bool banned;
	bool closed;
	std::string comment;
	std::string date;
	std::string email;
	Json::Value files;
	long long lasthit;
	std::string name;
	long long num;
	bool op;
	long long parent;
	bool sticky;
	std::string subject;
	std::string tags;
	long long timestamp;
	std::string trip;
	std::string trip_type; // enum?
	long long unique_posters;
	long long rel_num;
	post();
	post(const std::string &vcomment, const std::string &vemail,
		 const std::string &vname,    const std::string &vsubject,
		 const std::string &vtags,    const std::string &vtrip);
	post(const char *vcomment, const char *vemail,
		 const char *vname,    const char *vsubject,
		 const char *vtags,    const char *vtrip);
	post(Json::Value &val);
	post(const char *raw); // @TODO
	bool isNull();
private:
	bool isNull_;
};
typedef struct post makaba_post;

struct thread {
	long int num;
	long int nposts;
	std::string board;
	std::vector<makaba_post> posts;
	thread(const std::string &board, const long long &num);
	bool update();
	bool isNull();
private:
	bool isNull_;
	thread();
	bool append(const char *raw);
};
typedef struct thread makaba_thread;

struct captcha_2chaptcha {
	std::string id;
	std::string value;
	std::string error; // @TODO ro
	captcha_2chaptcha(const std::string &board, const long long &threadnum);
	captcha_2chaptcha(const makaba_thread& thread); // @TODO
	bool isNull();
	bool get_png();
private:
	bool isNull_;
    std::string png_url;
	captcha_2chaptcha();
	bool get_id(const std::string &board, const long long &threadnum);
	bool form_url();
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

char *parseHTML(const char *raw, const long long raw_len, const bool v);

int   initJsonCache  ();
bool  checkJsonCache (const makaba_thread &thread);
void  armJsonCache   (const makaba_thread &thread);
void  disarmJsonCache(const makaba_thread &thread);
char *readJsonCache  (const makaba_thread &thread, long long *threadsize);
int   writeJsonCache (const makaba_thread &thread, const char *thread_ch);
int   cleanJsonCache ();

void freePost  (makaba_thread &post);
void freeThread(makaba_thread &thread);

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
#include <json/json.h>
#include "error.h"
#include "makaba.h"
#include "external.h"
#include "external.h"

namespace Makaba {

class Post {
	bool isNull_;
public:
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
	Post();
	Post(const std::string &vcomment, const std::string &vemail,
		 const std::string &vname,    const std::string &vsubject,
		 const std::string &vtags,    const std::string &vtrip);
	Post(const char *vcomment, const char *vemail,
		 const char *vname,    const char *vsubject,
		 const char *vtags,    const char *vtrip);
	Post(Json::Value &val);
	Post(const char *raw); // @TODO
	bool isNull();
};

class Thread {
	bool isNull_;
	Thread();
	int append(const char *raw);
public:
	long long num;
	long long nposts;
	std::string board;
	std::vector<Makaba::Post> posts;
	Thread(const std::string &board, const long long &num);
	int update();
	bool isNull();
};

class Captcha_2ch {
	bool isNull_;
	std::string png_url;
	Captcha_2ch();
	int get_id(const std::string &board, const long long &threadnum);
	int form_url();
public:
	std::string id;
	std::string value;
	std::string error; // @TODO ro
	Captcha_2ch(const std::string &board, const long long &threadnum);
	Captcha_2ch(const Makaba::Thread& thread); // @TODO
	bool isNull();
	int get_png();
};

} // end namespace Makaba

// ========================================
// JSON cache
// ========================================

extern const size_t Json_cache_buf_size;
extern const char *Json_cache_suff_armed;
extern char *Json_cache_buf;
extern char Json_cache_dir[100];

// ========================================
// Ручной парсинг
// ========================================

extern const char *PATTERN_TAG_OPEN;
extern const char *PATTERN_TAG_CLOSE;

extern const char *PATTERN_HREF_OPEN;
extern const char *PATTERN_HREF_CLOSE;
extern const char *PATTERN_REPLY_CLASS;
extern const char *PATTERN_REPLY_THREAD;
extern const char *PATTERN_REPLY_NUM;
extern const char *PATTERN_NEWLINE;
extern const char *PATTERN_LT;
extern const char *PATTERN_GT;
extern const char *PATTERN_SLASH;
extern const char *PATTERN_BCKSLASH;
extern const char *PATTERN_NBSP;
extern const char *PATTERN_SINGLE_QUOT;
extern const char *PATTERN_DOUBLE_QUOT;
extern const char *PATTERN_AMP;

extern const char *CaptchaPngFilename;
extern const char *CaptchaUtfFilename;

// ========================================
// Прототипы
// ========================================

char *parseHTML(const char *raw, const long long raw_len, const bool v);

int   initJsonCache  ();
bool  checkJsonCache (const Makaba::Thread &thread);
void  armJsonCache   (const Makaba::Thread &thread);
void  disarmJsonCache(const Makaba::Thread &thread);
char *readJsonCache  (const Makaba::Thread &thread, long long *threadsize);
int   writeJsonCache (const Makaba::Thread &thread, const char *thread_ch);
int   cleanJsonCache ();

void freePost  (Makaba::Thread &post);
void freeThread(Makaba::Thread &thread);

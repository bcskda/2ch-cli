// ========================================
// File: parser.h
// Parsing functions
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
#include "error.h"
#include "external.h"

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

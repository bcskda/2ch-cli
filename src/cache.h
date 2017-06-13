// ========================================
// File: cache.h
// Manage raw JSON cache
// (Headers)
// ========================================

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include "error.h"
#include "makaba.h"
#include "external.h"
#pragma once


// ========================================
// Ручной парсинг
// ========================================


extern const size_t Json_cache_buf_size;
extern const char *Json_cache_suff_armed;
extern char *Json_cache_buf;
extern char Json_cache_dir[100];

int   initJsonCache  ();
bool  checkJsonCache (const Makaba::Thread &thread);
void  armJsonCache   (const Makaba::Thread &thread);
void  disarmJsonCache(const Makaba::Thread &thread);
char *readJsonCache  (const Makaba::Thread &thread, long long *threadsize);
int   writeJsonCache (const Makaba::Thread &thread, const char *thread_ch);
int   cleanJsonCache ();

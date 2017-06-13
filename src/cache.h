// ========================================
// JSON cache
// ========================================

#pragma once
#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include <memory>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include "error.h"
#include "parser.h"

extern const size_t Json_cache_buf_size;
extern const char *Json_cache_suff_armed;
extern char *Json_cache_buf;
extern char Json_cache_dir[100];

int   initJsonCache  ();
bool  checkJsonCache (const Makaba::Thread &thread);
void  armJsonCache   (const Makaba::Thread &thread);
void  disarmJsonCache(const Makaba::Thread &thread);
char *readJsonCache  (const std::string &board, const long long &tnum,
					  long long *threadsize);
int   writeJsonCache (const std::string &board, const long long &tnum,
					  const char *raw);
int   cleanJsonCache ();
 

// ========================================
// File: parser.h
// Parsing functions
// (Headers)
// ========================================

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include "error.h"
#include "external.h"
#pragma once

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


char *parseHTML(const char *raw, const long long raw_len, const bool v);


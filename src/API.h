// ========================================
// File: API.h
// Makaba REST API-related
// (Headers)
// ========================================

#pragma once

#include <vector>
#include <string>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <curl/curl.h>
#include "error.h"

typedef std::vector< std::pair<std::string, std::string> > Postfields;

// Global defs

extern const char *BASE_URL;
extern const char *MOBILE_API;
extern const char *POSTING_API;
extern const char *POSTING_FIELDS;
extern const char *CAPTCHA_SETTINGS;
extern const char *CAPTCHA_2CHAPTCHA;
extern const size_t CURL_BUFF_BODY_SIZE;
extern const size_t CURL_BUFF_HEADER_SIZE;
extern const size_t COMMENT_LEN_MAX;
extern char *CURL_BUFF_BODY;
extern char *CURL_BUFF_HEADER;
extern size_t CURL_BUFF_POS;

// End Global defs

size_t CURL_writeToBuff (const char *src, const size_t size, const size_t nmemb, void *dest);
void makabaSetup();
void makabaCleanup();
char *callAPI(const char *url, const char *post, long long *size, const bool v);

char *getBoardsList(const bool v);
char *getBoardPage(const char *board, const long long page, const bool v);
char *getBoardCatalog(const char *board, const bool v);
char *getThread(const char *board, const long long threadn,
	const long long postn_rel, long long *threadsize, const bool v);
// Важно и неочевидно: postnum_rel - относительный номер поста в треде

char *getCaptchaSettings(const char *board);
char *get2chaptchaId(const char *board, const long long threadn, const bool v);
char *get2chaptchaPicPNG(const char *url, long long *pic_size);

char *sendPost (const char *board, const long long threadn,
				const char *comment, const char *subject,
				const char *name, const char *email,
				const char *captcha_id, const char *captcha_value);

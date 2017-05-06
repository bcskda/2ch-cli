// ========================================
// File: makaba.h
// Makaba API-related functions
// (Headers)
// ========================================

#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cmath>
#include <curl/curl.h>
#include <stdbool.h>
#include "error.h"

const char *BASE_URL = "https://2ch.hk";
const char *MOBILE_API = "makaba/mobile.fcgi";
const char *POSTING_API = "makaba/posting.fcgi";
const char *POSTING_FIELDS = "json=1&task=post&captcha_type=2chaptcha";
const char *CAPTCHA_SETTINGS = "api/captcha/settings";
const char *CAPTCHA_2CHAPTCHA = "api/captcha/2chaptcha";
const size_t CURL_BUFF_BODY_SIZE = 2e6;
const size_t CURL_BUFF_HEADER_SIZE = 2000;
const long long COMMENT_LEN_MAX = 15e3;
// Юзерагент  - макрос CURL_UA в мейкфайле
char *CURL_BUFF_BODY = NULL;
char *CURL_BUFF_HEADER = NULL;
size_t CURL_BUFF_POS = 0;

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
char *form2chaptchaPicURL(const char *id);
char *get2chaptchaPicPNG(const char *url, long long *pic_size);

char *sendPost (const char *board, const long long threadn,
	const char *comment, const char *subject, const char *name, const char *email,
	const char *captcha_id, const char *captcha_value, long long *answer_length);

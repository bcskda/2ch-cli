// ========================================
// File: makaba.h
// Makaba API-related functions
// (Headers)
// ========================================

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <stdbool.h>

const char* BASE_URL = "https://2ch.hk/";
const char* MOBILE_API = "makaba/mobile.fcgi";
const char* CAPTCHA_SETTINGS = "api/captcha/settings/";
const char* CAPTCHA_2CHAPTCHA = "api/captcha/2chaptcha/";
const size_t CURL_BUFF_BODY_SIZE = 5000000;
const size_t CURL_BUFF_HEADER_SIZE = 2000;
char* CURL_BUFF_BODY = NULL;
char* CURL_BUFF_HEADER = NULL;
size_t CURL_BUFF_POS = 0;

const int ERR_MEMORY = -1,
		  ERR_UNKNOWN = -2,
		  ERR_CURL_INIT = -3,
		  ERR_CURL_PERFORM = -4,
		  ERR_MAKABA_SETUP = -5;

void makabaSetup();
void makabaCleanup();

char* getBoardsListJSON (const bool v);
char* getBoardPageJSON (const char* board, const long int page, bool v);
char* getBoardCatalogJSON (const char* board, const bool v);
char* getThreadJSON (const char* board, const long int threadnum, long int* threadsize, const bool v);

char* getCaptchaSettingsJSON (const char* board);
char* get2chaptchaIdJSON (const char* board, const char* thread);
char* get2chaptchaPicURL (const char* id);

size_t CURL_writeToBuff (const char* src, const size_t size, const size_t nmemb, void* dest);
char* lint2str (const long int val);
long int str2lint (const char* str, const long int len);

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
const char* CAPTCHA_API = "api/captcha/";
const size_t CURL_BUFF_BODY_SIZE = 5000000;
const size_t CURL_BUFF_HEADER_SIZE = 2000;
char* CURL_BUFF_BODY = NULL;
char* CURL_BUFF_HEADER = NULL;
size_t CURL_BUFF_POS = 0;

const int ERR_MEMORY = -1,
		  ERR_UNKNOWN = -2,
		  ERR_CURL_INIT = -3,
		  ERR_CURL_PERFORM = -4;

void makabaSetup();
void makabaCleanup();

int getBoardsList (const char* resFile, const bool v);
char* getBoardPageJSON (const char* board, const unsigned page, bool v);
char* getBoardCatalogJSON (const char* board, const bool v);
char* getThreadJSON (const char* board, const unsigned threadnum, const bool v);

char* getCaptchaSettings (const char* board, const bool v);
int getCaptchaID (const char* board, const unsigned threadnum, const bool v);

size_t CURL_writeToBuff (const char* src, const size_t size, const size_t nmemb, void* dest);
char* unsigned2str (const unsigned val);
unsigned str2unsigned (const char* str, const unsigned len);
// ========================================
// File: makaba.h
// Makaba API-related functions
// (Headers)
// ========================================

#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <stdbool.h>

struct list {
	void* data;
	struct list* first;
	struct list* next;
};

struct ref_reply {
	char* link;
	unsigned thread;
	unsigned num;
};

struct comment {
	char* text;
	struct ref_reply* refs;
	unsigned nrefs;};

struct post {
	unsigned num;
	struct comment* comment;
	char* date;
	char* name;
	char* email;
	char* files;
};

const char* BASE_URL = "https://2ch.hk/";
const char* MOBILE_API = "makaba/mobile.fcgi";
const size_t CURL_BUFF_BODY_SIZE = 5000000;
const size_t CURL_BUFF_HEADER_SIZE = 2000;
char* CURL_BUFF_BODY = 0;
char* CURL_BUFF_HEADER = 0;
size_t CURL_BUFF_POS = 0;

const char* PATTERN_COMMENT = ",\"comment\":\"";
const char* PATTERN_DATE = ",\"date\":\"";
const char* PATTERN_SUBJECT = ",\"subject\":\"";
const char* PATTERN_NAME = ",\"name\":\"";
const char* PATTERN_EMAIL = ",\"email\":\"";
const char* PATTERN_FILES = ",\"files\":[{";
const char* PATTERN_NUM = ",\"num\":\"";

const char* PATTERN_HREF_OPEN = "\\u003ca href=\\\"";
const char* PATTERN_HREF_CLOSE = "\\u003c/a\\u003e";
const char* PATTERN_REPLY_CLASS = "class=\\\"post-reply-link\\\"";
const char* PATTERN_REPLY_THREAD = "data-thread=\\\"";
const char* PATTERN_REPLY_NUM = "data-num=\\\"";
const char* PATTERN_GREEN = "span class=\\\"unkfunc\\\"";
const char* PATTERN_NEWLINE = "\\u003cbr\\u003e";

const int ERR_MEMORY_LEAK = -1,
					ERR_UNKNOWN = -2,
					ERR_CURL_INIT = -3,
					ERR_CURL_PERFORM = -4,
					ERR_PARTTHREAD_DEPTH = -5,
					ERR_POST_FORMAT = -6,
					ERR_COMMENT_FORMAT = -7,
					ERR_REF_FORMAT = -8;
					ERR_COMMENT_PARSING = -9;

int getBoardsList (const char* resFile, const bool v);
char* getBoardPageJSON (const char* board, const unsigned page, bool v);
char* getBoardCatalogJSON (const char* board, const bool v);
char* getThreadJSON (const char* board, const unsigned threadnum, const bool v);

int* findPostsInJSON (const char* src, int* postcount_res, const bool v);
struct post* initPost (const char* post_string, const short postlen, const bool v);
struct comment* parseComment (char* comment, const bool v);
char* cleanupComment (const char* src, const unsigned src_len, const bool v);
struct ref_reply* parseRef_Reply (const char* ch_ref, const int ref_len, const bool v);

void freeRefReply (struct ref_reply* ref);
void freePost (struct post* post);
void freeComment (struct comment* arg);

size_t CURL_writeToBuff (const char* src, const size_t size, const size_t nmemb, void* dest);
char* unsigned2str (const unsigned val);
unsigned str2unsigned (const char* str, const unsigned len);
// ========================================
// File: parser.h
// API answer parsing functions
// (Headers)
// ========================================

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#pragma once

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

struct thread {
	unsigned num;
	unsigned nposts;
	struct post** posts;
};

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

const char* PATTERN_CAPID = "\"id\":\"";

const int ERR_PARTTHREAD_DEPTH = -1,
		  ERR_POST_FORMAT = -2,
		  ERR_COMMENT_FORMAT = -3,
		  ERR_REF_FORMAT = -4;
		  ERR_COMMENT_PARSING = -5,
		  ERR_CAPTCHA_FORMAT = -6;

struct thread* initThread (const char* thread_string, const unsigned thread_len, const bool v);
unsigned* findPostsInJSON (const char* src, unsigned* postcount_res, const bool v);
struct post* initPost (const char* post_string, const unsigned postlen, const bool v);
struct comment* parseComment (char* comment, const bool v);
struct ref_reply* parseRef_Reply (const char* ch_ref, const unsigned ref_len, const bool v);
char* cleanupComment (const char* src, const unsigned src_len, const bool v);

char* parse2chaptchaId (const char* capid_string, const bool v);

void freeRefReply (struct ref_reply* ref);
void freePost (struct post* post);
void freeComment (struct comment* arg);
void freeThread (struct thread* thread);
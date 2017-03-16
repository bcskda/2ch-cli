// ========================================
// File: parser.h
// API answer parsing functions
// (Headers)
// ========================================

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#pragma once

struct list {
	void *data;
	struct list *first;
	struct list *next;
};

struct ref_reply {
	char *link;
	long int thread;
	long int num;
};

struct post {
	long int num;
	char *comment;
	char *date;
	char *name;
	char *email;
	char *files;
};

struct thread {
	long int num;
	long int nposts;
	struct post **posts;
};

const char* PATTERN_COMMENT = ",\"comment\":\"";
const char* PATTERN_COMMENT_END = "\",\"date\":\"";
const char* PATTERN_DATE = ",\"date\":\"";
const char* PATTERN_SUBJECT = ",\"subject\":\"";
const char* PATTERN_NAME = ",\"name\":\"";
const char* PATTERN_NAME_END = "\",\"num\":\"";
const char* PATTERN_EMAIL = ",\"email\":\"";
const char* PATTERN_FILES = ",\"files\":[{";
const char* PATTERN_NUM = ",\"num\":\"";

const char* PATTERN_TAG_OPEN = "\\u003c";
const char* PATTERN_TAG_CLOSE = "\\u003e";

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
		  ERR_CAPTCHA_FORMAT = -6,
		  ERR_INTERNAL = -7,
		  ERR_POST_OUT_OF_RANGE = -8;

struct thread *initThread (const char *thread_string, const long int thread_len, const bool v);
long int *findPostsInJSON (const char *src, long int *postcount_res, const bool v);
struct post *initPost (const char *post_string, const long int postlen, const bool v);
char *parseComment (char *comment, const long long  comment_len, const bool v);
struct ref_reply *parseRef_Reply (const char *ch_ref, const long int ref_len, const bool v);
char *cleanupComment (const char *src, const int src_len, int *new_len, const bool v);

char *parse2chaptchaId (const char *capid_string, const bool v);

void freeRefReply (struct ref_reply *ref);
void freePost (struct post *post);
void freeComment (char *arg);
void freeThread (struct thread *thread);

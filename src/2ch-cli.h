#include "makaba.h"
#include <ncurses.h>
#include <locale.h>

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
	struct comment* comment;
	char* date;
	char* name;
	char* email;
	char* files;
};

const char* PATTERN_COMMENT = ",\"comment\":\"";
const char* PATTERN_DATE = ",\"date\":\"";
const char* PATTERN_SUBJECT = ",\"subject\":\"";
const char* PATTERN_NAME = ",\"name\":\"";
const char* PATTERN_EMAIL = ",\"email\":\"";
const char* PATTERN_FILES = ",\"files\":[{";

const char* PATTERN_HREF_OPEN = "\\u003ca href=\\\"";
const char* PATTERN_HREF_CLOSE = "\\u003c/a\\u003e";
const char* PATTERN_REPLY_CLASS = "class=\\\"post-reply-link\\\"";
const char* PATTERN_REPLY_THREAD = "data-thread=\\\"";
const char* PATTERN_REPLY_NUM = "data-num=\\\"";
const char* PATTERN_GREEN = "span class=\\\"unkfunc\\\"";
const char* PATTERN_NEWLINE = "\\u003cbr\\u003e";


struct post* initPost (const char* post_string, const short postlen, const bool v);
struct ref_reply* parseRef_Reply (const char* ch_ref, const int ref_len, const bool v);
struct comment* parseComment (char* comment, const bool v);
char* cleanupComment (const char* src, const unsigned src_len, const bool v);

int printPost (struct post* post,const bool show_email,const bool show_files);

void freeRefReply (struct ref_reply* ref);
void freePost (struct post* post);
void freeComment (struct comment* arg);
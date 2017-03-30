// ========================================
// File: parser.h
// API answer parsing functions
// (Headers)
// ========================================

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <vector>
#include <json.h>
#include "error.h"
#pragma once

struct post_cpp {
	int banned; //bool
	int closed; //bool
	char *comment;
	char *date;
	char *email;
	// files
	long long lasthit;
	char *name;
	long long num;
	int op; //bool
	long long parent;
	int sticky; //bool
	char *subject;
	char *tags;
	long long timestamp;
	char *trip;
	char *trip_type; // enum?
	long long unique_posters;
};
typedef struct post_cpp makaba_post_cpp;

struct thread_cpp {
	long int num;
	long int nposts;
	std::vector<makaba_post_cpp> posts;
};
typedef struct thread_cpp makaba_thread_cpp;

// ========================================
// libjson
// ========================================

enum context_type {
    //boards_list,
    //board_page,
    //board_catalog,
    thread_new,
    //thread_upd,
    //captcha_settings,
    captcha_id,
    send_post
};
typedef enum context_type context_type;

enum parser_status_thread {
    in_post,
    in_files
};
typedef enum parser_status_thread parser_status_thread;

const int Status_default = -1;
const int Status_in_thread = 0;
const int Status_in_post = 1;
const int Status_in_files = 2;

const char *Key_banned = "banned";
const char *Key_closed = "closed";
const char *Key_comment = "comment";
const char *Key_date = "date";
const char *Key_email = "email";
const char *Key_files = "files";
const char *Key_lasthit = "lasthit";
const char *Key_name = "name";
const char *Key_num = "num";
const char *Key_op = "op";
const char *Key_parent = "parent";
const char *Key_sticky = "sticky";
const char *Key_subject = "subject";
const char *Key_tags = "tags";
const char *Key_timestamp = "timestamp";
const char *Key_trip = "trip";
const char *Key_trip_type = "trip_type";
const char *Key_unique_posters = "unique_posters";

const char *Keys_int[] = {
    Key_banned,
    Key_closed,
    Key_lasthit,
    Key_op,
    Key_sticky,
    Key_timestamp,
    Key_unique_posters
};
const char *Keys_string[] = {
    Key_comment,
    Key_date,
    Key_email,
    Key_name,
    Key_num,
    Key_parent,
    Key_subject,
    Key_trip,
    Key_trip_type
};

const int Expect_banned = 1;
const int Expect_closed = 2;
const int Expect_comment = 3;
const int Expect_date = 4;
const int Expect_email = 5;
const int Expect_files = 6;
const int Expect_lasthit = 7;
const int Expect_name = 8;
const int Expect_num = 9;
const int Expect_op = 10;
const int Expect_parent = 11;
const int Expect_sticky = 12;
const int Expect_subject = 13;
const int Expect_tags = 14;
const int Expect_timestamp = 15;
const int Expect_trip = 16;
const int Expect_trip_type = 17;
const int Expect_unique_posters = 18;

struct json_context {
    context_type type;
    int status;
    int expect;
    void *memdest;
};
typedef struct json_context json_context;

// ========================================
// Ручной парсинг
// ========================================

const char *PATTERN_TAG_OPEN = "<";
const char *PATTERN_TAG_CLOSE = ">";

const char *PATTERN_HREF_OPEN = "<a href=\"";
const char *PATTERN_HREF_CLOSE = "</a>";
const char *PATTERN_REPLY_CLASS = "class=\"post-reply-link\"";
const char *PATTERN_REPLY_THREAD = "data-thread=\"";
const char *PATTERN_REPLY_NUM = "data-num=\"";
const char *PATTERN_NEWLINE = "<br>";
const char *PATTERN_GT = "&gt";
const char *PATTERN_SLASH = "&#47";

const char *PATTERN_CAPID = "\"id\":\"";

// ========================================
// Прототипы
// ========================================

int json_callback(void *userdata, int type, const char *data, uint32_t length); // Вызывается парсером при событиях
int fill_expected(json_context *context, const char *data); // Определяет текущую переменную JSON
int fill_as_string(makaba_post_cpp &post, const int expect, const char *data); // Заполняют поле в структуре
int fill_as_int(makaba_post_cpp &post, const int expect, const char *data);    // в соотв. с текущей переменной JSON

int initThread_cpp(makaba_thread_cpp &thread, const char *thread_string, const long long thread_lenght, const bool v);

char *parseComment (const char *comment, const long long  comment_len, const bool v);

char *parse2chaptchaId (const char *capid_string);

void freeRefReply (struct ref_reply *ref);
void freePost (struct post *post);
void freeComment (char *arg);
void freeThread (struct thread *thread);

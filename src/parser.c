// ========================================
// File: parser.c
// API answer parsing functions
// (Implementation)
// ========================================

#pragma once
#include "parser.h"

char *parseComment (const char *comment, const long long  comment_len, const bool v) { // Пока что игнорируем разметку
	fprintf (stderr, "]] Started parseComment\n");
	fprintf(stderr, "Comment len: %d\n", comment_len);

	if (v) {
		fprintf(stderr, "Comment:\n%s\nEnd of comment\n", comment);
	}

	char *parsed = (char *) calloc(comment_len, sizeof(char));
	int depth = 0;
	long long parsed_len = 0;
	long long i;

	for (i = 0; i < comment_len; i++) {
		if (strncmp(&(comment[i]), PATTERN_TAG_OPEN, strlen(PATTERN_TAG_OPEN)) == 0) { // HTML tag open
			/*if (strncmp(&(comment[i]), , strlen(PATTERN_NEWLINE)) == 0) { // Post answer

			}
			else*/ {
				depth ++;
				if (strncmp(&(comment[i]), PATTERN_NEWLINE, strlen(PATTERN_NEWLINE)) == 0) { // <br>
					if (v) fprintf(stderr, "NL ");
					parsed[parsed_len] = '\n';
					parsed_len ++;
				}
				i += strlen(PATTERN_TAG_OPEN) - 1;
			}
		}
		else
			if (strncmp(&(comment[i]), PATTERN_TAG_CLOSE, strlen(PATTERN_TAG_CLOSE)) == 0) { // '>' char
					if (depth > 0) { // HTML tag close
						depth --;
					}
					else { // text '>'
						parsed[parsed_len] = '>';
						parsed_len ++;
					}
					i += strlen(PATTERN_TAG_CLOSE) - 1;
			}
			else {
				if (strncmp(&(comment[i]), PATTERN_GT, strlen(PATTERN_GT)) == 0) { // In-text '>'
					if(v) fprintf(stderr, "> ");
					parsed[parsed_len] = '>';
					parsed_len ++;
					i += strlen(PATTERN_GT);
				}
				else
					if (depth == 0) { // Ordinary character
						parsed[parsed_len] = comment[i];
						parsed_len ++;
					}
			}
	}
	fprintf(stderr, "]] Final length: %d\n", parsed_len);
	fprintf(stderr, "]] Exiting parseComment\n");
	return parsed;
}

// ========================================
// Captcha
// ========================================

char *parse2chaptchaId (const char *capId_string) {
	fprintf (stderr, "]] Starting parse2chaptchaId\n");

	char *captcha_start = (char *) strstr (capId_string, PATTERN_CAPID);
	if (captcha_start == NULL) {
		fprintf(stderr, "[parse2chaptchaId]! Error: Bad captcha-JSON format\n");
		makaba_errno = ERR_CAPTCHA_FORMAT;
		return NULL;
	}
	captcha_start += strlen(PATTERN_CAPID);
	short captcha_len = ((char *) strstr(captcha_start,"\"")) - captcha_start;
	char *captcha_id = (char*) calloc (captcha_len, sizeof(char));
	memcpy (captcha_id, captcha_start, captcha_len);

	fprintf(stderr, "]] Exiting parse2chaptchaId\n");

	return captcha_id;
}

// ========================================
// libjson
// ========================================

int json_callback(void *userdata, int type, const char *data, uint32_t length) {
    json_context *context = (json_context *) userdata;
    if (context->type == thread_new) {
        makaba_thread_cpp *thread = (makaba_thread_cpp *)context->memdest;
        switch (type) {
            case JSON_KEY:
                if (context->status == Status_in_post) {
                    if (fill_expected(context, (char *)data)) {
                        printf("! Error @ fill_expected()\n");
                        return 1;
                    }
                }
                // fill files later
                break;
            case JSON_ARRAY_BEGIN:
                switch(context->status) {
                    case Status_default:
                        //fprintf(stderr, ">>> Entering posts array, Status_in_thread\n");
                        context->status = Status_in_thread;
                        break;
                    case Status_in_post:
                        //fprintf(stderr, ">>> Entering files array, Status_in_files\n");
                        context->status = Status_in_files;
                        break;
                }
                break;
            case JSON_ARRAY_END:
                switch(context->status) {
                    case Status_in_thread:
                        //fprintf(stderr, "<<< Exiting posts array, Status_default\n");
                        context->status = Status_default;
                        fprintf(stderr, "==== Vector has %ld elements\n", thread->posts.size());
                        break;
                    case Status_in_files:
                        //fprintf(stderr, "]\n<<< Exiting files array, Status_in_post\n");
                        context->status = Status_in_post;
                        break;
                }
                break;
            case JSON_OBJECT_BEGIN:
                switch (context->status) {
                    case Status_in_thread:
                        //fprintf(stderr, ">>> Entering post, Status_in_post\n");
                        context->status = Status_in_post;
                        thread->nposts++;
                        thread->posts.push_back({ });
                        break;
                }
                break;
            case JSON_OBJECT_END:
                switch (context->status) {
                    case Status_in_post:
                        //printf("<<< Exiting post, Status_in_thread\n");
                        context->status = Status_in_thread;
                        break;
                }
                break;
            case JSON_STRING:
                if (context->status == Status_in_post) {
                    //fprintf(stderr, "Will fill as string %d\n", context->expect);
                    if (fill_as_string(thread->posts[thread->posts.size() - 1], context->expect, data)) {
                        fprintf(stderr, "! Error @ fill_as_string\n");
                        return 1;
                    }
                }
                break;
            case JSON_INT:
            case JSON_FLOAT:
                if (context->status == Status_in_post) {
                    //fprintf(stderr, "Will fill as int %d\n", context->expect);
                    if (fill_as_int(thread->posts[thread->posts.size() - 1], context->expect, data)) {
                        fprintf(stderr, "! Error @ fill_as_int\n");
                        return 1;
                    }
                }
                break;
            default:
                printf("[unhandled] \"%s\"\n", (char *)userdata);
                break;
        }
        return 0;
    }
}

int fill_expected(json_context *context, const char *data) {
    //fprintf(stderr, "Will expect %s\n", data);
    if (strncmp(data, Key_banned, strlen(Key_banned)) == 0) {
        context->expect = Expect_banned;
    }
    else if (strncmp(data, Key_closed, strlen(Key_closed)) == 0) {
        context->expect = Expect_closed;
    }
    else if (strncmp(data, Key_comment, strlen(Key_comment)) == 0) {
        context->expect = Expect_comment;
    }
    else if (strncmp(data, Key_date, strlen(Key_date)) == 0) {
        context->expect = Expect_date;
    }
    else if (strncmp(data, Key_email, strlen(Key_email)) == 0) {
        context->expect = Expect_email;
    }
    else if (strncmp(data, Key_files, strlen(Key_files)) == 0) {
        context->expect = Expect_files;
    }
    else if (strncmp(data, Key_lasthit, strlen(Key_lasthit)) == 0) {
        context->expect = Expect_lasthit;
    }
    else if (strncmp(data, Key_name, strlen(Key_name)) == 0) {
        context->expect = Expect_name;
    }
    else if (strncmp(data, Key_num, strlen(Key_num)) == 0) {
        context->expect = Expect_num;
    }
    else if (strncmp(data, Key_op, strlen(Key_op)) == 0) {
        context->expect = Expect_op;
    }
    else if (strncmp(data, Key_parent, strlen(Key_parent)) == 0) {
        context->expect = Expect_parent;
    }
    else if (strncmp(data, Key_sticky, strlen(Key_sticky)) == 0) {
        context->expect = Expect_sticky;
    }
    else if (strncmp(data, Key_subject, strlen(Key_subject)) == 0) {
        context->expect = Expect_subject;
    }
    else if (strncmp(data, Key_tags, strlen(Key_tags)) == 0) {
        context->expect = Expect_tags;
    }
    else if (strncmp(data, Key_timestamp, strlen(Key_timestamp)) == 0) {
        context->expect = Expect_timestamp;
    }
    else if (strncmp(data, Key_trip, strlen(Key_trip)) == 0) {
        context->expect = Expect_trip;
    }
    else if (strncmp(data, Key_trip_type, strlen(Key_trip_type)) == 0) {
        context->expect = Expect_trip_type;
    }
    else if (strncmp(data, Key_unique_posters, strlen(Key_unique_posters)) == 0) {
        context->expect = Expect_unique_posters;
    }
    else {
        fprintf(stderr, "! Error @ fill_expected: unknown key %s\n", data);
        return 1;
    }
    return 0;
}

int fill_as_int(makaba_post_cpp &post, const int expect, const char *data) {
    switch (expect) {
        case Expect_banned:
            post.banned = atoi(data);
            return 0;
        case Expect_closed:
            post.closed = atoi(data);
            return 0;
        case Expect_lasthit:
            post.lasthit = atoi(data);
            return 0;
        // libjson определяет длинные номера постов как строки;
        // парсятся в fill_as_string()
        /*
        case Expect_num:
        case Expect_parent:
        */
        case Expect_op:
            post.op = atoi(data);
            return 0;
        case Expect_sticky:
            post.sticky = atoi(data);
            return 0;
        case Expect_timestamp:
            post.timestamp = atoi(data);
            return 0;
        // unique_posters тоже как string
        /*
        case Expect_unique_posters:
        */
    }
    return 1;
}

int fill_as_string(makaba_post_cpp &post, const int expect, const char *data) {
	switch (expect) {
        //fprintf(stderr, "Expect %d ...\n", expect);
        case Expect_comment:
            post.comment = parseComment(data, strlen(data), true);
			if (post.comment == NULL) {
				fprintf(stderr, "[fill_as_string] ! Error @ parseComment()\n");
				return 1;
			}
            return 0;
        case Expect_date:
            post.date = (char *) calloc(strlen(data) + 1, sizeof(char));
            memcpy(post.date, data, strlen(data));
            return 0;
        case Expect_email:
            post.email = (char *) calloc(strlen(data) + 1, sizeof(char));
            memcpy(post.email, data, strlen(data));
            return 0;
        case Expect_name:
            post.name = (char *) calloc(strlen(data) + 1, sizeof(char));
            memcpy(post.name, data, strlen(data));
            return 0;
        case Expect_num: // libjson определяет длинные номера постов как строки:
            post.num = atoi(data);
            return 0;
        case Expect_parent: // это тоже номер поста
            post.parent = atoi(data);
            return 0;
        case Expect_subject:
            post.subject = (char *) calloc(strlen(data) + 1, sizeof(char));
            memcpy(post.subject, data, strlen(data));
            return 0;
        case Expect_tags:
            post.tags = (char *) calloc(strlen(data) + 1, sizeof(char));
            memcpy(post.tags, data, strlen(data));
            return 0;
        case Expect_trip:
            post.trip = (char *) calloc(strlen(data) + 1, sizeof(char));
            memcpy(post.trip, data, strlen(data));
            return 0;
        case Expect_trip_type:
            post.trip_type = (char *) calloc(strlen(data) + 1, sizeof(char));
            memcpy(post.trip_type, data, strlen(data));
            return 0;
        case Expect_unique_posters: // unique_posters тоже как string
            post.unique_posters = atoi(data);
            return 0;
    }
    fprintf(stderr, "! Error @ fill_as_string: unknown context.expect value: %d\n", expect);
    return 1;
}

int initThread_cpp(makaba_thread_cpp &thread, const char *thread_string, const long long thread_lenght, const bool v) {
	json_context context;
    context.type = thread_new;
    context.status = Status_default;
    context.memdest = &thread;

	json_parser parser;
    if (json_parser_init(&parser, NULL, json_callback, &context)) {
        fprintf(stderr, "[initThread_cpp] ! Error: json_parser_init() failed\n");
		makaba_errno = ERR_JSON_INIT;
		return 1;
    }

	int ret = json_parser_string(&parser, thread_string, thread_lenght, NULL);
	if (ret) {
		printf("Error @ parse: %d\n", ret);
        json_parser_free(&parser);
		makaba_errno = ERR_JSON_PARSE;
		return 1;
    }

	return 0;
};

// ========================================
// Freeing functions
// ========================================

// @TODO

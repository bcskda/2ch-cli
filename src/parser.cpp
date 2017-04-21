// ========================================
// File: parser.cpp
// API answer parsing functions
// (Implementation)
// ========================================

#include "parser.h"

char *parseHTML (const char *raw, const long long  raw_len, const bool v) { // Пока что игнорируем разметку

	if (v) {
		fprintf(stderr, "]] Started parseHTML\n");
		fprintf(stderr, "Raw len: %d\n", raw_len);
		fprintf(stderr, "Raw:\n%s\nEnd of raw\n", raw);
	}

	char *parsed = (char *) calloc(raw_len, sizeof(char));
	int depth = 0;
	long long parsed_len = 0;
	long long i;

	for (i = 0; i < raw_len; i++) {
		if (strncmp(&(raw[i]), PATTERN_TAG_OPEN, strlen(PATTERN_TAG_OPEN)) == 0) { // HTML tag open
			/*if (strncmp(&(raw[i]), , strlen(PATTERN_NEWLINE)) == 0) { // Post answer
				// @TODO
			}
			else*/ {
				depth ++;
				if (strncmp(&(raw[i]), PATTERN_NEWLINE, strlen(PATTERN_NEWLINE)) == 0) { // <br>
					if (v) fprintf(stderr, "NL ");
					parsed[parsed_len] = '\n';
					parsed_len ++;
				}
				i += strlen(PATTERN_TAG_OPEN) - 1;
			}
			continue;
		}
		if (strncmp(&(raw[i]), PATTERN_TAG_CLOSE, strlen(PATTERN_TAG_CLOSE)) == 0) { // '>' char
			if (depth > 0) { // HTML tag close
				depth --;
			}
			else { // text '>'
				parsed[parsed_len] = '>';
				parsed_len ++;
			}
			i += strlen(PATTERN_TAG_CLOSE) - 1;
			continue;
		}
		if (depth == 0) {
			if (strncmp(&(raw[i]), PATTERN_LT, strlen(PATTERN_LT)) == 0) { // '<' char
			    if (v) fprintf(stderr, "< ");
			    parsed[parsed_len] = '<';
			    parsed_len ++;
			    i += strlen(PATTERN_LT);
			    continue;
			}
			if (strncmp(&(raw[i]), PATTERN_GT, strlen(PATTERN_GT)) == 0) { // '>' char
				if (v) fprintf(stderr, "> ");
				parsed[parsed_len] = '>';
				parsed_len ++;
				i += strlen(PATTERN_GT);
				continue;
			}
			if (strncmp(&(raw[i]), PATTERN_SLASH, strlen(PATTERN_SLASH)) == 0) { // '/' char
				if (v) fprintf(stderr, "/ ");
				parsed[parsed_len] = '/';
				parsed_len ++;
				i += strlen(PATTERN_SLASH);
				continue;
			}
			if (strncmp(&(raw[i]), PATTERN_BCKSLASH, strlen(PATTERN_BCKSLASH)) == 0) { // '\' char
				if (v) fprintf(stderr, "\\ ");
				parsed[parsed_len] = '\\';
				parsed_len ++;
				i += strlen(PATTERN_BCKSLASH);
				continue;
			}
			if (strncmp(&(raw[i]), PATTERN_NBSP, strlen(PATTERN_NBSP)) == 0) { // ' ' char
				if (v) fprintf(stderr, "nbsp ");
				parsed[parsed_len] = ' ';
				parsed_len ++;
				i += strlen(PATTERN_NBSP);
				continue;
			}
			if (strncmp(&(raw[i]), PATTERN_SINGLE_QUOT, strlen(PATTERN_SINGLE_QUOT)) == 0) { // '\'' char
				if (v) fprintf(stderr, "\' ");
				parsed[parsed_len] = '\'';
				parsed_len ++;
				i += strlen(PATTERN_SINGLE_QUOT);
				continue;
			}
			if (strncmp(&(raw[i]), PATTERN_DOUBLE_QUOT, strlen(PATTERN_DOUBLE_QUOT)) == 0) { // '\"' char
				if (v) fprintf(stderr, "\" ");
				parsed[parsed_len] = '\"';
				parsed_len ++;
				i += strlen(PATTERN_DOUBLE_QUOT);
				continue;
			}
			if (strncmp(&(raw[i]), PATTERN_AMP, strlen(PATTERN_AMP)) == 0) { // '&' char
				if (v) fprintf(stderr, "& ");
				parsed[parsed_len] = '&';
				parsed_len ++;
				i += strlen(PATTERN_AMP);
				continue;
			}
			else { // Ordinary char
				parsed[parsed_len] = raw[i];
				parsed_len ++;
			}
		}
	}
	if (v) fprintf(stderr, "]] Final length: %d\n", parsed_len);
	if (v) fprintf(stderr, "]] Exiting parseHTML\n");
	return parsed;
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
                    if (fill_post_expected(context, (char *)data)) {
                        printf("! Error @ fill_post_expected()\n");
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
                        //fprintf(stderr, "==== Vector has %ld elements\n", thread->posts.size());
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
						thread->posts.back().rel_num = thread->nposts;
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
            case JSON_INT:
            case JSON_FLOAT:
                if (context->status == Status_in_post) {
                    if (fill_post_value(thread->posts[thread->posts.size() - 1],
						context->expect, data, context->verbose)) {
                        fprintf(stderr, "! Error @ fill_post_value\n");
                        return 1;
                    }
                }
                break;
            default:
                fprintf(stderr, "[unhandled] \"%s\"\n", (char *)userdata);
                break;
        }
        return 0;
    }
	else if (context->type == captcha_id) {
		makaba_2chaptcha *captcha = (makaba_2chaptcha *)context->memdest;
		switch (type) {
			case JSON_KEY:
				if (fill_captcha_id_expected(context, (char *)data)) {
					fprintf(stderr, "! Error @ fill_captcha_id_expected()\n");
					return 1;
				}
				return 0;
			case JSON_INT:
			case JSON_FLOAT:
			case JSON_STRING:
				if (fill_captcha_id_value(captcha, context->expect, (char *)data)) {
					fprintf(stderr, "! Error @ fill_captcha_id_value\n");
					return 1;
				}
				return 0;
		}
		return 0;
	}
}

int fill_post_expected(json_context *context, const char *data)
{
    if (strncmp(data, Key_banned, strlen(data))== 0) {
        context->expect = Expect_banned;
    }
    else if (strncmp(data, Key_closed, strlen(data))== 0) {
        context->expect = Expect_closed;
    }
    else if (strncmp(data, Key_comment, strlen(data))== 0) {
        context->expect = Expect_comment;
    }
    else if (strncmp(data, Key_date, strlen(data))== 0) {
        context->expect = Expect_date;
    }
    else if (strncmp(data, Key_email, strlen(data))== 0) {
        context->expect = Expect_email;
    }
    else if (strncmp(data, Key_files, strlen(data))== 0) {
        context->expect = Expect_files;
    }
    else if (strncmp(data, Key_lasthit, strlen(data))== 0) {
        context->expect = Expect_lasthit;
    }
    else if (strncmp(data, Key_name, strlen(data))== 0) {
        context->expect = Expect_name;
    }
    else if (strncmp(data, Key_num, strlen(data))== 0) {
        context->expect = Expect_num;
    }
    else if (strncmp(data, Key_op, strlen(data))== 0) {
        context->expect = Expect_op;
    }
    else if (strncmp(data, Key_parent, strlen(data))== 0) {
        context->expect = Expect_parent;
    }
    else if (strncmp(data, Key_sticky, strlen(data))== 0) {
        context->expect = Expect_sticky;
    }
    else if (strncmp(data, Key_subject, strlen(data))== 0) {
        context->expect = Expect_subject;
    }
    else if (strncmp(data, Key_tags, strlen(data))== 0) {
        context->expect = Expect_tags;
    }
    else if (strncmp(data, Key_timestamp, strlen(data))== 0) {
        context->expect = Expect_timestamp;
    }
    else if (strncmp(data, Key_trip, strlen(data))== 0) {
        context->expect = Expect_trip;
    }
    else if (strncmp(data, Key_trip_type, strlen(data))== 0) {
        context->expect = Expect_trip_type;
    }
    else if (strncmp(data, Key_unique_posters, strlen(data))== 0) {
        context->expect = Expect_unique_posters;
    }
    else {
        fprintf(stderr, "! Error @ fill_post_expected: unknown key %s\n", data);
        return 1;
    }
    return 0;
}

int fill_post_value(makaba_post_cpp &post, const int expect, const char *data,
	const bool &verbose)
{
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
		case Expect_op:
            post.op = atoi(data);
            return 0;
        case Expect_sticky:
            post.sticky = atoi(data);
            return 0;
        case Expect_timestamp:
            post.timestamp = atoi(data);
            return 0;
		case Expect_comment:
            post.comment = parseHTML(data, strlen(data), verbose);
			if (post.comment == NULL) {
				fprintf(stderr, "[fill_post_value] ! Error @ parseHTML()\n");
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
            post.name = parseHTML(data, strlen(data), verbose);
			if (post.name == NULL) {
				fprintf(stderr, "[fill_post_value] ! Error @ parseHTML()\n");
				return 1;
			}
            return 0;
        case Expect_num:
            post.num = atoi(data);
            return 0;
        case Expect_parent:
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
        case Expect_unique_posters:
            post.unique_posters = atoi(data);
            return 0;
    }
    fprintf(stderr, "! Error @ fill_post_value: unknown context.expect value: %d\n", expect);
    return 1;
}

int fill_captcha_id_expected(json_context *context, const char *data) {
	if (strncmp(data, Key_id, strlen(data)) == 0) {
        context->expect = Expect_id;
    }
    else if (strncmp(data, Key_result, strlen(data)) == 0) {
        context->expect = Expect_result;
    }
	else if (strncmp(data, Key_type, strlen(data)) == 0) {
        // Игнорируем, и так знаем, что 2chaptcha
    }
	else {
		fprintf(stderr, "! Error @ fill_captcha_expected: unknown key %s\n", data);
        return 1;
	}
	return 0;
}

int fill_captcha_id_value(makaba_2chaptcha *captcha, const int expect, const char *data) {
	switch (expect) {
		case Expect_id:
			captcha->id = (char *) calloc(strlen(data) + 1, sizeof(char));
			memcpy(captcha->id, data, strlen(data));
			return 0;
		case Expect_result:
			captcha->result = atoi(data);
			return 0;
	}
	fprintf(stderr, "! Error @ fill_captcha_id_value: unknown context.expect value: %d\n", expect);
	return 1;
}

// =======
// Captcha
// =======

int initCaptcha_cpp(makaba_2chaptcha &captcha, const char *board,
	const long long thread, const bool &verbose)
{
	if (CURL_BUFF_BODY == NULL)
		makabaSetup();
	char *captcha_str = get2chaptchaIdJSON(board, lint2str(thread));
	if (captcha_str == NULL) {
		fprintf(stderr, "[initCaptcha_cpp] ! Error @ get2chaptchaIdJSON(): %d\n", makaba_errno);
		return 1;
	}
	// Не заказываем еще раз, т.к. используется 1 раз

	json_context context;
    context.type = captcha_id;
    context.status = Status_default;
	context.verbose = verbose;
    context.memdest = &captcha;
	json_parser parser;
    if (json_parser_init(&parser, NULL, json_callback, &context)) {
        fprintf(stderr, "[initCaptcha_cpp] ! Error: json_parser_init() failed\n");
		makaba_errno = ERR_JSON_INIT;
		return 1;
    }
	int ret = json_parser_string(&parser, captcha_str, strlen(captcha_str), NULL);
	if (ret) {
		printf("Error @ parse: %d\n", ret);
        json_parser_free(&parser);
		makaba_errno = ERR_JSON_PARSE;
		return 1;
    }

	captcha.png_url = form2chaptchaPicURL(captcha.id);

	return 0;
}

int prepareCaptcha_cpp(makaba_2chaptcha &captcha, const char *board,
	const long long thread, const bool &verbose)
{
	if (initCaptcha_cpp(captcha, board, thread, verbose)) {
		fprintf(stderr, "[prepareCaptcha_cpp] ! Error @ initCaptcha_cpp: %d\n", makaba_errno);
		return 1;
	}

	long long pic_size;
	char *captcha_png = get2chaptchaPicPNG(captcha.png_url, &pic_size);
	if (captcha_png == NULL) {
		fprintf(stderr, "[prepareCaptcha_cpp] ! Error @ get2chaptchaPicPNG: %d\n", makaba_errno);
		return 1;
	}

	FILE *captcha_png_file = fopen(CaptchaPngFilename, "w");
	fwrite(captcha_png, sizeof(char), pic_size, captcha_png_file);
	fclose(captcha_png_file);
	convert_img(CaptchaPngFilename, CaptchaUtfFilename, verbose);

	return 0;
}

// ======
// Thread
// ======

thread_cpp::thread_cpp(): num(0), nposts(0) {}

int initThread_cpp(makaba_thread_cpp &thread, const char *thread_string, const long long &thread_lenght,
	const bool &verbose)
{
	json_context context;
    context.type = thread_new;
    context.status = Status_default;
	context.verbose = verbose;
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
	thread.num = thread.posts.front().num;

	return 0;
};

int prepareThread_cpp (makaba_thread_cpp &thread, const char *board,
	const long long threadnum, const bool &verbose)
{
	fprintf(stderr, "[prepareThread_cpp] Started\n");
	long long threadsize = 0;
	char *thread_ch = NULL;
	thread.num = threadnum;

	thread.board = (char *) calloc(strlen(board), sizeof(char));
	if (thread.board == NULL) {
		fprintf(stderr, "[prepareThread_cpp] ! Error: calloc() failed\n");
		makaba_errno = ERR_MEMORY;
		return 1;
	}
	memcpy(thread.board, board, strlen(board));

	bool fallback = false;
	if (Json_cache_dir == NULL) {
		if (initJsonCache()) {
			fprintf(stderr, "[prepareThread_cpp] ! Error @ initJsonCache()\n");
			fprintf(stderr, "[prepareThread_cpp]! Warning: Fallback to getThreadJSON()\n");
			fallback = true;
		}
	}

	fprintf(stderr, "[prepareThread_cpp] Checking for cache file\n");
	if (fallback == false && checkJsonCache(thread)) { // Тред есть в кэше
		fprintf(stderr, "[prepareThread_cpp] Trying to read from cache\n");
		thread_ch = readJsonCache(thread, &threadsize);
		if (thread_ch == NULL) {
			fprintf(stderr, "[prepareThread_cpp]! Warning: Error @ readJsonCache()\n");
			fprintf(stderr, "[prepareThread_cpp]! Warning: Fallback to getThreadJSON()\n");
			thread_ch = getThreadJSON(board, threadnum, 1, &threadsize, verbose);
			if (thread_ch == NULL) {
				fprintf(stderr, "[prepareThread_cpp]! Error @ getThreadJSON()\n");
				return 1;
			}
		} // Избавиться бы от копипасты
		fprintf(stderr, "[prepareThread_cpp] Read from cache\n");
		armJsonCache(thread);
	}
	else { // В кэше нет или что-то пошло не так
		fprintf(stderr, "[prepareThread_cpp] Not found in cache, loading\n");
		thread_ch = getThreadJSON(board, threadnum, 1, &threadsize, verbose);
		if (thread_ch == NULL) {
			fprintf(stderr, "[prepareThread_cpp]! Error @ getThreadJSON()\n");
			return 1;
		}
		fprintf(stderr, "[prepareThread_cpp] Writing to cache\n");
		if (writeJsonCache(thread, thread_ch)) {
			fprintf(stderr, "[prepareThread_cpp]! Error @ writeJsonCache()\n");
			return 1;
		}
		fprintf(stderr, "[prepareThread_cpp] Written to cache\n");
		armJsonCache(thread);
	}

	if (initThread_cpp(thread, thread_ch, threadsize, verbose)) {
		fprintf(stderr, "[prepareThread_cpp]! Error @ initThread_cpp()\n");
		free(thread.board);
		return 1;
	}
	fprintf(stderr, "[prepareThread_cpp] Done, exiting\n");

	return 0;
}

int updateThread_cpp(makaba_thread_cpp &thread, const bool &verbose)
{
	long long threadsize = 0;
	char *thread_ch = getThreadJSON(thread.board, thread.num,
		thread.nposts + 1, &threadsize, verbose);
	// Номер следующего поста - thread.nposts + 1

	if (thread_ch == NULL) {
		fprintf(stderr, "[updateThread_cpp] ! Error @ getThreadJSON()\n");
		return 1;
	}
	if (verbose) fprintf(stderr, ">> Got update:\n%s\n>> End of raw update\n",
		thread_ch);

	bool write_cache = true;
	if (Json_cache_dir == NULL) {
		if (initJsonCache()) {
			write_cache = false;
			fprintf(stderr, "[prepareThread_cpp] Error @ initJsonCache()\n");
			fprintf(stderr, "[prepareThread_cpp] Warning: Fallback to getThreadJSON()\n");
		}
	}
	if (write_cache)
		writeJsonCache(thread, thread_ch);

	if (initThread_cpp(thread, thread_ch, threadsize, verbose)) {
		fprintf(stderr, "[updateThread_cpp] ! Error @ initThread_cpp()\n");
		return 1;
	}

	return 0;
}

// ========================================
// JSON cache
// ========================================
int initJsonCache()
{
	char *homedir = getenv("HOME");
	if (homedir == NULL) {
		fprintf(stderr, "[initJsonCache]! Error: can`t getenv() HOME\n");
		makaba_errno = ERR_GETENV;
		return -1;
	}
	sprintf(Json_cache_dir, "%s/.cache/2ch-cli", homedir);
	fprintf(stderr, "[initJsonCache] Json_cache_dir = \"%s\"\n", Json_cache_dir);
	if (access(Json_cache_dir, F_OK)) {
		if (mkdir(Json_cache_dir, S_IRWXU) == -1) { // Директория -> 0700
			fprintf(stderr, "[initJsonCache]! Error: mkdir() failed: %s\n",
				strerror(errno));
			makaba_errno = ERR_UNKNOWN;
			return -1;
		}
	}
	return 0;
}

bool checkJsonCache(const makaba_thread_cpp &thread)
{
	char filename[70] = "";
	sprintf(filename, "%s/thread-%s-%d",
		Json_cache_dir, thread.board, thread.num);
	return access(filename, F_OK) == 0;
}

void armJsonCache(const makaba_thread_cpp &thread)
{
	char filename_old[70] = "";
	sprintf(filename_old, "%s/thread-%s-%d",
		Json_cache_dir, thread.board, thread.num);
	char filename_new[70] = "";
	sprintf(filename_new, "%s-%s", filename_old, Json_cache_suff_armed);
	rename(filename_old, filename_new);
}

void disarmJsonCache(const makaba_thread_cpp &thread)
{
	char filename_new[70] = "";
	sprintf(filename_new, "%s/thread-%s-%d",
		Json_cache_dir, thread.board, thread.num);
	char filename_old[70] = "";
	sprintf(filename_old, "%s-%s", filename_new, Json_cache_suff_armed);
	rename(filename_old, filename_new);
}

char *readJsonCache(const makaba_thread_cpp &thread, long long *threadsize)
{
	char filename[70] = "";
	sprintf(filename, "%s/thread-%s-%d",
		Json_cache_dir, thread.board, thread.num);
	if (strlen(Json_cache_dir) == 0) {
		if (initJsonCache() == -1) {
			fprintf(stderr, "[readJsonCache]! Error: @ initJsonCache()\n");
		}
	}
	if (! checkJsonCache(thread)) {
		fprintf(stderr, "[readJsonCache]! Error: cache file %s doesn`t exist\n",
				filename);
		makaba_errno = ERR_CACHE_NOENT;
		return NULL;
	}

	FILE *fd = fopen(filename, "r");
	fseek(fd, 0, SEEK_END);
	*threadsize = ftell(fd);
	if (Json_cache_buf == NULL) {
		Json_cache_buf = (char *) calloc(Json_cache_buf_size, sizeof(char));
	}

	Json_cache_buf[0] = '[';
	fseek(fd, 0, SEEK_SET);
	fread(Json_cache_buf + 1, sizeof(char), *threadsize, fd);
	Json_cache_buf[*threadsize + 1] = ']';
	if (ferror(fd)) {
		fprintf(stderr, "[readJsonCache]! Error while reading cache file: %d\n",
				ferror(fd));
		fclose(fd);
		makaba_errno = ERR_CACHE_READ;
		return NULL;
	}
	fclose(fd);

	return Json_cache_buf;
}

int writeJsonCache(const makaba_thread_cpp &thread, const char *thread_ch)
{
	if (strlen(Json_cache_dir) == 0) {
		if (initJsonCache() == -1) {
			fprintf(stderr, "[writeJsonCache]! Error: @ initJsonCache()\n");
		}
	}
	if (strlen(thread_ch) <= 2)
		return 0;

	char filename[70] = "";
	sprintf(filename, "%s/thread-%s-%d-%s",
		Json_cache_dir, thread.board, thread.num, Json_cache_suff_armed);

	FILE *fd = fopen(filename, "a+");
	long long fsize = 0;
	fseek(fd, 0, SEEK_END);
	fsize = ftell(fd);
	if (fsize > 0) {
		fputc(',', fd);
	}
	fwrite(thread_ch + 1, sizeof(char), strlen(thread_ch) - 2, fd);
	fclose(fd);
	return 0;
}

int cleanJsonCache() {
    if (strlen(Json_cache_dir) == 0) {
		if (initJsonCache() == -1) {
			fprintf(stderr, "[cleanJsonCache]! Error: @ initJsonCache()\n");
		}
	}

    if (chdir(Json_cache_dir)) {
        switch(errno) {
            case ENOENT:
                fprintf(stderr, "[cleanJsonCache]! Error: cache directory %s doesn`t exist after initJsonCache()\n");
                return 0;
            default:
                fprintf(stderr, "[cleanJsonCache]! Error: can`t chdir() to %s: %s\n",
                    Json_cache_dir, strerror(errno));
                return -1;
        }
    }

    DIR *dir;
    struct dirent *entry;
    dir = opendir("./");

    if (dir != NULL) {
        for (int i = 0; entry = readdir(dir); i++) {
            if (i > 1) {
				if (! strstr(entry->d_name, Json_cache_suff_armed)) {
					fprintf(stderr, "[cleanJsonCache] Delete %s/%s\n",
	                    Json_cache_dir, entry->d_name);
	                remove(entry->d_name);
				}
            }
        }
       closedir(dir);
    }
    else {
        fprintf(stderr, "[cleanJsonCache]! Error: Can`t open cache directory: %s\n",
            Json_cache_dir);
        return -1;
    }

    return 0;
}
// ========================================
// Freeing functions
// ========================================

void freePost(makaba_post_cpp &post) {
	free(post.comment);
	free(post.date);
	free(post.email);
	free(post.name);
	free(post.subject);
	free(post.tags);
	free(post.trip);
	free(post.trip_type);
}

void freeThread(makaba_thread_cpp &thread) {
	for (int i = 0; i < thread.nposts; i++) {
		freePost(thread.posts[i]);
	}
	free(thread.board);
}

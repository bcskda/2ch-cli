// ========================================
// File: parser.cpp
// API answer parsing functions
// (Implementation)
// ========================================

#include "parser.h"
#include <iostream>

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
	if (context->type == captcha_id) {
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
	char *captcha_str = get2chaptchaId(board, thread, verbose);
	if (captcha_str == NULL) {
		fprintf(stderr, "[initCaptcha_cpp] ! Error @ get2chaptchaId(): %d\n", makaba_errno);
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

// ========================================
// Треды, посты
// ========================================

thread::thread(): // private
	isNull_(true)
	{} 

thread::thread(const std::string nboard, const long long nnum):
	isNull_(true),
	num   (nnum),
	board (nboard),
	nposts(0)
{
	char *raw;
	bool fallback = false;
	if (Json_cache_dir == NULL) {
		if (initJsonCache()) {
			fprintf(stderr, "[thread::thread(const std::string, const long long)] "
							"Error @ initJsonCache()\n");
			fprintf(stderr, "[thread::thread(const std::string, const long long)] "
							"Warning: Fallback to getThread()\n");
			fallback = true;
		}
	}
	long long size;
	if (fallback == false && checkJsonCache(*this)) { // Тред есть в кэше
		raw = readJsonCache(*this, &size);
		if (raw == NULL) {
			fprintf(stderr, "[thread::thread(const std::string, const long long)] "
							"Warning: Fallback to getThread()\n");
			raw = getThread(this->board.data(), this->num,
							1, &size, false);
			if (raw == NULL) {
				fprintf(stderr, "[thread::thread(const std::string, const long long)] "
								"Error @ getThread()\n");
				return;
			}
		} // Избавиться бы от копипасты
		armJsonCache(*this);
	}
	else { // В кэше нет или что-то пошло не так
		raw = getThread(this->board.data(), this->num,
						1, &size, false);
		if (raw == NULL) {
			fprintf(stderr, "[thread::thread(const std::string, const long long)] "
							"Error @ getThread()\n");
			return;
		}
		if (writeJsonCache(*this, raw)) {
			fprintf(stderr, "[thread::thread(const std::string, const long long)] "
							"Error @ writeJsonCache()\n");
			return;
		}
		armJsonCache(*this);
	}

	if (! this->append(raw)) {
		fprintf(stderr, "[thread::thread(const std::string, const long long)] "
						"Error @ thread::append(const char *)\n");
		makaba_errno = ERR_GENERAL_FORMAT;
	}
	this->isNull_ = false;
}

bool thread::isNull()
{
	return this->isNull_;
}

bool thread::append(const char *raw)
{
	Json::CharReaderBuilder rbuilder;
	std::unique_ptr<Json::CharReader> const reader(rbuilder.newCharReader());
	std::string errs;
	Json::Value array;
	if (! reader->parse(raw, raw + strlen(raw), &array, &errs)) {
		fprintf(stderr, "[thread::append(const char *)] Error:\n"
						"Json::CharReader::parse():\n  %s\n",
				errs.data());
		makaba_errno = ERR_GENERAL_FORMAT;
		return false;
	}

	for (auto obj : array) {
		this->nposts++;
		makaba_post post(obj);
		post.rel_num = this->nposts;
		this->posts.push_back(post);
	}
	return true;
}

bool thread::update()
{
	if (this->isNull()) {
		fprintf(stderr, "[bool thread::update()] Error: is null\n");
		makaba_errno = ERR_INTERNAL;
		return false;
	}

	long long size;
	char *raw = getThread(this->board.data(), this->num,
						  this->nposts + 1, &size, false);
	if (raw == NULL) {
		fprintf(stderr, "[bool thread::update()] Error @ getThread()\n");
		return false;
	}

	bool write_cache = true;
	if (Json_cache_dir == NULL) {
		if (initJsonCache()) {
			write_cache = false;
			fprintf(stderr, "[bool thread::update()] Error @ initJsonCache()"
							"[bool thread::update()] Warning: Can`t write cache\n");
		}
	}
	if (write_cache)
		writeJsonCache(*this, raw);

	if (! this->append(raw)) {
		fprintf(stderr, "[bool thread::update()] Error @ thread::append()\n");
		return false;
	}

	return true;
}

post::post():
	isNull_(true)
	{}

post::post(Json::Value &val):
	isNull_        (false),
	banned        ( atoi(       val["banned"        ].asString().data()) ),
	closed        ( atoi(       val["closed"        ].asString().data()) ),
	comment       ( ""                                                   ),
	date          ( std::string(val["date"          ].asString()       ) ),
	email         ( std::string(val["email"         ].asString()       ) ),
	files         (             val["files"         ]                    ),
	lasthit       ( atoi(       val["lasthit"       ].asString().data()) ),
	name          ( ""                                                   ),
	num           ( atoi(       val["num"           ].asString().data()) ),
	op            ( atoi(       val["op"            ].asString().data()) ),
	parent        ( atoi(       val["parent"        ].asString().data()) ),
	sticky        ( atoi(       val["sticky"        ].asString().data()) ),
	subject       ( std::string(val["subject"       ].asString()       ) ),
	tags          ( std::string(val["tags"          ].asString()       ) ),
	timestamp     ( atoi(       val["timestamp"     ].asString().data()) ),
	trip          ( std::string(val["trip"          ].asString()       ) ),
	trip_type     ( std::string(val["trip_type"     ].asString()       ) ),
	unique_posters( atoi(       val["unique_posters"].asString().data()) ),
	rel_num       ( 0                                                    )
{
	const char *comment_raw = val["comment"].asCString();
	char *comment_parsed = parseHTML(comment_raw, strlen(comment_raw), true);
	this->comment = std::string(comment_parsed);
	free(comment_parsed);
	const char *name_raw = val["name"].asCString();
	char *name_parsed = parseHTML(name_raw, strlen(name_raw), true);
	this->name = std::string(name_parsed);
	free(name_parsed);
	fprintf(stderr, "<init post #%8lld>\n", this->num);
}

bool post::isNull()
{
	return this->isNull_;
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

bool checkJsonCache(const makaba_thread &thread)
{
	char filename[70] = "";
	sprintf(filename, "%s/thread-%s-%d",
		Json_cache_dir, thread.board, thread.num);
	return access(filename, F_OK) == 0;
}

void armJsonCache(const makaba_thread &thread)
{
	char filename_old[70] = "";
	sprintf(filename_old, "%s/thread-%s-%d",
		Json_cache_dir, thread.board, thread.num);
	char filename_new[70] = "";
	sprintf(filename_new, "%s-%s", filename_old, Json_cache_suff_armed);
	rename(filename_old, filename_new);
}

void disarmJsonCache(const makaba_thread &thread)
{
	char filename_new[70] = "";
	sprintf(filename_new, "%s/thread-%s-%d",
		Json_cache_dir, thread.board, thread.num);
	char filename_old[70] = "";
	sprintf(filename_old, "%s-%s", filename_new, Json_cache_suff_armed);
	rename(filename_old, filename_new);
}

char *readJsonCache(const makaba_thread &thread, long long *threadsize)
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

int writeJsonCache(const makaba_thread &thread, const char *thread_ch)
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
// jsoncpp
// ========================================


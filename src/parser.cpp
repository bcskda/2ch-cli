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
// Captcha
// ========================================

bool captcha_2chaptcha::isNull()
{
	return this->isNull_;
}

captcha_2chaptcha::captcha_2chaptcha():
	isNull_(true),
	id     (std::string())
	{}

captcha_2chaptcha::captcha_2chaptcha(const std::string &board, const long long &threadnum):
	isNull_(true),
	id     (std::string())
{
	if (! this->get_id(board, threadnum)) {
		fprintf(stderr, "[captcha_2chaptcha::captcha_2chaptcha(const std::string &, const long long &)]: "
						"Error: captcha_2chaptcha::get_id() failed\n");
		return;
	}
	if (! this->form_url()) {
		fprintf(stderr, "[captcha_2chaptcha::captcha_2chaptcha(const std::string &, const long long &)]: "
						"Error: captcha_2chaptcha::form_url() failed\n");
		return;
	}
	isNull_ = false;
}

bool captcha_2chaptcha::get_id(const std::string &board, const long long &threadnum)
{
	if (this->id.length()) {
		fprintf(stderr, "[bool captcha_2chaptcha::get_id()] Note: already has ID\n"
						"  board = %s, thread = %lld\n", board.data(), threadnum);
		return true;
	}
	char *id_raw = get2chaptchaId(board.data(), threadnum, false);
	if (id_raw == NULL) {
		fprintf(stderr, "[bool captcha_2chaptcha::get_id()] Error: "
						"get2chaptchaId(): %d\n", makaba_errno);
		return false;
	}
	Json::CharReaderBuilder rbuilder;
	std::unique_ptr<Json::CharReader> const reader(rbuilder.newCharReader());
	std::string errs;
	Json::Value ans;
	if (! reader->parse(id_raw, id_raw + strlen(id_raw), &ans, &errs)) {
		fprintf(stderr, "[bool captcha_2chaptcha::get_id()] Error:\n"
						"Json::CharReader::parse():\n  %s\n",
				errs.data());
		makaba_errno = ERR_GENERAL_FORMAT;
		return false;
	}
	fprintf(stderr, "btw ans:\n");
    std::cerr << ans << std::endl;
	if (! ans["error"].isNull()) {
		fprintf(stderr, "[bool captcha_2chaptcha::get_id()] Error: "
						"API returned \"error\":\"%d\"\n");
		this->error = ans["description"].asString();
		return false;
	}
	this->id = ans["id"].asString();
	return true;
}

bool captcha_2chaptcha::form_url()
{
	if (this->id.length() == 0) {
		fprintf(stderr, "[bool captcha_2chaptcha::form_url] Error: "
						"ID is null\n");
		return false;
	}
	if (this->png_url.length()) {
		fprintf(stderr, "[bool captcha_2chaptcha::form_url] Note: "
						"already has png_url\n");
		return true;
	}
	this->png_url.resize(strlen(BASE_URL) + strlen(CAPTCHA_2CHAPTCHA) +
						this->id.length() + 10);
	this->png_url = BASE_URL;
	this->png_url += '/';
	this->png_url += CAPTCHA_2CHAPTCHA;
	this->png_url += "/image/";
	this->png_url += this->id;
	return true;
}

bool captcha_2chaptcha::get_png() {
	if (this->id.length() == 0) {
		fprintf(stderr, "[bool captcha_2chaptcha::get_png] Error: "
						"ID is null\n");
		return false;
	}
	if (this->png_url.length() == 0) {
		fprintf(stderr, "[bool captcha_2chaptcha::get_png] Error: "
						"png_url is null\n");
		return false;
	}
	
	long long pic_size;
	char *pic = get2chaptchaPicPNG(this->png_url.data(), &pic_size);
	if (pic == NULL) {
		fprintf(stderr, "[bool captcha_2chaptcha::get_png] Error: "
						"get2chaptchaPicPNG() failed: %d\n", makaba_errno);
		return false;
	}
	FILE *pic_file = fopen(CaptchaPngFilename, "w");
	fwrite(pic, sizeof(char), pic_size, pic_file);
	fclose(pic_file);
	
	convert_img(CaptchaPngFilename, CaptchaUtfFilename, false);

	return true;
}

// ========================================
// Треды, посты
// ========================================

thread::thread(): // private
	isNull_(true)
	{} 

thread::thread(const std::string &board, const long long &num):
	isNull_(true),
	num    (num),
	board  (board),
	nposts (0)
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
		Json_cache_dir, thread.board.data(), thread.num);
	return access(filename, F_OK) == 0;
}

void armJsonCache(const makaba_thread &thread)
{
	char filename_old[70] = "";
	sprintf(filename_old, "%s/thread-%s-%d",
		Json_cache_dir, thread.board.data(), thread.num);
	char filename_new[70] = "";
	sprintf(filename_new, "%s-%s", filename_old, Json_cache_suff_armed);
	rename(filename_old, filename_new);
}

void disarmJsonCache(const makaba_thread &thread)
{
	char filename_new[70] = "";
	sprintf(filename_new, "%s/thread-%s-%d",
		Json_cache_dir, thread.board.data(), thread.num);
	char filename_old[70] = "";
	sprintf(filename_old, "%s-%s", filename_new, Json_cache_suff_armed);
	rename(filename_old, filename_new);
}

char *readJsonCache(const makaba_thread &thread, long long *threadsize)
{
	char filename[70] = "";
	sprintf(filename, "%s/thread-%s-%d",
		Json_cache_dir, thread.board.data(), thread.num);
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
		Json_cache_dir, thread.board.data(), thread.num, Json_cache_suff_armed);

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

// ========================================
// File: makaba.cpp
// High-level imageboard API
// ========================================


#include "makaba.h"


const size_t Json_cache_buf_size = 2e6;
const char *Json_cache_suff_armed = "active";
char *Json_cache_buf = NULL;
char Json_cache_dir[100] = "";


const char *CaptchaPngFilename = "/tmp/2ch-captcha.png";
const char *CaptchaUtfFilename = "/tmp/2ch-captcha.utf8";


// ========================================
// Посты
// ========================================


Makaba::Post::Post():
	isNull_(true)
	{}


Makaba::Post::Post(const std::string &vcomment, const std::string &vemail,
				   const std::string &vname,    const std::string &vsubject,
				   const std::string &vtags,    const std::string &vtrip):
	isNull_(false),
	comment(std::string(vcomment)), email  (std::string(vemail)),
	name   (std::string(vname)),    subject(std::string(vsubject)),
	tags   (std::string(vtags)),    trip   (std::string(vtrip))
	{}


Makaba::Post::Post(const char *vcomment, const char *vemail,
				   const char *vname,    const char *vsubject,
				   const char *vtags,    const char *vtrip):
	isNull_(false                      ),
	comment(vcomment), email  (vemail  ),
	name   (vname   ), subject(vsubject),
	tags   (vtags   ), trip   (vtrip   )
	{}


Makaba::Post::Post(Json::Value &val):
	isNull_       ( false                                          ),
	banned        ( atoi( val["banned"        ].asString().data()) ),
	closed        ( atoi( val["closed"        ].asString().data()) ),
	comment       ( ""                                             ),
	date          (       val["date"          ].asString()         ),
	email         (       val["email"         ].asString()         ),
	files         (       val["files"         ]                    ),
	lasthit       ( atoi( val["lasthit"       ].asString().data()) ),
	name          ( ""                                             ),
	num           ( atoi( val["num"           ].asString().data()) ),
	op            ( atoi( val["op"            ].asString().data()) ),
	parent        ( atoi( val["parent"        ].asString().data()) ),
	sticky        ( atoi( val["sticky"        ].asString().data()) ),
	subject       (       val["subject"       ].asString()         ),
	tags          (       val["tags"          ].asString()         ),
	timestamp     ( atoi( val["timestamp"     ].asString().data()) ),
	trip          (       val["trip"          ].asString()         ),
	trip_type     (       val["trip_type"     ].asString()         ),
	unique_posters( atoi( val["unique_posters"].asString().data()) ),
	rel_num       ( 0                                              )
{
	const char *comment_raw = val["comment"].asCString();
	char *comment_parsed = parseHTML(comment_raw, strlen(comment_raw), false);
	this->comment = comment_parsed;
	free(comment_parsed);
	const char *name_raw = val["name"].asCString();
	char *name_parsed = parseHTML(name_raw, strlen(name_raw), false);
	this->name = name_parsed;
	free(name_parsed);
	fprintf(stderr, "<init post #%10lld>\n", this->num);
}


bool Makaba::Post::isNull()
{
	return this->isNull_;
}


// ========================================
// Треды
// ========================================


Makaba::Thread::Thread():
	isNull_(true)
	{} 


Makaba::Thread::Thread(const std::string &board, const long long &num):
	isNull_(true ),
	num    (num  ),
	nposts (0    ),
	board  (board)
{
	char *raw;
	bool fallback = false;
	if (Json_cache_dir == NULL) {
		if (initJsonCache()) {
			fprintf(stderr, "[%s] Error @ initJsonCache()\n",
					__PRETTY_FUNCTION__);
			fprintf(stderr, "[%s] Warning: Fallback to getThread()\n",
					__PRETTY_FUNCTION__);
			fallback = true;
		}
	}
	long long size;
	if (fallback == false && checkJsonCache(*this)) { // Тред есть в кэше
		raw = readJsonCache(*this, &size);
		if (raw == NULL) {
			fprintf(stderr, "[%s] Warning: Fallback to getThread()\n",
					__PRETTY_FUNCTION__);
			raw = getThread(this->board.data(), this->num,
							1, &size, false);
			if (raw == NULL) {
				fprintf(stderr, "[%s] Error @ getThread()\n",
						__PRETTY_FUNCTION__);
				return;
			}
		} // Избавиться бы от копипасты
		armJsonCache(*this);
	}
	else { // В кэше нет или что-то пошло не так
		raw = getThread(this->board.data(), this->num,
						1, &size, false);
		if (raw == NULL) {
			fprintf(stderr, "[%s] Error @ getThread()\n", __PRETTY_FUNCTION__);
			return;
		}
		if (writeJsonCache(*this, raw)) {
			fprintf(stderr, "[%s] Error @ writeJsonCache()\n", __PRETTY_FUNCTION__);
			return;
		}
		armJsonCache(*this);
	}
	if (this->append(raw)) {
		fprintf(stderr, "[%s] Error @ this->append()\n", __PRETTY_FUNCTION__);
		makaba_errno = ERR_GENERAL_FORMAT;
	}
	this->isNull_ = false;
}


bool Makaba::Thread::isNull()
{
	return this->isNull_;
}


int Makaba::Thread::append(const char *raw)
{
	Json::CharReaderBuilder rbuilder;
	std::unique_ptr<Json::CharReader> const reader(rbuilder.newCharReader());
	std::string errs;
	Json::Value array;
	if (! reader->parse(raw, raw + strlen(raw), &array, &errs)) {
		fprintf(stderr, "[%s] Error:\n"
						"Json::CharReader::parse():\n  %s\n",
				__PRETTY_FUNCTION__, errs.data());
		makaba_errno = ERR_GENERAL_FORMAT;
		return -1;
	}
	for (auto obj : array) {
		this->nposts++;
		Makaba::Post post(obj);
		post.rel_num = this->nposts;
		this->posts.push_back(post);
	}
	return 0;
}


int Makaba::Thread::update()
{
	if (this->isNull()) {
		fprintf(stderr, "[%s] Error: is null\n", __PRETTY_FUNCTION__);
		makaba_errno = ERR_INTERNAL;
		return -1;
	}
	long long size;
	char *raw = getThread(this->board.data(), this->num,
						  this->nposts + 1, &size, false);
	if (raw == NULL) {
		fprintf(stderr, "[%s] Error @ getThread()\n", __PRETTY_FUNCTION__);
		return -1;
	}
	bool write_cache = true;
	if (Json_cache_dir == NULL) {
		if (initJsonCache()) {
			write_cache = false;
			fprintf(stderr, "[%s] Error @ initJsonCache(), can`t write cache\n",
				__PRETTY_FUNCTION__);
		}
	}
	if (write_cache)
		writeJsonCache(*this, raw);
	if (this->append(raw)) {
		fprintf(stderr, "[%s] Error @ this->append()\n", __PRETTY_FUNCTION__);
		return -1;
	}
	return 0;
}


// ========================================
// Капча
// ========================================


bool Makaba::Captcha_2ch::isNull()
{
	return this->isNull_;
}


Makaba::Captcha_2ch::Captcha_2ch():
	isNull_(true),
	id     (std::string())
	{}


Makaba::Captcha_2ch::Captcha_2ch(const std::string &board, const long long &threadnum):
	isNull_(true),
	id     (std::string())
{
	if (this->get_id(board, threadnum)) {
		fprintf(stderr, "[%s]: Error: this->get_id() failed\n",
				__PRETTY_FUNCTION__);
		return;
	}
	if (this->form_url()) {
		fprintf(stderr, "[%s]: Error: this->form_url() failed\n",
				__PRETTY_FUNCTION__);
		return;
	}
	isNull_ = false;
}


int Makaba::Captcha_2ch::get_id(const std::string &board, const long long &threadnum)
{
	if (this->id.length()) {
		fprintf(stderr, "[%s] Note: already has ID\n"
						"  board = %s, thread = %lld\n",
				__PRETTY_FUNCTION__, board.data(), threadnum);
		return -1;
	}
	char *id_raw = get2chaptchaId(board.data(), threadnum, false);
	if (id_raw == NULL) {
		fprintf(stderr, "[%s] Error: get2chaptchaId(): %d\n",
				__PRETTY_FUNCTION__, makaba_errno);
		return -1;
	}
	Json::CharReaderBuilder rbuilder;
	std::unique_ptr<Json::CharReader> const reader(rbuilder.newCharReader());
	std::string errs;
	Json::Value ans;
	if (! reader->parse(id_raw, id_raw + strlen(id_raw), &ans, &errs)) {
		fprintf(stderr, "[%s] Error:\n"
						"Json::CharReader::parse():\n  %s\n",
				__PRETTY_FUNCTION__, errs.data());
		makaba_errno = ERR_GENERAL_FORMAT;
		return -1;
	}
	fprintf(stderr, "btw ans:\n");
    std::cerr << ans << std::endl;
	if (! ans["error"].isNull()) {
		fprintf(stderr, "[%s] Error: \n"
						"API returned \"error\":\"%d\"\n",
						__PRETTY_FUNCTION__, ans["error"].asInt());
		this->error = ans["description"].asString();
		return -1;
	}
	this->id = ans["id"].asString();
	return 0;
}


int Makaba::Captcha_2ch::form_url()
{
	if (this->id.length() == 0) {
		fprintf(stderr, "[%s] Error: ID is null\n", __PRETTY_FUNCTION__);
		return -1;
	}
	if (this->png_url.length()) {
		fprintf(stderr, "[%s] Note: already has png_url, not re-doing\n",
				__PRETTY_FUNCTION__);
		return -1;
	}
	this->png_url.resize(strlen(BASE_URL) + strlen(CAPTCHA_2CHAPTCHA) +
						this->id.length() + 10);
	this->png_url = BASE_URL;
	this->png_url += '/';
	this->png_url += CAPTCHA_2CHAPTCHA;
	this->png_url += "/image/";
	this->png_url += this->id;
	return 0;
}


int Makaba::Captcha_2ch::get_png() {
	if (this->id.length() == 0) {
		fprintf(stderr, "[%s] Error: ID is null\n", __PRETTY_FUNCTION__);
		return -1;
	}
	if (this->png_url.length() == 0) {
		fprintf(stderr, "[%s] Error: png_url is null\n", __PRETTY_FUNCTION__);
		return -1;
	}
	long long pic_size;
	char *pic = get2chaptchaPicPNG(this->png_url.data(), &pic_size);
	if (pic == NULL) {
		fprintf(stderr, "[%s] Error: get2chaptchaPicPNG() failed: %d\n",
				__PRETTY_FUNCTION__, makaba_errno);
		return -1;
	}
	FILE *pic_file = fopen(CaptchaPngFilename, "w");
	fwrite(pic, sizeof(char), pic_size, pic_file);
	fclose(pic_file);
	
	convert_img(CaptchaPngFilename, CaptchaUtfFilename, false);
	return 0;
}


// ========================================
// Кэш
// ========================================


int initJsonCache()
{
	if (Env_HOME.length() == 0) {
		std::cerr << "call setup" << std::endl;
		setup_env();
	}
	std::cerr << "[at json] btw HOME = \"" << Env_HOME.data() << std::endl;
	sprintf(Json_cache_dir, "%s/.cache/2ch-cli", Env_HOME.data());
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

bool checkJsonCache(const Makaba::Thread &thread)
{
	char filename[70] = "";
	sprintf(filename, "%s/thread-%s-%lld",
		Json_cache_dir, thread.board.data(), thread.num);
	return access(filename, F_OK) == 0;
}

void armJsonCache(const Makaba::Thread &thread)
{
	char filename_old[70] = "";
	sprintf(filename_old, "%s/thread-%s-%lld",
		Json_cache_dir, thread.board.data(), thread.num);
	char filename_new[70] = "";
	sprintf(filename_new, "%s-%s", filename_old, Json_cache_suff_armed);
	rename(filename_old, filename_new);
}

void disarmJsonCache(const Makaba::Thread &thread)
{
	char filename_new[70] = "";
	sprintf(filename_new, "%s/thread-%s-%lld",
		Json_cache_dir, thread.board.data(), thread.num);
	char filename_old[70] = "";
	sprintf(filename_old, "%s-%s", filename_new, Json_cache_suff_armed);
	rename(filename_old, filename_new);
}

char *readJsonCache(const Makaba::Thread &thread, long long *threadsize)
{
	char filename[70] = "";
	sprintf(filename, "%s/thread-%s-%lld",
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

int writeJsonCache(const Makaba::Thread &thread, const char *thread_ch)
{
	if (strlen(Json_cache_dir) == 0) {
		if (initJsonCache() == -1) {
			fprintf(stderr, "[writeJsonCache]! Error: @ initJsonCache()\n");
		}
	}
	if (strlen(thread_ch) <= 2)
		return 0;

	char filename[70] = "";
	sprintf(filename, "%s/thread-%s-%lld-%s",
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
                fprintf(stderr, "[cleanJsonCache]! Error: cache directory %s doesn`t exist after initJsonCache()\n",
						Json_cache_dir);
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

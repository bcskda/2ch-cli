// ========================================
// File: parser.cpp
// Parsing functions
// (Implementation)
// ========================================

#include "parser.h"

// Global defs

const size_t Json_cache_buf_size = 2e6;
const char *Json_cache_suff_armed = "active";
char *Json_cache_buf = NULL;
char Json_cache_dir[100] = "";

const char *PATTERN_TAG_OPEN = "<";
const char *PATTERN_TAG_CLOSE = ">";

const char *PATTERN_HREF_OPEN = "<a href=\"";
const char *PATTERN_HREF_CLOSE = "</a>";
const char *PATTERN_REPLY_CLASS = "class=\"post-reply-link\"";
const char *PATTERN_REPLY_THREAD = "data-thread=\"";
const char *PATTERN_REPLY_NUM = "data-num=\"";
const char *PATTERN_NEWLINE = "<br>";
const char *PATTERN_LT = "&lt";
const char *PATTERN_GT = "&gt";
const char *PATTERN_SLASH = "&#47";
const char *PATTERN_BCKSLASH = "&#92";
const char *PATTERN_NBSP = "&nbsp";
const char *PATTERN_SINGLE_QUOT = "&#39";
const char *PATTERN_DOUBLE_QUOT = "&quot";
const char *PATTERN_AMP = "&amp";

const char *CaptchaPngFilename = "/tmp/2ch-captcha.png";
const char *CaptchaUtfFilename = "/tmp/2ch-captcha.utf8";

// End Global defs

char *parseHTML (const char *raw, const long long raw_len, const bool v) { // Пока что игнорируем разметку

	if (v) {
		fprintf(stderr, "]] Started parseHTML\n");
		fprintf(stderr, "Raw len: %lld\n", raw_len);
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
	if (v) fprintf(stderr, "]] Final length: %lld\n", parsed_len);
	if (v) fprintf(stderr, "]] Exiting parseHTML\n");
	return parsed;
}




// ========================================
// JSON cache
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

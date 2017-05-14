// ========================================
// File: makaba.cpp
// Makaba API-related functions
// (Implementation)
// ========================================

#include "makaba.h"

// ========================================
// API general
// ========================================

char *callAPI(const std::string &url,
			  const Postfields &postfields,
			  long long *size)
{
	fprintf(stderr, "[callAPI] Start\n");
	fprintf(stderr, "[callAPI] url   = \"%s\"\n", url.data());
	fprintf(stderr, "[callAPI] postfields:\n");
	for (auto p: postfields) {
		fprintf(stderr, "  \"%10s\" = \"%s\"\n", p.first.data(), p.second.data());
	}
	if (url.length() == 0) {
		fprintf(stderr, "[callAPI] Error: zero-length url\n");
		makaba_errno = ERR_ARGS;
		return NULL;
	}
	if (CURL_BUFF_BODY == NULL) {
		fprintf(stderr, "[callAPI] Warning: curl body buffer not allocated\n");
		makabaSetup();
	}
	CURL *curl_handle = curl_easy_init();
	if (! curl_handle) {
		fprintf(stderr, "[callAPI] Error: curl_easy_init() failed\n");
		makaba_errno = ERR_CURL_INIT;
		return NULL;
    }

	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, CURL_UA);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, CURL_BUFF_BODY);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, CURL_writeToBuff);
	curl_easy_setopt(curl_handle, CURLOPT_URL, url.data());

	if (postfields.size() > 0) {
		struct curl_httppost* post = NULL;
		struct curl_httppost* last = NULL;
		
		for (auto p: postfields) {
			curl_formadd(&post, &last,
						CURLFORM_COPYNAME,  p.first.data(),
						CURLFORM_COPYCONTENTS, p.second.data(),
						CURLFORM_END);
		}
		
		curl_easy_setopt(curl_handle, CURLOPT_HTTPPOST, post);
	}

	CURLcode request_status = curl_easy_perform(curl_handle);
	fprintf(stderr, "[callAPI] request performed\n");
	CURL_BUFF_BODY[CURL_BUFF_POS] = 0;
	if (size != NULL)
		*size = (long long) CURL_BUFF_POS;
	CURL_BUFF_POS = 0;

	char *ret = CURL_BUFF_BODY;
	if (request_status != CURLE_OK) {
		fprintf(stderr, "[callAPI] Error: curl_easy_perform() failed: %s\n",
				curl_easy_strerror(request_status));
		makaba_errno = ERR_CURL_PERFORM;
		ret = NULL;
	}
	curl_easy_cleanup(curl_handle);

    fprintf(stderr, "[callAPI] Exit\n");

	return ret;
}

// ========================================
// Get from API
// ========================================

char *getBoardsList(const bool v) {
	if (v) fprintf(stderr, "[getBoardsList] Start");

	// URL: 2ch.hk/makaba/mobile.fcgi?task=get_boards
	std::string url = BASE_URL;
	url += '/';
	url += MOBILE_API;
	url += "?task=get_boards";
	if (v) fprintf(stderr, "[getBoardsList] url = %s\n", url.data());
	
	char *result = callAPI(url, {}, NULL);
	if (result == NULL) {
		fprintf(stderr, "[getBoardsListJSON] Error: callAPI() failed\n");
	}
	if (v) fprintf(stderr, "[getBoardsList] Exit");
	return result;
}

char *getBoardPage(const char *board, const long long page, const bool v) {
	if (v) fprintf(stderr, "[getBoardPage] Start");
	if (board == NULL) {
		fprintf(stderr, "[getBoardPage] Error: board = null\n");
		makaba_errno = ERR_ARGS;
		return NULL;
	}
	if (page <= 0) {
		fprintf(stderr, "[getBoardPage] Error: page \"%lld\" <= 0\n", page);
		makaba_errno = ERR_ARGS;
		return NULL;
	}

	// URL format: 2ch.hk/$board/$page.json
	std::string url = BASE_URL;
	url += '/';
	url += board;
	url += '/';
	url += std::to_string(page);
	url += ".json";
	if (v) fprintf(stderr, "[getBoardPage] url = %s\n", url.data());
	
	char *result = callAPI(url, {}, NULL);
	if (result == NULL) {
		fprintf(stderr, "[getBoardPage] Error: callAPI() failed\n");
	}
	if (v) fprintf(stderr, "[getBoardPage] Exit");
	return result;
}

char *getBoardCatalog(const char *board, const bool v) {
	if (v) fprintf(stderr, "[getBoardCatalog] Start");
	if (board == NULL) {
		fprintf(stderr, "[getBoardCatalog] Error: board = null\n");
		makaba_errno = ERR_ARGS;
		return NULL;
	}

	// URL format: 2ch.hk/$board/catalog.json
	std::string url = BASE_URL;
	url += board;
	url += "/catalog.json";
	if (v) fprintf(stderr, "[getBoardCatalog] url = %s\n", url.data());

	char *result = callAPI(url, {}, NULL);
	if (result == NULL) {
		fprintf(stderr, "[getBoardCatalog] Error: callAPI() failed\n");
	}
	if (v) fprintf(stderr, "[getBoardCatalog] Exit");
	return result;
}

char *getThread(const char *board, const long long threadn,
	const long long postn_rel, long long *threadsize, const bool v) {
	if (v) fprintf(stderr, "[getThread] Start\n");
	if (board == NULL) {
		fprintf(stderr, "[getThread] Error: board = null\n");
		makaba_errno = ERR_ARGS;
		return NULL;
	}
	if (threadn <= 0) {
		fprintf(stderr, "[getThread] Error: thread \"%lld\" <= 0\n", threadn);
		makaba_errno = ERR_ARGS;
		return NULL;
	}
	
	// URL: 2ch.hk/makaba/mobile.fcgi
	std::string url = BASE_URL;
	url += '/';
	url += MOBILE_API;
	
	// POST: task=get_thread&board=$board&thread=$threadn&post=$postn_rel
	Postfields post(
		{
			{ "task",   "get_thread"              },
			{ "board",  board                     },
			{ "thread", std::to_string(threadn)   },
			{ "post",   std::to_string(postn_rel) }
		}
													   );
	
	char *result = callAPI(url, post, threadsize);
	if (result == NULL) {
		fprintf(stderr, "[getThread] Error: callAPI() failed\n");
	}
	if (v) fprintf(stderr, "[getThread] Exit\n");
	return result;
}

// ========================================
// Captcha
// ========================================

char *getCaptchaSettings(const char *board, const bool v) {
	if (v) fprintf(stderr, "[getCaptchaSettings] Start");
	if (board == NULL) {
		fprintf(stderr, "[getCaptchaSettings] Error: board = null\n");
		makaba_errno = ERR_ARGS;
		return NULL;
	}
	
	// URL: 2ch.hk/api/captcha/settings/$board
	std::string url = BASE_URL;
	url += '/';
	url += CAPTCHA_SETTINGS;
	url += '/';
	url += board;
	if (v) fprintf(stderr, "[getCaptchaSettings] url = %s\n", url.data());
	
	char *result = callAPI(url, {}, NULL);
	if (result == NULL) {
		fprintf(stderr, "[getCaptchaSettings] Error: callAPI() failed\n");
	}
	if (v) fprintf(stderr, "[getCaptchaSettings] Exit");
	return result;
}

char *get2chaptchaId(const char *board, const long long threadn, const bool v) {
	if (v) fprintf(stderr, "[getCaptchaId] Start\n");
	if (board == NULL) {
		fprintf(stderr, "[getCaptchaId] Error: board = null\n");
		makaba_errno = ERR_ARGS;
		return NULL;
	}
	if (threadn <= 0) {
		fprintf(stderr, "[getCaptchaId] Error: thread \"%lld\" <= 0\n", threadn);
		makaba_errno = ERR_ARGS;
		return NULL;
	}
	
	// URL: 2ch.hk/api/captcha/2chaptcha/id
	std::string url = BASE_URL;
	url += '/';
	url += CAPTCHA_2CHAPTCHA;
	url += "/id";
	if (v) fprintf(stderr, "[get2chaptchaId] url = %s\n", url.data());
	
	// POST: board=$board&thread=$threadn
	Postfields post(
		{
			{ "board",  board                   },
			{ "thread", std::to_string(threadn) }
		}
			   );
	
	char *result = callAPI(url, post, NULL);
	if (result == NULL) {
		fprintf(stderr, "[get2chaptchaId] Error: callAPI() failed\n");
	}
	if (v) fprintf(stderr, "[get2chaptchaId] Exit\n");
	return result;
}

char *get2chaptchaPicPNG(const char *url, long long *pic_size) {
	if (url == NULL) {
		fprintf(stderr, "[get2chaptchaPicPNG] Error: url = null\n");
		makaba_errno = ERR_ARGS;
		return NULL;
	}
	
	if (CURL_BUFF_BODY == NULL) {
		fprintf(stderr, "[get2chaptchaPicPNG] Warning: curl body buffer not allocated\n");
		makabaSetup();
	}

	char *result = callAPI(url, {}, pic_size);
	if (result == NULL) {
		fprintf(stderr, "[get2chaptchaPicPNG] Error: callAPI() failed\n");
	}
	return result;
}

// ========================================
// Posting
// ========================================

char *sendPost (const char *board, const long long threadn,
				const char *comment, const char *subject,
				const char *name, const char *email,
				const char *captcha_id, const char *captcha_value)
{
	if (board == NULL) {
		fprintf(stderr, "[sendPost] Error: board = null\n");
		makaba_errno = ERR_ARGS;
		return NULL;
	}
	if (threadn < 0) {
		fprintf(stderr, "[sendPost] Error: thread \"%lld\" < 0\n", threadn);
		makaba_errno = ERR_ARGS;
		return NULL;
	}
	if (comment == NULL) {
		fprintf(stderr, "[sendPost] Error: comment = null\n");
		makaba_errno = ERR_ARGS;
		return NULL;
	}
	if (strlen(comment) > COMMENT_LEN_MAX) {
		fprintf(stderr, "[sendPost] Error: comment length \"%d\" > maximal \"%lld\"\n",
				(int)strlen(comment), COMMENT_LEN_MAX);
		makaba_errno = ERR_ARGS;
		return NULL;
	}
	if (captcha_id == NULL) { // @TODO Капча обязательна не везде
		fprintf(stderr, "[sendPost] Error: captcha_id = null\n");
		makaba_errno = ERR_ARGS;
		return NULL;
	}
	if (captcha_value == NULL) {
		fprintf(stderr, "[sendPost] Error: captcha_value = null\n");
		makaba_errno = ERR_ARGS;
		return NULL;
	}
	
	// URL: 2ch.hk/makaba/posting.fcgi
	std::string url = BASE_URL;
	url += '/';
	url += POSTING_API;
	fprintf(stderr, "[sendPost] url = %s\n", url.data());
	
	Postfields post(
		{
			{ "json",            "1"                     },
			{ "task",            "post"                  },
			{ "captcha_type",    "2chaptcha"             },
			{ "board",           board                   },
			{ "thread",          std::to_string(threadn) },
			{ "comment",         comment                 },
			{ "2chaptcha_id",    captcha_id              },
			{ "2chaptcha_value", captcha_value           }
		}
			   );
	if (subject != NULL) {
		post.push_back( { "subject", subject } );
	}
	if (name != NULL) {
		post.push_back( { "name", name } );
	}
	if (email != NULL) {
		post.push_back( { "email", email } );
	}
	
	char *result = callAPI(url, post, NULL);
	if (result == NULL) {
		fprintf(stderr, "[sendPost] Error: callAPI() failed\n");
	}
	fprintf(stderr, "[sendPost] Exit\n");
	return result;
}

// ========================================
// Misc utility functions
// ========================================

size_t CURL_writeToBuff (const char *src, const size_t block_size, const size_t nmemb, void *dest) {
	if (src == NULL ||
		CURL_BUFF_POS + block_size * nmemb > CURL_BUFF_BODY_SIZE) {
		return 0;
	}
	else {
		memcpy (dest + CURL_BUFF_POS, src, block_size * nmemb);
		CURL_BUFF_POS += block_size * nmemb;
		return block_size * nmemb;
	}
}

void makabaSetup() {
	if (CURL_BUFF_BODY == NULL)
		CURL_BUFF_BODY = (char*) calloc (CURL_BUFF_BODY_SIZE, sizeof(char));
}

void makabaCleanup() {
	if (CURL_BUFF_BODY != NULL)
		free(CURL_BUFF_BODY);
}

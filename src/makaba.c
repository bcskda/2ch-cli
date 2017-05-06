// ========================================
// File: makaba.c
// Makaba API-related functions
// (Implementation)
// ========================================

#include "makaba.h"

// ========================================
// API general
// ========================================

char *callAPI(const char *url, const char *post, long long *size, const bool v) {
	fprintf(stderr, "[callAPI] Start\n");
	fprintf(stderr, "[callAPI] url   = \"%s\"\n", url);
	fprintf(stderr, "[callAPI] post  = \"%s\"\n", post);
	if (url == NULL) {
		fprintf(stderr, "[callAPI] Error: URL = null\n");
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
	curl_easy_setopt(curl_handle, CURLOPT_URL, url);
	if (post != NULL) {
		curl_easy_setopt(curl_handle, CURLOPT_POST, 1);
		curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, strlen(post));
		curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, post);
	}

	CURLcode request_status = curl_easy_perform(curl_handle);
	if (v) fprintf(stderr, "[callAPI] request performed\n");
	CURL_BUFF_BODY[CURL_BUFF_POS] = 0;
	if (size != NULL)
		*size = (long long) CURL_BUFF_POS;
	CURL_BUFF_POS = 0;
	if (request_status != CURLE_OK) {
		fprintf(stderr, "[callAPI] Error: curl_easy_perform() failed: %s\n",
				curl_easy_strerror(request_status));
		curl_easy_cleanup(curl_handle);
		makaba_errno = ERR_CURL_PERFORM;
		return NULL;
	}
	curl_easy_cleanup(curl_handle);

    fprintf(stderr, "[callAPI] Exit\n");

	return CURL_BUFF_BODY;
}

// ========================================
// Get from API
// ========================================

char *getBoardsList(const bool v) {
	if (v) fprintf(stderr, "[getBoardsList] Start");

	int url_length = strlen(BASE_URL) + strlen(MOBILE_API) + 20;
	// URL: 2ch.hk/makaba/mobile.fcgi?task=get_boards
	char *url = (char *) calloc(url_length, sizeof(char));
	if (url == NULL) {
		fprintf(stderr, "[getBoardsList] Error: failed calloc() for url\n");
		makaba_errno = ERR_MEMORY;
		return NULL;
	}
	sprintf(url, "%s/%s?task=get_boards", 
			BASE_URL, MOBILE_API);
	if (v) fprintf(stderr, "[getBoardsList] url = %s\n", url);
	if (CURL_BUFF_BODY == NULL) {
		fprintf(stderr, "[getBoardsList] Warning: curl body buffer not allocated\n");
		makabaSetup();
	}
	
	char *result = callAPI(url, NULL, NULL, v);
	if (result == NULL) {
		fprintf(stderr, "[getBoardsListJSON] Error: callAPI() failed\n");
	}
	free(url);
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
		fprintf(stderr, "[getBoardPage] Error: page \"%ld\" <= 0\n", page);
		makaba_errno = ERR_ARGS;
		return NULL;
	}

	int url_length = strlen(BASE_URL) + strlen(board) + log10(page) + 10;
	// URL format: 2ch.hk/$board/$page.json
	char *url = (char *) calloc(url_length, sizeof(char));
	if (url == NULL) {
		fprintf(stderr, "[getBoardPage] Error: failed calloc() for url\n");
		makaba_errno = ERR_MEMORY;
		return NULL;
	}
	sprintf(url, "%s/%s/%ld.json",
			BASE_URL, board, page);
	if (v) fprintf(stderr, "[getBoardPage] url = %s\n", url);
	if (CURL_BUFF_BODY == NULL) {
		fprintf(stderr, "[getBoardPage] Warning: curl body buffer not allocated\n");
		makabaSetup();
	}

	char *result = callAPI(url, NULL, NULL, v);
	if (result == NULL) {
		fprintf(stderr, "[getBoardPage] Error: callAPI() failed\n");
	}
	free(url);
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

	int url_length = strlen(BASE_URL) + strlen(board) + 20;
	// URL format: 2ch.hk/$board/catalog.json
	char *url = (char *) calloc(url_length, sizeof(char));
	if (url == NULL) {
		fprintf(stderr, "[getBoardCatalog] Error: failed calloc() for url\n");
		makaba_errno = ERR_MEMORY;
		return NULL;
	}
	sprintf(url, "%s/%s/catalog.json",
			BASE_URL, board);
	if (v) fprintf(stderr, "[getBoardCatalog] url = %s\n", url);
	if (CURL_BUFF_BODY == NULL) {
		fprintf(stderr, "[getBoardCatalog] Warning: curl body buffer not allocated\n");
		makabaSetup();
	}

	char *result = callAPI(url, NULL, NULL, v);
	if (result == NULL) {
		fprintf(stderr, "[getBoardCatalog] Error: callAPI() failed\n");
	}
	free(url);
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
		fprintf(stderr, "[getThread] Error: thread \"%ld\" <= 0\n", threadn);
		makaba_errno = ERR_ARGS;
		return NULL;
	}

	int url_length = strlen(BASE_URL) + strlen(MOBILE_API) + 5;
	// URL: 2ch.hk/makaba/mobile.fcgi
	char *url = (char *) calloc(url_length, sizeof(char));
	if (url == NULL) {
		fprintf(stderr, "[getThread] Error: failed calloc() for url\n");
		makaba_errno = ERR_MEMORY;
		return NULL;
	}
	sprintf(url, "%s/%s",
			BASE_URL, MOBILE_API);
	if (v) fprintf(stderr, "[getThread] url = %s\n", url);
	
	int post_length = strlen(board) + log10(threadn) + log10(postn_rel) + 40;
	// POST: task=get_thread&board=$board&thread=$threadn&post=$postn_rel
	char *post = (char *) calloc(post_length, sizeof(char));
	if (post == NULL) {
		fprintf(stderr, "[getThread] Error: failed calloc() for post\n");
		makaba_errno = ERR_MEMORY;
		free(url);
		return NULL;
	}
	sprintf(post, "task=get_thread&board=%s&thread=%ld&post=%ld", 
			board, threadn, postn_rel);
	if (v) fprintf(stderr, "[getThread] post = %s\n", post);

	if (CURL_BUFF_BODY == NULL) {
		fprintf(stderr, "[getThread] Warning: curl body buffer not allocated\n");
		makabaSetup();
	}
	char *result = callAPI(url, post, threadsize, v);
	if (result == NULL) {
		fprintf(stderr, "[getThread] Error: callAPI() failed\n");
	}
	free(post);
	free(url);
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
	
	int url_length = strlen(BASE_URL) + strlen(CAPTCHA_SETTINGS) + strlen(board) + 5;
	// URL: 2ch.hk/api/captcha/settings/$board
	char *url = (char *) calloc(url_length, sizeof(char));
	if (url == NULL) {
		fprintf(stderr, "[getCaptchaSettings] Error: failed calloc() for url\n");
		makaba_errno = ERR_MEMORY;
		return NULL;
	}
	sprintf(url, "%s/%s/%s", 
				BASE_URL, CAPTCHA_SETTINGS, board);
	if (v) fprintf(stderr, "[getCaptchaSettings] url = %s\n", url);
	if (CURL_BUFF_BODY == NULL) {
		fprintf(stderr, "[getCaptchaSettings] Warning: curl body buffer not allocated\n");
		makabaSetup();
	}
	
	char *result = callAPI(url, NULL, NULL, v);
	if (result == NULL) {
		fprintf(stderr, "[getCaptchaSettings] Error: callAPI() failed\n");
	}
	free(url);
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
		fprintf(stderr, "[getCaptchaId] Error: thread \"%ld\" <= 0\n", threadn);
		makaba_errno = ERR_ARGS;
		return NULL;
	}

	int url_length = strlen(BASE_URL) + strlen(MOBILE_API) + 5;
	// URL: 2ch.hk/api/captcha/2chaptcha/id
	char *url = (char *) calloc(url_length, sizeof(char));
	if (url == NULL) {
		fprintf (stderr, "[get2chaptchaId] Error: failed calloc() for url\n");
		makaba_errno = ERR_MEMORY;
		return NULL;
	}
	sprintf(url, "%s/%s/id",
			BASE_URL, CAPTCHA_2CHAPTCHA);
	if (v) fprintf(stderr, "[get2chaptchaId] url = %s\n", url);
	
	int post_length = strlen(board) + log10(threadn) + 15;
	// POST: board=$board&thread=$threadn
	char *post = (char *) calloc(post_length, sizeof(char));
	if (post == NULL) {
		fprintf (stderr, "[get2chaptchaId] Error: failed calloc() for post\n");
		makaba_errno = ERR_MEMORY;
		free(url);
		return NULL;
	}
	sprintf(post, "board=%s&thread=%ld", 
			board, threadn);
	if (v) fprintf(stderr, "[get2chaptchaId] post = %s\n", post);

	if (CURL_BUFF_BODY == NULL) {
		fprintf(stderr, "[get2chaptchaId] Warning: curl body buffer not allocated\n");
		makabaSetup();
	}
	char *result = callAPI(url, post, NULL, v);
	if (result == NULL) {
		fprintf(stderr, "[get2chaptchaId] Error: callAPI() failed\n");
	}
	free(post);
	free(url);
	if (v) fprintf(stderr, "[get2chaptchaId] Exit\n");
	return result;
}

char *form2chaptchaPicURL(const char *id) {
	int url_length = strlen(BASE_URL) + strlen(CAPTCHA_2CHAPTCHA) + strlen(id) + 10;
	char *url = (char*) calloc(url_length, sizeof(char));
	if (url == NULL) {
			fprintf (stderr, "[form2chaptchaPicURL] Error: failed calloc() for url\n");
			makaba_errno = ERR_MEMORY;
			return NULL;
	}
	sprintf(url, "%s/%s/image/%s",
			BASE_URL, CAPTCHA_2CHAPTCHA, id);
	return url;
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

	char *result = callAPI(url, NULL, pic_size, false);
	if (result == NULL) {
		fprintf(stderr, "[get2chaptchaPicPNG] Error: callAPI() failed\n");
	}
	return result;
}

// ========================================
// Posting
// ========================================

char *sendPost (const char *board, const long long threadn,
	const char *comment, const char *subject, const char *name, const char *email,
	const char *captcha_id, const char *captcha_value, long long *answer_length)
{
	if (board == NULL) {
		fprintf(stderr, "[sendPost] Error: board = null\n");
		makaba_errno = ERR_ARGS;
		return NULL;
	}
	if (threadn < 0) {
		fprintf(stderr, "[sendPost] Error: thread \"%ld\" < 0\n", threadn);
		makaba_errno = ERR_ARGS;
		return NULL;
	}
	if (comment == NULL) {
		fprintf(stderr, "[sendPost] Error: comment = null\n");
		makaba_errno = ERR_ARGS;
		return NULL;
	}
	if (strlen(comment) > COMMENT_LEN_MAX) {
		fprintf(stderr, "[sendPost] Error: comment length \"%d\" > maximal \"%d\"\n",
				strlen(comment), COMMENT_LEN_MAX);
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

	int url_length = strlen(BASE_URL) + strlen(POSTING_API) + 5;
	// URL: 2ch.hk/makaba/posting.fcgi
	char *url = (char *) calloc(url_length, sizeof(char));
	if (url == NULL) {
		fprintf(stderr, "[sendPost] Error: failed calloc() for url\n");
		makaba_errno = ERR_MEMORY;
		return NULL;
	}
	sprintf(url, "%s/%s",
			BASE_URL, POSTING_API);
	fprintf(stderr, "[sendPost] url = %s\n", url);
	
	int post_length = strlen(POSTING_FIELDS) + strlen(board) + 25;
	post_length += (threadn > 0) ? (log10(threadn)) : (1);
	post_length += 9 + strlen(comment);
	if (subject != NULL) {
		post_length += 9 + strlen(subject);
	}
	if (name != NULL) {
		post_length += 6 + strlen(name);
	}
	if (email != NULL) {
		post_length += 7 + strlen(email);
	}
	post_length += 12 + strlen(captcha_id);
	post_length += 15 + strlen(captcha_value);
	char *post = (char *) calloc(post_length, sizeof(char));
	if (post == NULL) {
		fprintf(stderr, "[sendPost] Error: failed calloc() for post\n");
		makaba_errno = ERR_MEMORY;
		free(url);
		return NULL;
	}
	sprintf(post, "%s&board=%s&thread=%ld&comment=%s",
				POSTING_FIELDS, board, threadn, comment);
	if (subject != NULL) {
		sprintf(post, "%s&subject=%s",
				post, subject);
	}
	if (name != NULL) {
		sprintf(post, "%s&name=%s",
				post, name);
	}
	if (email != NULL) {
		sprintf(post, "%s&email=%s",
				post, email);
	}
	sprintf(post, "%s&2chaptcha_id=%s&2chaptcha_value=%s",
			post, captcha_id, captcha_value);
	fprintf(stderr, "[sendPost] post = %s\n", post);

	if (CURL_BUFF_BODY == NULL) {
		fprintf(stderr, "[sendPost] Warning: curl body buffer not allocated\n");
		makabaSetup();
	}
	char *result = callAPI(url, post, NULL, false);
	if (result == NULL) {
		fprintf(stderr, "[sendPost] Error: callAPI() failed\n");
	}
	free(post);
	free(url);
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

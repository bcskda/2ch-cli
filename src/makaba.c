// ========================================
// File: makaba.c
// Makaba API-related functions
// (Implementation)
// ========================================

#pragma once
#include "makaba.h"

// ========================================
// General info getting
// ========================================

char *getBoardsListJSON (const bool v) {
	fprintf (stderr, "]] Starting getBoardsList");
	if (v) fprintf (stderr, " (verbose)"); fprintf (stderr, "\n");
	if (v) fprintf (stderr, "] initializing curl handle\n");
	CURL *curl_handle = curl_easy_init();
	CURLcode request_status;
	if (curl_handle) {
		if (v) fprintf (stderr, "] curl handle initialized\n");
		short URL_length = strlen(BASE_URL)+strlen(MOBILE_API)+16+1;
		if (v) fprintf (stderr, "URL length = %d\n", URL_length);
		// URL: 2ch.hk/makaba/mobile.fcgi?task=get_boards
		char *URL = (char*) calloc (URL_length, sizeof(char));
		if (URL != NULL) {
			if (v) fprintf (stderr, "memory allocated (URL)\n");
		}
		else {
			fprintf (stderr, "[getBoardsList]! Error allocating memory (URL)\n");
			curl_easy_cleanup (curl_handle);
			makaba_errno = ERR_MEMORY;
			return NULL;
		}

		if (v) fprintf (stderr, "] Forming URL\n");
		URL = strcpy (URL, BASE_URL);
		if (v) fprintf (stderr, "URL state 0: %s\n", URL);
		URL = strcat (URL, MOBILE_API);
		if (v) fprintf (stderr, "URL state 1: %s\n", URL);
		URL = strcat (URL, "?task=get_boards");
		if (v) fprintf (stderr, "URL state 2: %s\n", URL);
		if (v) fprintf (stderr, "] URL formed\n");
		curl_easy_setopt (curl_handle, CURLOPT_URL, URL);
		if (v) fprintf (stderr, "] option URL set\n");

		if (CURL_BUFF_BODY == NULL) {
			fprintf(stderr, "[getBoardsList]! Error: curl body buffer not allocated\n");
			makaba_errno = ERR_MAKABA_SETUP;
			return NULL;
		}
		curl_easy_setopt (curl_handle, CURLOPT_WRITEDATA, CURL_BUFF_BODY);
		if (v) fprintf (stderr, "] option WRITEDATA set\n");

		curl_easy_setopt (curl_handle, CURLOPT_WRITEFUNCTION, CURL_writeToBuff);
		if (v) fprintf (stderr, "] option WRITEFUNCTION set\n");

		request_status = curl_easy_perform (curl_handle);
		if (v) fprintf (stderr, "] curl request performed\n");
		CURL_BUFF_BODY[CURL_BUFF_POS] = 0;
		CURL_BUFF_POS = 0;
		if (v) fprintf (stderr, "] buffer pos set to 0\n");
		if (request_status == CURLE_OK) {
			if (v) fprintf (stderr, "request status: OK\n");
			printf ("%s\n", CURL_BUFF_BODY);
		}
		else {
			fprintf (stderr, "[getBoardsList]! Error @ curl_easy_perform: %s\n",
				curl_easy_strerror(request_status));
			curl_easy_cleanup (curl_handle);
			free (URL);
			makaba_errno = ERR_CURL_PERFORM;
			return NULL;
		}

		curl_easy_cleanup (curl_handle);
		if (v) fprintf (stderr, "] curl cleanup done\n");
		free (URL);
		fprintf (stderr, "]] Exiting getBoardsList\n");
	}
	else {
		fprintf (stderr, "! Error initializing curl handle\n");
		makaba_errno = ERR_CURL_INIT;
		return NULL;
	}

	return CURL_BUFF_BODY;
}

char *getBoardPageJSON (const char *board, const long int page, const bool v) {
	fprintf (stderr, "]] Starting getBoardPage");
	if (v) fprintf (stderr, " (verbose)"); fprintf (stderr, "\n");
	if (v) fprintf (stderr, "] initializing curl handle\n");
	CURL *curl_handle = curl_easy_init();
	CURLcode request_status;
	if (curl_handle) {
		if (v) fprintf (stderr, "] curl handle initialized\n");
		char *page_string = lint2str (page);
		if (v) fprintf (stderr, "page number (string) = %s\n", page_string);
		short URL_length = strlen(BASE_URL)+strlen(board)+1+strlen(page_string)+5;
		if (v) fprintf (stderr, "URL length = %d\n", URL_length);
		// URL format: 2ch.hk/$board/$page.json
		char *URL = (char*) calloc (URL_length, sizeof(char));
		if (URL != NULL) {
			if (v) fprintf (stderr, "memory allocated (URL)\n");
		}
		else {
			fprintf (stderr, "[getBoardPage]! Error allocating memory (URL)\n");
			curl_easy_cleanup (curl_handle);
			makaba_errno = ERR_MEMORY;
			return NULL;
		}

		if (v) fprintf (stderr, "] Forming URL\n");
		URL = strcpy (URL, BASE_URL);
		if (v) fprintf (stderr, "URL state 0: %s\n", URL);
		URL = strcat (URL, board);
		if (v) fprintf (stderr, "URL state 1: %s\n", URL);
		URL = strcat (URL, "/");
		if (v) fprintf (stderr, "URL state 2: %s\n", URL);
		URL = strcat (URL, page_string);
		if (v) fprintf (stderr, "URL state 3: %s\n", URL);
		URL = strcat (URL, ".json");
		if (v) fprintf (stderr, "URL state 4: %s\n", URL);
		if (v) fprintf (stderr, "] URL formed\n");
		curl_easy_setopt (curl_handle, CURLOPT_URL, URL);
		if (v) fprintf (stderr, "] option URL set\n");

		if (CURL_BUFF_BODY == NULL) {
			fprintf(stderr, "[getBoardPage]! Error: curl body buffer not allocated\n");
			makaba_errno = ERR_MAKABA_SETUP;
			return NULL;
		}
		curl_easy_setopt (curl_handle, CURLOPT_WRITEDATA, CURL_BUFF_BODY);
		if (v) fprintf (stderr, "] option WRITEDATA set\n");

		curl_easy_setopt (curl_handle, CURLOPT_WRITEFUNCTION, CURL_writeToBuff);
		if (v) fprintf (stderr, "] option WRITEFUNCTION set\n");

		request_status = curl_easy_perform (curl_handle);
		if (v) fprintf (stderr, "] curl request performed\n");
		CURL_BUFF_BODY[CURL_BUFF_POS] = 0;
		CURL_BUFF_POS = 0;
		if (v) fprintf (stderr, "] buffer pos set to 0\n");
		if (request_status == CURLE_OK) {
			if (v) fprintf (stderr, "request status: OK\n");
			printf ("%s\n", CURL_BUFF_BODY);
		}
		else {
			fprintf (stderr, "[getBoardPage]! Error @ curl_easy_perform: %s\n",
				curl_easy_strerror(request_status));
			curl_easy_cleanup (curl_handle);
			free (URL);
			makaba_errno = ERR_CURL_PERFORM;
			return NULL;
		}

		curl_easy_cleanup (curl_handle);
		if (v) fprintf (stderr, "] curl cleanup done\n");
		free (URL);
		fprintf (stderr, "]] Exiting getBoardPage\n");
	}
	else {
		fprintf (stderr, "! Error initializing curl handle\n");
		makaba_errno = ERR_CURL_INIT;
		return NULL;
	}

	return CURL_BUFF_BODY;
}

char *getBoardCatalogJSON (const char *board, const bool v) {
	fprintf (stderr, "]] Starting getBoardCatalog");
	if (v) fprintf (stderr, " (verbose)"); fprintf (stderr, "\n");
	if (v) fprintf (stderr, "] initializing curl handle\n");
	CURL *curl_handle = curl_easy_init();
	CURLcode request_status;
	if (curl_handle) {
		if (v) fprintf (stderr, "] curl handle initialized\n");
		short URL_length = strlen(BASE_URL)+strlen(board)+1+7+5;
		// URL format: 2ch.hk/$board/catalog.json
		if (v) fprintf (stderr, "URL length = %d\n", URL_length);
		char *URL = (char*) calloc (URL_length, sizeof(char));
		if (URL != NULL) {
			if (v) fprintf (stderr, "memory allocated (URL)\n");
		}
		else {
			fprintf (stderr, "[getBoardCatalog]! Error allocating memory (URL)\n");
			curl_easy_cleanup (curl_handle);
			makaba_errno = ERR_MEMORY;
			return NULL;
		}

		if (v) fprintf (stderr, "] Forming URL\n");
		URL = strcpy (URL, BASE_URL);
		if (v) fprintf (stderr, "URL state 0: %s\n", URL);
		URL = strcat (URL, board);
		if (v) fprintf (stderr, "URL state 1: %s\n", URL);
		URL = strcat (URL, "/catalog.json");
		if (v) fprintf (stderr, "URL state 2: %s\n", URL);
		if (v) fprintf (stderr, "] URL formed\n");
		curl_easy_setopt (curl_handle, CURLOPT_URL, URL);
		if (v) fprintf (stderr, "] option URL set\n");

		if (CURL_BUFF_BODY == NULL) {
			fprintf(stderr, "[getBoardCatalog]! Error: curl body buffer not allocated\n");
			makaba_errno = ERR_MAKABA_SETUP;
			return NULL;
		}
		curl_easy_setopt (curl_handle, CURLOPT_WRITEDATA, CURL_BUFF_BODY);
		if (v) fprintf (stderr, "] option WRITEDATA set\n");

		curl_easy_setopt (curl_handle, CURLOPT_WRITEFUNCTION, CURL_writeToBuff);
		if (v) fprintf (stderr, "] option WRITEFUNCTION set\n");

		request_status = curl_easy_perform (curl_handle);
		if (v) fprintf (stderr, "] curl request performed\n");
		if (request_status == CURLE_OK) {
			if (v) fprintf (stderr, "request status: OK\n");
			printf ("%s\n", CURL_BUFF_BODY);
		}
		else {
			fprintf (stderr, "[getBoardCatalog]! Error @ curl_easy_perform: %s\n",
				curl_easy_strerror(request_status));
				curl_easy_cleanup (curl_handle);
			free (URL);
			makaba_errno = ERR_CURL_PERFORM;
			return NULL;
		}

		curl_easy_cleanup (curl_handle);
		if (v) fprintf (stderr, "] curl cleanup done\n");
		free (URL);
		fprintf (stderr, "] memory free done\n");
		fprintf (stderr, "]] Exiting getBoardCatalog\n");
	}
	else {
		fprintf (stderr, "[getBoardCatalog]! Error initializing curl handle\n");
		makaba_errno = ERR_CURL_INIT;
		return NULL;
	}

	return CURL_BUFF_BODY;
}

char *getThreadJSON (const char *board, const long int threadnum, long int *threadsize, const bool v) {
	fprintf (stderr, "]] Starting getThread");
	if (v) fprintf (stderr, " (verbose)"); fprintf (stderr, "\n");
	if (v) fprintf (stderr, "] initializing curl handle\n");
	CURL *curl_handle = curl_easy_init();
	CURLcode request_status;
	if (curl_handle) {
		if (v) fprintf (stderr, "] curl handle initialized\n");
		char *threadnum_string = lint2str (threadnum);
		if (v) fprintf (stderr, "thread number (string) = %s\n", threadnum_string);
		const short URL_length = strlen(BASE_URL)+strlen(MOBILE_API);
		if (v) fprintf (stderr, "URL length = %d\n", URL_length);
		// API URL: 2ch.hk/makaba/mobile.fcgi
		char *URL = (char*) calloc (URL_length, sizeof(char));
		if (URL != NULL) {
			if (v) fprintf (stderr, "memory allocated (URL)\n");
		}
		else {
			fprintf (stderr, "[getThread]! Error allocating memory (URL)\n");
			curl_easy_cleanup (curl_handle);
			makaba_errno = ERR_MEMORY;
			return NULL;
		}

		curl_easy_setopt (curl_handle, CURLOPT_POST, 1);
		if (v) fprintf (stderr, "] option POST set\n");

		if (v) fprintf (stderr, "] Forming URL\n");
		URL = strcpy (URL, BASE_URL);
		if (v) fprintf (stderr, "URL state 0: %s\n", URL);
		URL = strcat (URL, MOBILE_API);
		if (v) fprintf (stderr, "URL state 1: %s\n", URL);
		if (v) fprintf (stderr, "] URL formed\n");
		curl_easy_setopt (curl_handle, CURLOPT_URL, URL);
		if (v) fprintf (stderr, "] option URL set\n");

		const short postfields_length = 15+1+6+strlen(board)+1+7+strlen(threadnum_string)+1+6;
		// POST data format: task=get_thread&board=$board&thread=$threadnum&post=0

		curl_easy_setopt (curl_handle, CURLOPT_POSTFIELDSIZE, postfields_length);
		if (v) fprintf (stderr, "] Option POSTFIELDSIZE set\n");

		char *postfields = (char*) calloc (postfields_length, sizeof(char));
		if (postfields != NULL) {
			if (v) fprintf (stderr, "memory allocated (POST data)\n");
		}
		else {
			fprintf (stderr, "[getThread]! Error allocating memory (POST data)\n");
			curl_easy_cleanup (curl_handle);
			free (URL);
			makaba_errno = ERR_MEMORY;
			return NULL;
		}
		if (v) fprintf (stderr, "] Forming POST data\n");
		postfields = strcpy (postfields, "task=get_thread");
		if (v) fprintf (stderr, "POST data state 0: %s\n", postfields);
		postfields = strcat (postfields, "&board=");
		if (v) fprintf (stderr, "POST data state 1: %s\n", postfields);
		postfields = strcat (postfields, board);
		if (v) fprintf (stderr, "POST data state 2: %s\n", postfields);
		postfields = strcat (postfields, "&thread=");
		if (v) fprintf (stderr, "POST data state 3: %s\n", postfields);
		postfields = strcat (postfields, threadnum_string);
		if (v) fprintf (stderr, "POST data state 4: %s\n", postfields);
		postfields = strcat (postfields, "&post=0");
		if (v) fprintf (stderr, "POST data state 5: %s\n", postfields);
		if (v) fprintf (stderr, "] POST data formed\n");
		curl_easy_setopt (curl_handle, CURLOPT_POSTFIELDS, postfields);
		if (v) fprintf (stderr, "] Option POSTFIELDS set\n");

		if (CURL_BUFF_BODY == NULL) {
			fprintf(stderr, "[getThread]! Error: curl body buffer not allocated\n");
			makaba_errno = ERR_MAKABA_SETUP;
			return NULL;
		}
		curl_easy_setopt (curl_handle, CURLOPT_WRITEDATA, CURL_BUFF_BODY);
		if (v) fprintf (stderr, "] option WRITEDATA set\n");

		curl_easy_setopt (curl_handle, CURLOPT_WRITEFUNCTION, CURL_writeToBuff);
		if (v) fprintf (stderr, "] option WRITEFUNCTION set\n");

		request_status = curl_easy_perform (curl_handle);
		if (v) fprintf (stderr, "] curl request performed\n");
		CURL_BUFF_BODY[CURL_BUFF_POS] = 0;

		*threadsize = (long int)CURL_BUFF_POS; // Кладём по данному указателю размер
		CURL_BUFF_POS = 0;           // буфера с тредом
		if (v) fprintf (stderr, "] buffer pos set to 0\n");
		if (request_status == CURLE_OK) {
			if (v) fprintf (stderr, "request status: OK\n");
		}
		else {
			fprintf (stderr, "[getThread]! Error @ curl_easy_perform: %s\n",
				curl_easy_strerror(request_status));
			curl_easy_cleanup (curl_handle);
			free (URL);
			free (postfields);
			makaba_errno = ERR_CURL_PERFORM;
			return NULL;
		}

		curl_easy_cleanup (curl_handle);
		if (v) fprintf (stderr, "] curl cleanup done\n");
		free (URL);
		free (postfields);
		if (v) fprintf (stderr, "] memory free done\n");
		fprintf (stderr, "]] Exiting getThread\n");
	}
	else {
		fprintf (stderr, "[getThread]! Error initializing curl handle\n");
		makaba_errno = ERR_CURL_INIT;
		return NULL;
	}

	return CURL_BUFF_BODY;
}

// ========================================
// Captcha
// ========================================

char *getCaptchaSettingsJSON (const char *board) {
	fprintf(stderr, "]] Starting getCaptchaSettings\n");

	CURL *curl_handle = curl_easy_init();
	CURLcode request_status;
	if (curl_handle) {
		const short URL_length = strlen(BASE_URL)+strlen(CAPTCHA_SETTINGS)+strlen(board)+1;
		char *URL = (char *) calloc (URL_length, sizeof(char));
		if (URL == NULL) {
			fprintf (stderr, "[getCaptchaSettings]! Error allocating memory (URL)\n");
			curl_easy_cleanup (curl_handle);
			makaba_errno = ERR_MEMORY;
			return NULL;
		}
		URL = strcpy (URL, BASE_URL);
		URL = strcat (URL, CAPTCHA_SETTINGS);
		URL = strcat (URL, board);
		curl_easy_setopt (curl_handle, CURLOPT_URL, URL);

		if (CURL_BUFF_BODY == NULL) {
			fprintf(stderr, "[getCaptchaSettings]! Error: curl body buffer not allocated\n");
			makaba_errno = ERR_MAKABA_SETUP;
			return NULL;
		}
		curl_easy_setopt (curl_handle, CURLOPT_WRITEDATA, CURL_BUFF_BODY);

		curl_easy_setopt (curl_handle, CURLOPT_WRITEFUNCTION, CURL_writeToBuff);

		request_status = curl_easy_perform (curl_handle);
		CURL_BUFF_BODY[CURL_BUFF_POS] = 0;
		CURL_BUFF_POS = 0;
		if (request_status != CURLE_OK) {
			fprintf (stderr, "[getCaptchaSettings]! Error @ curl_easy_perform: %s\n",
				curl_easy_strerror(request_status));
			curl_easy_cleanup (curl_handle);
			free (URL);
			makaba_errno = ERR_CURL_PERFORM;
			return NULL;
		}

		curl_easy_cleanup (curl_handle);
		free (URL);
	}
	else {
		fprintf (stderr, "[getCaptchaSettings]! Error initializing curl handle\n");
		makaba_errno = ERR_CURL_INIT;
		return NULL;
	}

	fprintf(stderr, "]] Exiting getCaptchaSettings\n");

	return CURL_BUFF_BODY;
}


char *get2chaptchaIdJSON (const char *board, const char *thread) {
	fprintf (stderr, "]] Starting get2chaptchaIdJSON\n");

	CURL *curl_handle = curl_easy_init();
	CURLcode request_status;
	if (curl_handle) {
		const short URL_length = strlen(BASE_URL)+strlen(CAPTCHA_2CHAPTCHA)+2+1;
		char *URL = (char*) calloc (URL_length, sizeof(char));
		if (URL == NULL) {
			fprintf (stderr, "[get2chaptchaIdJSON]! Error allocating memory (URL)\n");
			curl_easy_cleanup (curl_handle);
			makaba_errno = ERR_MEMORY;
			return NULL;
		}
		URL = strcpy (URL, BASE_URL);
		URL = strcat (URL, CAPTCHA_2CHAPTCHA);
		URL = strcat (URL, "id");
		curl_easy_setopt (curl_handle, CURLOPT_URL, URL);

		const short postfields_length = 6+strlen(board)+8+strlen(thread)+1;
		char *postfields = (char*) calloc (postfields_length, sizeof(char));
		if (postfields == NULL) {
			fprintf (stderr, "[get2chaptchaIdJSON]! Error allocating memory (POST data)\n");
			curl_easy_cleanup (curl_handle);
			free (URL);
			makaba_errno = ERR_MEMORY;
			return NULL;
		}
		postfields = strcpy (postfields, "board=");
		postfields = strcat (postfields, board);
		if (thread != NULL) {
			postfields = strcat (postfields, "&thread=");
			postfields = strcat (postfields, thread);
		}
		curl_easy_setopt (curl_handle, CURLOPT_POSTFIELDS, postfields);

		if (CURL_BUFF_BODY == NULL) {
			fprintf(stderr, "[get2chaptchaIdJSON]! Error: curl body buffer not allocated\n");
			makaba_errno = ERR_MAKABA_SETUP;
			return NULL;
		}
		curl_easy_setopt (curl_handle, CURLOPT_WRITEDATA, CURL_BUFF_BODY);

		curl_easy_setopt (curl_handle, CURLOPT_WRITEFUNCTION, CURL_writeToBuff);

		request_status = curl_easy_perform (curl_handle);
		CURL_BUFF_BODY[CURL_BUFF_POS] = 0;
		CURL_BUFF_POS = 0;
		if (request_status != CURLE_OK) {
			fprintf (stderr, "[get2chaptchaIdJSON]! Error @ curl_easy_perform: %s\n",
			curl_easy_strerror(request_status));
			curl_easy_cleanup (curl_handle);
			free (URL);
			free (postfields);
			makaba_errno = ERR_CURL_PERFORM;
			return NULL;
		}

		curl_easy_cleanup (curl_handle);
		free (URL);
		free (postfields);
	}
	else {
		fprintf (stderr, "[get2chaptchaIdJSON]! Error initializing curl handle\n");
		makaba_errno = ERR_CURL_INIT;
		return NULL;
	}

	fprintf(stderr, "]] Exiting get2chaptchaIdJSON\n");

	return CURL_BUFF_BODY;
}

char *form2chaptchaPicURL (const char *id) {
	fprintf (stderr, "]] Starting get2chaptchaPicURL\n");

	const long int URL_length = strlen(BASE_URL)+strlen(CAPTCHA_2CHAPTCHA)+6+strlen(id)+1;
	char *URL = (char*) calloc (URL_length, sizeof(char));
	if (URL == NULL) {
			fprintf (stderr, "[get2chaptchaPicURL]! Error allocating memory (URL)\n");
			makaba_errno = ERR_MEMORY;
			return NULL;
	}
	URL = strcpy (URL, BASE_URL);
	URL = strcat (URL, CAPTCHA_2CHAPTCHA);
	URL = strcat (URL, "image/");
	URL = strcat (URL, id);

	fprintf(stderr, "]] Exiting get2chaptchaPicURL\n");

	return URL;
}

char *get2chaptchaPicPNG (const char *URL, long int *pic_size) {
	fprintf(stderr, "]] Starting get2chaptchaPicPNG\n");

	CURL *curl_handle = curl_easy_init();
	CURLcode request_status;
	if (curl_handle) {
		curl_easy_setopt (curl_handle, CURLOPT_URL, URL);

		if (CURL_BUFF_BODY == NULL) {
			fprintf(stderr, "[get2chaptchaPicPNG]! Error: curl body buffer not allocated\n");
			makaba_errno = ERR_MAKABA_SETUP;
			return NULL;
		}
		curl_easy_setopt (curl_handle, CURLOPT_WRITEDATA, CURL_BUFF_BODY);

		curl_easy_setopt (curl_handle, CURLOPT_WRITEFUNCTION, CURL_writeToBuff);

		request_status = curl_easy_perform (curl_handle);
		CURL_BUFF_BODY[CURL_BUFF_POS] = 0;
		*pic_size = CURL_BUFF_POS;
		CURL_BUFF_POS = 0;
		if (request_status != CURLE_OK) {
			fprintf (stderr, "[get2chaptchaPicPNG]! Error @ curl_easy_perform: %s\n",
				curl_easy_strerror(request_status));
			curl_easy_cleanup (curl_handle);
			makaba_errno = ERR_CURL_PERFORM;
			return NULL;
		}

		curl_easy_cleanup (curl_handle);
	}
	else {
		fprintf (stderr, "[get2chaptchaPicPNG]! Error initializing curl handle\n");
		makaba_errno = ERR_CURL_INIT;
		return NULL;
	}

	fprintf(stderr, "]] Exiting get2chaptchaPicPNG\n");

	return CURL_BUFF_BODY;
}

// ========================================
// Posting
// ========================================

int sendPost (const char *board, const char *thread,
	const char *comment, const char *subject, const char *name, const char *email,
	const char *captcha_id, const char *captcha_value, long long *answer_length)
{
	fprintf (stderr, "]] Starting sendPost\n");

	CURL *curl_handle = curl_easy_init();
	CURLcode request_status;
	if (curl_handle) {
		// URL: https://2ch.hk/makaba/posting.fcgi
		short URL_length = strlen(BASE_URL) + strlen(POSTING_API);
		char *URL = (char *) calloc (URL_length, sizeof(char));
		if (URL == NULL) {
			fprintf (stderr, "[sendPost]! Error allocating memory (URL)\n");
			curl_easy_cleanup (curl_handle);
			return ERR_MEMORY;
		}
		URL = strcpy (URL, BASE_URL);
		URL = strcat (URL, POSTING_API);
		fprintf(stderr, "url = %s\n", URL);
		curl_easy_setopt (curl_handle, CURLOPT_URL, URL);

		/* Postfields:
		 * = json=1
		 * = task=post
		 * = captcha_type=2chaptcha
		 * ! board=
		 * ! thread=
		 *   name=
		 *   email=
		 *   subject=
		 * ! comment=
		 * ! 2chaptcha_id=
		 * ! 2chaptcha_value=
		 * = - постоянные значения
		 * ! - обязательные поля
		 */
		short postfields_length = strlen(POSTING_FIELDS);
		if (board != NULL) {
			postfields_length += 7 + strlen(board);
		}
		else {
			fprintf(stderr, "[sendPost]! Error: board given as NULL\n");
			curl_easy_cleanup(curl_handle);
			free(URL);
			return ERR_ARGS;
		}
		if (thread != NULL) {
			postfields_length += 8 + strlen(thread);
		}
		else {
			postfields_length += 9; // thread=0 - создать тред
		}
		if (comment != NULL) { // Проверить тут потом на длину <= 15000
			postfields_length += 9 + strlen(comment);
		}
		else {
			fprintf(stderr, "[sendPost]! Error: comment given as NULL\n");
			curl_easy_cleanup(curl_handle);
			free(URL);
			return ERR_ARGS;
		}
		if (subject != NULL) {
			postfields_length += 9 + strlen(subject);
		}
		if (name != NULL) {
			postfields_length += 6 + strlen(name);
		}
		if (email != NULL) {
			postfields_length += 7 + strlen(email);
		}
		if (captcha_id != NULL) {
			postfields_length += 12 + strlen(captcha_id);
		}
		else {
			fprintf(stderr, "[sendPost]! Error: captcha_id given as NULL\n");
			curl_easy_cleanup(curl_handle);
			free(URL);
			return ERR_ARGS;
		}
		if (captcha_value != NULL) {
			postfields_length += 15 + strlen(captcha_value);
		}
		else {
			fprintf(stderr, "[sendPost]! Error: captcha_value given as NULL\n");
			curl_easy_cleanup(curl_handle);
			free(URL);
			return ERR_ARGS;
		}
		char *postfields = (char *) calloc (postfields_length, sizeof(char));
		if (postfields == NULL) {
			fprintf (stderr, "[sendPost]! Error allocating memory (POST data)\n");
			curl_easy_cleanup (curl_handle);
			free (URL);
			return ERR_MEMORY;
		}
		postfields = strcpy (postfields, POSTING_FIELDS);
		postfields = strcat (postfields, "&board=");
		postfields = strcat (postfields, board);
		postfields = strcat (postfields, "&thread=");
		if (thread != NULL) {
			postfields = strcat (postfields, thread);
		}
		else {
			postfields = strcat (postfields, "0");
		}
		postfields = strcat (postfields, "&comment=");
		postfields = strcat (postfields, comment);
		if (subject != NULL) {
			postfields = strcat (postfields, "&subject=");
			postfields = strcat (postfields, subject);
		}
		if (name != NULL) {
			postfields = strcat (postfields, "&name=");
			postfields = strcat (postfields, name);
		}
		if (email != NULL) {
			postfields = strcat (postfields, "&email=");
			postfields = strcat (postfields, email);
		}
		postfields = strcat (postfields, "&2chaptcha_id=");
		postfields = strcat (postfields, captcha_id);
		postfields = strcat (postfields, "&2chaptcha_value=");
		postfields = strcat (postfields, captcha_value);
		fprintf(stderr, "postfields = %s\n", postfields);
		curl_easy_setopt (curl_handle, CURLOPT_POSTFIELDS, postfields);

		if (CURL_BUFF_BODY == NULL) {
			fprintf(stderr, "[sendPost]! Error: curl body buffer not allocated\n");
			return ERR_MAKABA_SETUP;
		}
		curl_easy_setopt (curl_handle, CURLOPT_WRITEDATA, CURL_BUFF_BODY);
		curl_easy_setopt (curl_handle, CURLOPT_WRITEFUNCTION, CURL_writeToBuff);

		curl_easy_setopt (curl_handle, CURLOPT_USERAGENT, CURL_UA);

		request_status = curl_easy_perform (curl_handle);
		CURL_BUFF_BODY[CURL_BUFF_POS] = 0;
		*answer_length = CURL_BUFF_POS;
		CURL_BUFF_POS = 0;
		if (request_status != CURLE_OK) {
			fprintf (stderr, "[sendPost]! Error @ curl_easy_perform: %s\n",
			curl_easy_strerror(request_status));
			curl_easy_cleanup (curl_handle);
			free (URL);
			free (postfields);
			return ERR_CURL_PERFORM;
		}

		curl_easy_cleanup (curl_handle);
		free (URL);
		free (postfields);
	}
	else {
		fprintf (stderr, "[sendPost]! Error initializing curl handle\n");
		return ERR_CURL_INIT;
	}

	fprintf(stderr, "]] Exiting sendPost\n");

	return 0;
}

// ========================================
// Misc utility functions
// ========================================

void makabaSetup() {
	if (CURL_BUFF_BODY == NULL)
		CURL_BUFF_BODY = (char*) calloc (CURL_BUFF_BODY_SIZE, sizeof(char));
}

void makabaCleanup() {
	if (CURL_BUFF_BODY != NULL)
		free(CURL_BUFF_BODY);
}

size_t CURL_writeToBuff (const char *src, const size_t block_size, const size_t nmemb, void *dest) {
	if (src==NULL || CURL_BUFF_POS+block_size*nmemb > CURL_BUFF_BODY_SIZE) {
		return 0;
	}
	else {
		memcpy (dest + CURL_BUFF_POS, src, block_size * nmemb);
		CURL_BUFF_POS += block_size*nmemb;
		return block_size*nmemb;
	}
}

char *lint2str (const long int val) {
	short length = 0;
	for (int k = 1; k <= val; k*=10) {
		length += 1;
	}
	char *res = (char*) calloc (length+1, sizeof(char));
	if (res != NULL) {
	}
	else {
		fprintf (stderr, "[lint2str]! Error allocating memory (res)\n");
		return NULL;
	}
	for (int i = 1, k = 1; i <= length; i+=1, k*=10) {
		res[length-i] = '0' + ((val/k)%10);
	}
	return res;
}

long int str2lint (const char *str, const long int len) {
	long int res = 0;
	for (long int i = 1, k = 1; i <= len; i+=1, k*=10) {
		res += k  *(str[len-i] - '0');
	}
	return res;
}

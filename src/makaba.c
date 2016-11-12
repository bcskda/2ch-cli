// ========================================
// File: makaba.c
// Makaba API-related functions
// (Implementation)
// ========================================
// TODO:
//[ ] struct post -> num
//[ ] captcha
//[ ] fix ^J in comment.text
// ========================================

#include "makaba.h"

// ========================================
// Info getting
// ========================================

int getBoardsList (const char* resFile, const bool v) {

	return 0;
}

char* getBoardPageJSON (const char* board, const unsigned page, const bool v) {
	fprintf (stderr, "]] Starting getBoardPage");
	if (v) fprintf (stderr, " (verbose)"); fprintf (stderr, "\n");
	if (v) fprintf (stderr, "] initializing curl handle\n");
	CURL* curl_handle = curl_easy_init();
	CURLcode request_status = 0;
	if (curl_handle) {
		if (v) fprintf (stderr, "] curl handle initialized\n");
		char* page_string = unsigned2str (page);
		if (v) fprintf (stderr, "page number (string) = %s\n", page_string);
		short URL_length = strlen(BASE_URL)+strlen(board)+1+strlen(page_string)+5;
		if (v) fprintf (stderr, "URL length = %d\n", URL_length);
		// URL format: 2ch.hk/$board/$page.json
		char* URL = (char*) calloc (sizeof(char), URL_length);
		if (URL != NULL) {
			if (v) fprintf (stderr, "memory allocated (URL)\n");
		}
		else {
			fprintf (stderr, "[getBoardPage]! Error allocating memory (URL)\n");
			curl_easy_cleanup (curl_handle);
			return ERR_MEMORY_LEAK;
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

		CURL_BUFF_BODY = (char*) calloc (sizeof(char), CURL_BUFF_BODY_SIZE);
		if (CURL_BUFF_BODY != NULL) {
			if (v) fprintf (stderr, "memory allocated (curl body buffer)\n");
		}
		else {
			fprintf (stderr, "[getBoardPage]! Error allocating memory (curl body buffer)\n");
			curl_easy_cleanup (curl_handle);
			free (URL);
			return ERR_MEMORY_LEAK;
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
			fprintf (stderr, "[getBoardPage]! Error @ curl_easy_perform: %s\n",
				curl_easy_strerror(request_status));
			curl_easy_cleanup (curl_handle);
			free (URL);
			free (CURL_BUFF_BODY);
			return ERR_CURL_PERFORM;
		}

		curl_easy_cleanup (curl_handle);
		if (v) fprintf (stderr, "] curl cleanup done\n");
		free (URL);
		fprintf (stderr, "]] Exiting getBoardPage\n");
	}
	else {
		fprintf (stderr, "! Error initializing curl handle\n");
		return ERR_CURL_INIT;
	}

	return CURL_BUFF_BODY;
}

char* getBoardCatalogJSON (const char* board, const bool v) {
	fprintf (stderr, "]] Starting getBoardCatalog");
	if (v) fprintf (stderr, " (verbose)"); fprintf (stderr, "\n");
	if (v) fprintf (stderr, "] initializing curl handle\n");
	CURL* curl_handle = curl_easy_init();
	CURLcode request_status = 0;
	if (curl_handle) {
		if (v) fprintf (stderr, "] curl handle initialized\n");
		short URL_length = strlen(BASE_URL)+strlen(board)+1+7+5;
		// URL format: 2ch.hk/$board/catalog.json
		if (v) fprintf (stderr, "URL length = %d\n", URL_length);
		char* URL = (char*) calloc (sizeof(char), URL_length);
		if (URL != NULL) {
			if (v) fprintf (stderr, "memory allocated (URL)\n");
		}
		else {
			fprintf (stderr, "[getBoardCatalog]! Error allocating memory (URL)\n");
			curl_easy_cleanup (curl_handle);
			return ERR_MEMORY_LEAK;
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

		CURL_BUFF_BODY = (char*) calloc (sizeof(char), CURL_BUFF_BODY_SIZE);
		if (CURL_BUFF_BODY != NULL) {
			if (v) fprintf (stderr, "memory allocated (curl body buffer)\n");
		}
		else {
			fprintf (stderr, "[getBoardCatalog]! Error allocating memory (curl body buffer)\n");
			curl_easy_cleanup (curl_handle);
			free (URL);
			return ERR_MEMORY_LEAK;
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
			free (CURL_BUFF_BODY);
			return ERR_CURL_PERFORM;
		}

		curl_easy_cleanup (curl_handle);
		if (v) fprintf (stderr, "] curl cleanup done\n");
		free (URL);
		fprintf (stderr, "] memory free done\n");
		fprintf (stderr, "]] Exiting getBoardCatalog\n");
	}
	else {
		fprintf (stderr, "[getBoardCatalog]! Error initializing curl handle\n");
		return ERR_CURL_INIT;
	}

	return CURL_BUFF_BODY;
}

char* getThreadJSON (const char* board, const unsigned threadnum, const bool v) {
	fprintf (stderr, "]] Starting getThread");
	if (v) fprintf (stderr, " (verbose)"); fprintf (stderr, "\n");
	if (v) fprintf (stderr, "] initializing curl handle\n");
	CURL* curl_handle = curl_easy_init();
	CURLcode request_status = 0;
	if (curl_handle) {
		if (v) fprintf (stderr, "] curl handle initialized\n");
		char* threadnum_string = unsigned2str (threadnum);
		if (v) fprintf (stderr, "thread number (string) = %s\n", threadnum_string);
		const short URL_length = strlen(BASE_URL)+strlen(MOBILE_API);
		if (v) fprintf (stderr, "URL length = %d\n", URL_length);
		// API URL: 2ch.hk/makaba/mobile.fcgi
		char* URL = (char*) calloc (sizeof(char), URL_length);
		if (URL != NULL) {
			if (v) fprintf (stderr, "memory allocated (URL)\n");
		}
		else {
			fprintf (stderr, "[getThread]! Error allocating memory (URL)\n");
			curl_easy_cleanup (curl_handle);
			return ERR_MEMORY_LEAK;
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

		const char* postfields = (char*) calloc (sizeof(char),postfields_length);
		if (postfields != NULL) {
			if (v) fprintf (stderr, "memory allocated (POST data)\n");
		}
		else {
			fprintf (stderr, "[getThread]! Error allocating memory (POST data)\n");
			curl_easy_cleanup (curl_handle);
			free (URL);
			return ERR_MEMORY_LEAK;
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

		CURL_BUFF_BODY = (char*) calloc (sizeof(char), CURL_BUFF_BODY_SIZE);
		if (CURL_BUFF_BODY != NULL) {
			if (v) fprintf (stderr, "memory allocated (curl body buffer)\n");
		}
		else {
			fprintf (stderr, "[getThread]! Error allocating memory (curl body buffer)\n");
			curl_easy_cleanup (curl_handle);
			free (URL);
			free (postfields);
			return ERR_MEMORY_LEAK;
		}
		curl_easy_setopt (curl_handle, CURLOPT_WRITEDATA, CURL_BUFF_BODY);
		if (v) fprintf (stderr, "] option WRITEDATA set\n");

		curl_easy_setopt (curl_handle, CURLOPT_WRITEFUNCTION, CURL_writeToBuff);
		if (v) fprintf (stderr, "] option WRITEFUNCTION set\n");

		request_status = curl_easy_perform (curl_handle);
		if (v) fprintf (stderr, "] curl request performed\n");
		if (request_status == CURLE_OK) {
			if (v) fprintf (stderr, "request status: OK\n");
		}
		else {
			fprintf (stderr, "[getThread]! Error @ curl_easy_perform: %s\n",
				curl_easy_strerror(request_status));
			curl_easy_cleanup (curl_handle);
			free (URL);
			free (postfields);
			free (CURL_BUFF_BODY);
			return ERR_CURL_PERFORM;
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
		return ERR_CURL_INIT;
	}

	return CURL_BUFF_BODY;
}

// ========================================
// Info parsing
// ========================================

int* findPostsInJSON (const char* src, int* postcount_res, const bool v) {
	fprintf (stderr, "]] Starting findPostsInJSON");
	if (v) fprintf (stderr, " (verbose)"); fprintf (stderr, "\n");
	short srclen = strlen (src);
	int* temp = (int*) calloc (sizeof(int),srclen/8);

	short depth = 0;
	int postcount = 0;
	bool comment_read = false;

	for (int i = 1; i < srclen; i+=1) {
		switch (depth) {
			case 2:
				if (comment_read) { // in: .post.files
					if (src[i] == ']') {
						if (v) fprintf (stderr, "Exiting files. ");
						depth -= 1;
					}
				}
				else { // in: .post.comment
					if (src[i] == '"') {
						if (v) fprintf (stderr, "Exiting comment. ");
						comment_read = true;
						depth -= 1;
					}
				}
				continue;
			case 1: // in: .post
				if (src[i] == '}') { //@TODO reverse order of ifs in 'case 1'
					if (v) fprintf (stderr, "Exiting post (@%d).\n", i);
					postcount += 1;
					depth -= 1;
					comment_read = false;
				}
				if ((src[i-2]=='"')&&(src[i-1]=='f')&&(src[i]=='i')&&
					(src[i+1]=='l')&&(src[i+2]=='e')) {
					if (v) fprintf (stderr, "Entering files. ");
					depth += 1;
				}
				if ((src[i-2]=='"')&&(src[i-1]=='c')&&(src[i]=='o')&&
					(src[i+1]=='m')&&(src[i+2]=='m')) {
					if (v) fprintf (stderr, "Entering comment. ");
					depth += 1;
				}
				continue;
			case 0: // in: .
				if (src[i] == '{') {
					if (v) fprintf (stderr, "Entering post #%d ", postcount+1);
					temp[postcount] = i;
					if (v) fprintf (stderr, "(@%d)\n", temp[postcount]);
					depth += 1;
				}
				continue;
			default:
				fprintf (stderr, "! Error: Unusual depth (%d)\n", depth);
				free (temp);
				return ERR_PARTTHREAD_DEPTH;
		}
	}
	if (depth != 0) {
		fprintf (stderr, "! Error: Non-zero depth (%d) after cycle\n", depth);
		return ERR_PARTTHREAD_DEPTH;
	}

	if (v) fprintf (stderr, "Total: %d posts found\n", postcount);

	int* posts = (int*) calloc (sizeof(int), postcount);
	posts = memcpy (posts, temp, postcount*sizeof(int));
	free (temp);
	if (v) fprintf (stderr, "Posts begin at:\n");
	if (v) for (int i = 0; i < postcount; i++)
		fprintf (stderr, "] %2d: %5d\n", i, posts[i]);

	*postcount_res = postcount;
	fprintf (stderr, "]] Exiting findPostsInJSON\n");
	return posts;
}

struct post* initPost (const char* post_string, const short postlen, const bool v) {
	fprintf(stderr, "]] Starting initPost");
	FILE* LOCAL_LOG = NULL;
	if (v) LOCAL_LOG = fopen("log/initPost", "a");
	if (v) fprintf(stderr, " (verbose, log in ./log/initPost)"); fprintf (stderr, "\n");
	if (v) fprintf(LOCAL_LOG, "]] Args:\n]]| %s\n]]| %d\n", post_string, postlen);

	// Detecting comment:
	char* ptr_comment = strstr (post_string, PATTERN_COMMENT) + strlen (PATTERN_COMMENT);
	if (ptr_comment == NULL) {
		fprintf (stderr, "! Error: Bad post format: Comment pattern not found\n");
		return ERR_POST_FORMAT;
	}

	
	fprintf(LOCAL_LOG, "\nptr_comment: \n");
	for (int i = 0; i < 25; i++)  {
		fprintf(LOCAL_LOG, "%c", post_string[ptr_comment-post_string+i]);
	}
	fprintf(LOCAL_LOG, "\n");


	short comment_len = 0; bool stop = false;
	fprintf(LOCAL_LOG, "i: [%d;%d]\n", ptr_comment-post_string, postlen);
	for (int i = ptr_comment-post_string; !stop && (i < postlen); i++) {
		fprintf(LOCAL_LOG, "%c", post_string[i]);
		if ((post_string[i-1] != '\\') && (post_string[i] == '\"') && (post_string[i+1] != 'u')) {
			stop = true;
			fprintf(LOCAL_LOG, "\nStop @ i=%d\n", i);
		}
		comment_len++;
	}
	if (!stop) {
		fprintf(LOCAL_LOG, "\n=== ! Range violation! ===\n");
	}
	fprintf(LOCAL_LOG, "\nEND\n\n");
	comment_len--;
	if (comment_len <= 0) {
		fprintf (stderr, "! Error: Bad post format: Null comment\n");
		return ERR_POST_FORMAT;
	}
	if (v) fprintf (LOCAL_LOG, "] Comment length: %d\n", comment_len);
	if (v) fprintf (LOCAL_LOG, "] Comment: \n");
	if (v) for (int i = 0; i < comment_len; i++)
		fprintf (LOCAL_LOG, "%c", ptr_comment[i]);
	if (v) fprintf (LOCAL_LOG, "\n] End of comment\n");

	// Detect date:

	char* ptr_date = strstr (ptr_comment+comment_len, PATTERN_DATE)+strlen(PATTERN_DATE);
	if (ptr_date == NULL) {
		fprintf (stderr, "! Error: Bad post format: Date pattern not found\n");
		return ERR_POST_FORMAT;
	}
	short date_len = strstr (ptr_date, "\"")-ptr_date;
	if (v) {
		fprintf (LOCAL_LOG, "] Date length: %d\n", date_len);
		fprintf (LOCAL_LOG, "] Date: ");
		for (int i = 0; i < date_len; i++)
			fprintf (LOCAL_LOG, "%c", ptr_date[i]);
		fprintf (LOCAL_LOG, "\n");

	}

	// Detect email:
	char* ptr_email = strstr (ptr_date+date_len, PATTERN_EMAIL)+strlen(PATTERN_EMAIL);
	if (ptr_email == NULL) {
		fprintf (stderr, "! Error: Bad post format: Email pattern not found\n");
		return ERR_POST_FORMAT;
	}
	short email_len = strstr (ptr_email, "\"")-ptr_email;
	if (v) if (email_len == 0) {
		fprintf (LOCAL_LOG, "] Email not specified\n");
	} else {
		fprintf (LOCAL_LOG, "] Email length: %d\n", email_len);
		fprintf (LOCAL_LOG, "] Email: ");
		for (int i = 0; i < email_len; i++)
			fprintf (LOCAL_LOG, "%c", ptr_email[i]);
		fprintf (LOCAL_LOG, "\n");
	}

	// Detect files:
	char* ptr_files = strstr (ptr_email+email_len, PATTERN_FILES)+strlen(PATTERN_FILES);
	short files_len = 0;
	// If NULL, simply no files in post
	if (ptr_files-strlen(PATTERN_FILES) == NULL) {
		if (v) fprintf (LOCAL_LOG, "] Files not specified\n");
	} else {
		files_len = strstr (ptr_files, "}]")-ptr_files;
		if (files_len == 0) {
			fprintf (stderr, "! Error: Bad post format: Files field specified but null\n");
			return ERR_POST_FORMAT;
		} else
			if (v) {
				fprintf(LOCAL_LOG, "] Files length: %d\n", files_len);
				fprintf (LOCAL_LOG, "] Files: \n");
				for (int i = 0; i < files_len; i++) {
					fprintf (LOCAL_LOG, "%c", ptr_files[i]);
				}
				fprintf (LOCAL_LOG, "\n] End of files\n");
			}
	}

	// Detect name:
	char* ptr_name = 0; char* ptr_name_diff = 0; short name_diff_len = 0;
	if (ptr_files-strlen(PATTERN_FILES) == NULL) {
		ptr_name_diff = ptr_email; // Files field may include "\"name\":" substring,
		name_diff_len = email_len; // so we must ignore the files field, if it exists.
	} else {                     // However, if it is not specified, we cannot use
		ptr_name_diff = ptr_files; // ptr_files. So we split into 2 cases and use
		name_diff_len = files_len; // 2 variants of values for 2 new pointers.
	}
	ptr_name = strstr (ptr_name_diff+name_diff_len, PATTERN_NAME)+strlen(PATTERN_NAME);
	if (ptr_name == NULL) {
		fprintf (stderr, "! Error: Bad post format: Name pattern not found\n");
		return ERR_POST_FORMAT;
	}
	short name_len = strstr (ptr_name, "\"")-ptr_name;
	if (v) fprintf (LOCAL_LOG, "] Name length: %d\n", name_len);
	if (v) fprintf (LOCAL_LOG, "] Name: ");
	if (v) for (int i = 0; i < name_len; i++)
		fprintf (LOCAL_LOG, "%c", ptr_name[i]);
	if (v) fprintf (LOCAL_LOG, "\n");
  
	// Detect postnum
	char* ptr_num = strstr (ptr_name+name_len,PATTERN_NUM) + strlen(PATTERN_NUM);
	if (ptr_num == NULL) {
		fprintf (stderr, "! Error: Bad post format: Num pattern not found\n");
		return ERR_POST_FORMAT;
	}
	short num_len = strstr (ptr_num, ",")-ptr_num;
	if (v) fprintf (LOCAL_LOG, "] Num length: %d\n", num_len);
	if (v) fprintf (LOCAL_LOG, "] Num: ");
	if (v) for (int i = 0; i < num_len; i++)
		fprintf (LOCAL_LOG, "%c", ptr_num[i]);
	if (v) fprintf (LOCAL_LOG, "\n");

	if (v) fprintf (LOCAL_LOG, "] = All main fields detected\n");

	// Init struct:
	struct post* post = (struct post*) calloc (1,sizeof(struct post));

	post->num = str2unsigned(ptr_num, num_len);

	char* comment_str = (char*) calloc (comment_len, sizeof(char));
	memcpy(comment_str, ptr_comment, comment_len);

	post->comment = parseComment (comment_str, true);
	if (post->comment == NULL) {
		fprintf (stderr, "! Error parsing comment\n");
		return ERR_COMMENT_PARSING;
	}
	free (comment_str);
	fprintf (LOCAL_LOG, "!! post.comment.nrefs = %d\n", post->comment->nrefs);
	fprintf (LOCAL_LOG, "!! post.comment.refs[0].link = %s\n", post->comment->refs[0].link);
	fprintf (LOCAL_LOG, "!! post.comment.text = %s\n", post->comment->text);

	post->date = (char*) calloc (sizeof(char), date_len+1);
	if (post->date == NULL) {
		fprintf (stderr, "! Error allocating memory (post.date)\n");
		return ERR_MEMORY_LEAK;
	}
	memcpy (post->date, ptr_date, sizeof(char)*date_len);

	post->name = (char*) calloc (sizeof(char), name_len+1);
	if (post->name == NULL) {
		fprintf (stderr, "! Error allocating memory (post.name)\n");
		return ERR_MEMORY_LEAK;
	}
	memcpy (post->name, ptr_name, sizeof(char)*name_len);
	if (email_len == 0) {
		post->email = NULL;
	} else {
		post->email = (char*) calloc (sizeof(char), email_len+1);
		if (post->email == NULL) {
		fprintf (stderr, "! Error allocating memory (post.email)\n");
		return ERR_MEMORY_LEAK;
	}
		memcpy (post->email, ptr_email, sizeof(char)*email_len);
	}
	if (files_len == 0) {
		post->files = NULL;
	} else {
		post->files = (char*) calloc (sizeof(char), files_len+1);
		if (post->files == NULL) {
		fprintf (stderr, "! Error allocating memory (post.files)\n");
		return ERR_MEMORY_LEAK;
	}
		memcpy (post->files, ptr_files, sizeof(char)*files_len);
	}
	
	if (v) fprintf (LOCAL_LOG, "] = Init struct done\n");
	fprintf (stderr, "]] Exiting initPost\n");
	fclose(LOCAL_LOG);
	return post;
}

struct comment* parseComment (char* comment, const bool v) {
	fprintf (stderr, "]] Started parseComment");
	FILE* LOCAL_LOG = NULL;
	if (v) LOCAL_LOG = fopen("log/parseComment", "a");
	if (v) fprintf (stderr, " (verbose, log in ./log/parseComment)"); fprintf(LOCAL_LOG, "\n");
	fprintf (LOCAL_LOG, "\n");
	if (v) fprintf(LOCAL_LOG, "]] Args:\n]]| %s\n", comment);
	
	unsigned comment_len = strlen (comment);
	int nrefs = 0;
	char* cinst = strstr(comment,PATTERN_HREF_OPEN);
	size_t pos = 0;
	struct list* refs = calloc (1, sizeof(struct list));
	refs->first = refs;
	refs->data = 0;
	refs->next = 0;
	struct ref_reply* cref = 0;
	unsigned clean_len = 0;
	char* clean_comment = (char*) calloc (comment_len, sizeof(char));
	for ( ; cinst != NULL; cinst = strstr(pos+comment,PATTERN_HREF_OPEN)) {
		if (v) fprintf (LOCAL_LOG, "]]] New ref\n]]]] Opened @ [%d]\n", cinst-comment);
		pos = cinst-comment;
		char* cinst_end = strstr (cinst, PATTERN_HREF_CLOSE);
		if (cinst_end == NULL) {
			fprintf (stderr, "! Error: ref opened but not closed\n");
			fprintf (LOCAL_LOG, "-- Comment: \"%s\"\n", comment);
			return ERR_COMMENT_FORMAT;
		}
			if (v) fprintf (LOCAL_LOG, "]]]] Closed @ [%d]\n", cinst_end-comment);
			char* class = strstr(cinst, PATTERN_REPLY_CLASS);
			if (class != NULL) {
				if (v) fprintf (LOCAL_LOG, "]]]] Type: reply\n");
				nrefs++;
				cref = parseRef_Reply (cinst, cinst_end-cinst, true);
				memcpy (clean_comment+clean_len, ">>", sizeof(char)*2);
				clean_len += 2;
				short csize = strlen (unsigned2str(cref->num));
				memcpy (clean_comment+clean_len, unsigned2str(cref->num), sizeof(char)*csize);
				clean_len += csize;
				if (v) fprintf (LOCAL_LOG, "]]]] Init done:\n");
				if (v) fprintf (LOCAL_LOG, "]]]]] link=\"%s\"\n]]]]] thread=%d\n]]]]] num=%d\n",
												cref->link, cref->thread, cref->num);
				if (v) fprintf(LOCAL_LOG, "Adding new ref #%d\n", nrefs);
				if (nrefs > 1) {
					refs->next = (struct list*) calloc (1, sizeof(struct list));
					refs->next->first = refs->first;
					refs->next->next = 0;
					refs->next->data = calloc (1, sizeof(struct ref_reply));
					memcpy (refs->next->data, (void*) cref, sizeof(struct ref_reply));
					refs = refs->next;
					if (v) fprintf(LOCAL_LOG, "List member state: 1st = %p, cur = %p, next = %p\n", refs->first, refs, refs->next);
				} else {
					refs->next = 0;
					refs->data = calloc (1, sizeof(struct ref_reply));
					memcpy (refs->data, (void*) cref, sizeof(struct ref_reply));
					if (v) fprintf(LOCAL_LOG, "Filling 1st list member\n");
 					if (v) fprintf(LOCAL_LOG, "List member state: 1st = %p, cur = %p, next = %p\n", refs->first, refs, refs->next);
				}
			} else {
				fprintf (LOCAL_LOG, "]]]] Unknown ref type, treat as text\n");
			}
			short csize = cinst-(comment+pos);
			fprintf(LOCAL_LOG, "[!] From #%d: %s\n", pos, comment+pos);
			fprintf(LOCAL_LOG, "[!] To: %p+%d\n", clean_comment, clean_len);
			fprintf(LOCAL_LOG, "[!] Size: %d\n", csize);
			if (csize > 0) {
				memcpy (clean_comment+clean_len, comment+pos, sizeof(char)*csize);
				clean_len += csize;
			}
			pos = cinst_end-comment+strlen(PATTERN_HREF_CLOSE);
			free (cref);
	}
	short csize = comment_len-pos;
	fprintf(LOCAL_LOG, "[!] From #%d: %s\n", pos, comment+pos);
	fprintf(LOCAL_LOG, "[!] To: %p+%d\n", clean_comment, clean_len);
	fprintf(LOCAL_LOG, "[!] Size: %d\n", csize);
	if (csize > 0) {
		memcpy (clean_comment+clean_len, comment+pos, sizeof(char)*csize);
		clean_len += csize;
	}
	char* finally_clean_comment = cleanupComment (clean_comment, clean_len, true);
	if (v) fprintf(LOCAL_LOG, "]]] Total: %d refs\n", nrefs);
	struct comment* parsed = (struct comment*) calloc (1, sizeof(struct comment));
	parsed->text = (char*) calloc (comment_len, sizeof(char));
	memcpy (parsed->text, finally_clean_comment, sizeof(char)*strlen(finally_clean_comment));
	free (clean_comment);
	fprintf(LOCAL_LOG, "text ok: %s\n", parsed->text);
	parsed->nrefs = nrefs;
	fprintf(LOCAL_LOG, "nrefs ok: %d\n", parsed->nrefs);
	parsed->refs = (struct ref_reply*) calloc (nrefs, sizeof(struct ref_reply));
	refs = refs->first;
	for (int i = 0; i < nrefs; i++) {
		if (v) fprintf(LOCAL_LOG, "Copy ref #%d, first = %p, current = %p, next = %p\n", i+1, refs->first, refs, refs->next);
		memcpy (parsed->refs+i*sizeof(struct ref_reply), (void*) refs->data, sizeof(struct ref_reply));
	}
	fprintf (stderr, "]] Exiting parseComment\n");
	return parsed;
}

char* cleanupComment (const char* src, const unsigned src_len, const bool v) {
	fprintf(stderr, "]] Started cleanupComment\n");
	char* buf = (char*) calloc (src_len, sizeof(char));
	unsigned pos = 0, len = 0;
	for ( ; pos < src_len; ) {
		if ((src[pos] == '\\')   && (src[pos+1] == 'u') && (src[pos+2] == '0') && (src[pos+3] == '0')
			&& (src[pos+4] == '3') && (src[pos+5] == 'c') && (src[pos+6] == 'b') && (src[pos+7] == 'r')) {
			pos += 14;
			buf[len] = '\n';
			len++;
		} else if ((src[pos   ] == '\\') && (src[pos+1 ] == 'u' ) && (src[pos+2 ] == '0' ) && (src[pos+3 ] == '0')
				    && (src[pos+4 ] == '3' ) && (src[pos+5 ] == 'c' ) && (src[pos+6 ] == 's' ) && (src[pos+7 ] == 'p')
					  && (src[pos+8 ] == 'a' ) && (src[pos+9 ] == 'n' ) && (src[pos+10] == ' ' ) && (src[pos+11] == 'c')
					  && (src[pos+12] == 'l' ) && (src[pos+13] == 'a' ) && (src[pos+14] == 's' ) && (src[pos+15] == 's')
					  && (src[pos+16] == '=' ) && (src[pos+17] == '\\') && (src[pos+18] == '"' ) && (src[pos+19] == 'u')
					  && (src[pos+20] == 'n' ) && (src[pos+21] == 'k' ) && (src[pos+22] == 'f' ) && (src[pos+23] == 'u')
					  && (src[pos+24] == 'n' ) && (src[pos+25] == 'c' ) && (src[pos+26] == '\\') && (src[pos+27] == '"')) {
				pos += 38;
				// Color: green
		} else if ((src[pos   ] == '\\') && (src[pos+1 ] == 'u' ) && (src[pos+2 ] == '0' ) && (src[pos+3 ] == '0')
				    && (src[pos+4 ] == '3' ) && (src[pos+5 ] == 'c' ) && (src[pos+6 ] == '/' ) && (src[pos+7 ] == 's')
					  && (src[pos+8 ] == 'p' ) && (src[pos+9 ] == 'a' ) && (src[pos+10] == 'n' )) {
				pos += 17;
		} else if ((src[pos   ] == '\\') && (src[pos+1 ] == '\\' ) && (src[pos+2 ] == 'r') && (src[pos+3 ] == '\\')
						&& (src[pos+4 ] == '\\') && (src[pos+5 ] == 'n')) {
			pos += 6;
			buf[len] = '\n';
			len++;
		} else {
			buf[len] = src[pos];
			len++;
			pos++;
		}
	}

	char* clean = (char*) calloc (len, sizeof(char));
	memcpy(clean, buf, sizeof(char)*len);
	free (buf);
	fprintf(stderr, "]] Exiting cleanupComment\n");
	return clean;
}

struct ref_reply* parseRef_Reply (const char* ch_ref, const int ref_len, const bool v) {
	if (v) fprintf (stderr, "-]] Started parseRef_Reply (verbose)\n");

	char* link_start = ch_ref + strlen (PATTERN_HREF_OPEN);
	short link_len = strstr (ch_ref, PATTERN_REPLY_CLASS) - 3 - link_start;
													// '-3' to exclude JSON 

	char* data_thread_start = strstr (ch_ref, PATTERN_REPLY_THREAD) + strlen(PATTERN_REPLY_THREAD);
	if (data_thread_start == NULL) { // '+ strlen' to exclude opening string
		fprintf (stderr, "! Error: no data-thread in reply-ref\n");
		return ERR_REF_FORMAT;
	}
	char* data_thread_end = strstr (data_thread_start, "\\\"") - 1;
	if (data_thread_end == NULL-1) {  // '- 1' to exclude '\'
		fprintf (stderr, "! Error: data-thread opened but not closed\n");
		return ERR_REF_FORMAT;
	}
	unsigned data_thread = str2unsigned (data_thread_start, data_thread_end-data_thread_start+1);
	if (v) fprintf (stderr, "-]]] Thread: %d\n", data_thread);

	char* data_num_start = strstr (ch_ref, PATTERN_REPLY_NUM) + strlen(PATTERN_REPLY_NUM);
	if (data_num_start == NULL) { // '+ strlen' to exclude opening string
		fprintf (stderr, "! Error: no data-num in reply-ref\n");
		return ERR_REF_FORMAT;
	}
	char* data_num_end = strstr (data_num_start, "\\\"") - 1;
	if (data_thread_end == NULL-1) {  // '- 1' to exclude '\'
		fprintf (stderr, "! Error: data-num opened but not closed\n");
		return ERR_REF_FORMAT;
	}
	unsigned data_num = str2unsigned (data_num_start, data_num_end-data_num_start+1);
	if (v) fprintf (stderr, "-]]] Postnum: %d\n", data_num);

	struct ref_reply* ref_parsed = (struct ref_reply*) calloc (1, sizeof(struct ref_reply));
	ref_parsed->link = (char*) calloc (link_len, sizeof(char) + 1);
	memcpy (ref_parsed->link, link_start, link_len*sizeof(char));
	ref_parsed->thread = data_thread;
	ref_parsed->num = data_num;

	fprintf (stderr, "-]] Exiting parseRef_Reply\n");

	return ref_parsed;
}

// ========================================
// Freeing functions
// ========================================

void freeRefReply (struct ref_reply* ref) {
	free (ref->link);
}

void freeComment (struct comment* arg) {
	free (arg->text);
	free (arg->refs);
}

void freePost (struct post* post) {
	freeComment (post->comment);
	free (post->date);
	free (post->name);
	if (post->email != -1)
		free (post->email);
	if (post->files != -1)
		free (post->files);
	free (post);
}

// ========================================
// Misc utility functions
// ========================================

size_t CURL_writeToBuff (const char* src, const size_t block_size, const size_t nmemb, void* dest) {
	if (src==NULL || CURL_BUFF_POS+block_size*nmemb > CURL_BUFF_BODY_SIZE) {
		return 0;
	}
	else {
		//fprintf (stderr, "writing to buffer (src):\n%s\n", src);
		memcpy (dest+CURL_BUFF_POS, src, block_size*nmemb);
		//fprintf (stderr, "written to buffer (res):\n%s\n", dest);
		CURL_BUFF_POS += block_size*nmemb;
		return block_size*nmemb;
	}
}

char* unsigned2str (const unsigned val) {
	//fprintf (stderr, ">> Entered unsigned2str\n");
	short length = 0;
	for (int k = 1; k <= val; k*=10) {
		length += 1;
	}
	//fprintf (stderr, "> length = %d\n", length);
	char* res = (char*) calloc (sizeof(char), length+1);
	if (res != NULL) {
		//fprintf (stderr, "memory allocated (res)\n");
	}
	else {
		fprintf (stderr, "[unsigned2str]! Error allocating memory (res)\n");
		return NULL;
	}
	for (int i = 1, k = 1; i <= length; i+=1, k*=10) {
		//fprintf (stderr, "i = %d, k = %d\n", i, k);
		//fprintf (stderr, "val %% k = %d\n", (val/k)%10);
		res[length-i] = '0' + ((val/k)%10);
		//fprintf (stderr, "written char: %c\n", res[length-i]);
	}
	//fprintf (stderr, "> returning res = %s\n", res);
	//fprintf (stderr, ">> Exiting unsigned2str\n");
	return res;
}

unsigned str2unsigned (const char* str, const unsigned len) {
	unsigned res = 0;
	for (unsigned i = 1, k = 1; i <= len; i+=1, k*=10) {
		res += k * (str[len-i] - '0');
	}
	return res;
}
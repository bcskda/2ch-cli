#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <stdbool.h>

// ========================================
// File: makaba.h
// Header file for Makaba API-related functions
// ========================================

const char* BASE_URL = "http://2ch.hk/";
const char* MOBILE_API = "makaba/mobile.fcgi";
const size_t CURL_BUFF_BODY_SIZE = 5000000;
const size_t CURL_BUFF_HEADER_SIZE = 2000;
char* CURL_BUFF_BODY = 0;
char* CURL_BUFF_HEADER = 0;
size_t CURL_BUFF_POS = 0;

const int ERR_MEMORY_LEAK = -1,
		  		ERR_UNKNOWN = -2,
		  		ERR_CURL_INIT = -3,
		  		ERR_CURL_PERFORM = -4,
		  		ERR_PARTTHREAD_DEPTH = -5,
					ERR_POST_FORMAT = -6;

int getBoardsList (const char* resFile, const bool v);
char* getBoardPageJSON (const char* board, const unsigned page, bool v);
char* getBoardCatalogJSON (const char* board, const bool v);
char* getThreadJSON (const char* board, const unsigned threadnum, const bool v);
int* findPostsInJSON (const char* src, int* postcount_res, const bool v);

size_t CURL_writeToBuff (const char* src, const size_t size, const size_t nmemb, void* dest);
char* unsigned2str (const unsigned val);

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

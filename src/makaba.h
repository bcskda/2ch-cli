#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <curl/curl.h>

// ========================================
// Functions for Makaba API
// ========================================

const char* BASE_URL = "http://2ch.hk/";
const size_t CURL_BUFF_BODY_SIZE = 5000000;
const size_t CURL_BUFF_HEADER_SIZE = 2000;
char* CURL_BUFF_BODY = 0;
char* CURL_BUFF_HEADER = 0;
size_t CURL_BUFF_POS = 0;

// @TODO GLOBAL ERROR CODES

int getBoardsList (const char* resFile, const bool v);
int getBoardPageJSON (const char* board, const unsigned page, bool v);
int getThreadJSON (const char* board, const char* threadNum_str, const bool v);

size_t curl_writeToBuff (const char* src, const size_t size, const size_t nmemb, void* dest);
char* unsigned2str (const unsigned val);

int getBoardsList (const char* resFile, const bool v) {

  return 0;
}

int getBoardPageJSON (const char* board, const unsigned page, const bool v) {
  fprintf (stderr, "]] Starting getBoardPage\n");
  if (v) fprintf (stderr, "] initializing curl handle\n");
  CURL* curl_handle = curl_easy_init();
  CURLcode request_status = 0;
  if (curl_handle) {
    if (v) fprintf (stderr, "] curl handle initialized\n");
    char* page_string = unsigned2str (page);
    if (v) fprintf (stderr, "page number (string) = %s\n", page_string);
    short URL_length = strlen(BASE_URL)+strlen(board)+1+strlen(page_string)+5;
    if (v) fprintf (stderr, "URL length = %d\n", URL_length);
    char* URL = (char*) calloc (sizeof(char), URL_length);
    if (URL != NULL) {
      if (v) fprintf (stderr, "memory allocated (URL)\n");
    }
    else {
      fprintf (stderr, "! Error allocating memory (URL)\n");
      curl_easy_cleanup (curl_handle);
      return 1;
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
      fprintf (stderr, "! Error allocating memory (curl body buffer)\n");
      curl_easy_cleanup (curl_handle);
      return 1;
    }
    curl_easy_setopt (curl_handle, CURLOPT_WRITEDATA, CURL_BUFF_BODY);
    if (v) fprintf (stderr, "] option WRITEDATA set\n");

    /*
    CURL_BUFF_HEADER = (char*) calloc (sizeof(char), CURL_BUFF_HEADER_SIZE);
    if (CURL_BUFF_HEADER != NULL) {
      if (v) fprintf (stderr, "memory allocated (curl header buffer)\n");
    }
    else {
      fprintf (stderr, "! Error allocating memory (curl header buffer)\n");
      curl_easy_cleanup (curl_handle);
      return 1;
    }
    curl_easy_setopt (curl_handle, CURLOPT_WRITEHEADER, CURL_BUFF_HEADER);
    if (v) fprintf (stderr, "] option WRITEHEADER set\n");
    */

    curl_easy_setopt (curl_handle, CURLOPT_WRITEFUNCTION, curl_writeToBuff);
    if (v) fprintf (stderr, "] option WRITEFUNCTION set\n");

    request_status = curl_easy_perform (curl_handle);
    if (v) fprintf (stderr, "] curl request performed\n");
    if (request_status == CURLE_OK) {
      if (v) fprintf (stderr, "request status: OK\n");
      printf ("%s\n", CURL_BUFF_BODY);
    }
    else {
      fprintf (stderr, "! Error @ curl_easy_perform: %s\n",
        curl_easy_strerror(request_status));
      free (CURL_BUFF_BODY);
      curl_easy_cleanup (curl_handle);
      return 3;
    }

    curl_easy_cleanup (curl_handle);
    if (v) fprintf (stderr, "] curl cleanup done\n");
    fprintf (stderr, "]] Exiting getBoardPage\n");
  }
  else {
    fprintf (stderr, "! Error initializing curl handle\n");
    free (CURL_BUFF_BODY);
    return 2;
  }

  free (CURL_BUFF_BODY);

  return 0;
}

int getThreadJSON (const char* board, const char* threadNum_str, const bool v) {
  /*char* args[] = {"-q", "--silent", "-X", "POST", "-F", "task=get_thread",
                  "-F", strcat("board=",board), "-F", strcat("thread=",threadNum_str),
                  "http://2ch.hk/makaba/mobile.fcgi", 0};
  CURL* curl_handle = curl_easy_init();
  CURLcode res = 0;
  if (curl_handle) {
    curl_easy_setopt (curl_handle, CURLOPT_URL,
      strcat("http://2ch.hk/",strcat(board,strcat("/",strcat(threadNum_str,"/")))));
    res = curl_easy_perform (curl_handle);
    curl_easy_cleanup (curl_handle);
  }
  else {
    if (v) fprintf (stderr, "! Error initializing curl\n");
    return 1;
  }
  */
  return 0;
}

// ========================================
// Misc utility functions
// ========================================

size_t curl_writeToBuff (const char* src, const size_t block_size, const size_t nmemb, void* dest) {
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
  //fprintf (stderr, "memory allocated (res)\n");
  for (int i = 1, k = 10; i <= length; i+=1, k*=10) {
    res[length-i] = '0' + (val % k);
    //fprintf (stderr, "written char: %c\n", res[length-i]);
  }
  //fprintf (stderr, "> returning res = %s\n", res);
  //fprintf (stderr, ">> Exiting unsigned2str\n");
  return res;
}

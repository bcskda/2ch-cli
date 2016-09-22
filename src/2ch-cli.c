#include "makaba.h"
#include <ncurses.h>
#include <locale.h>

// ========================================
// File: 2ch-cli.c
// A CLI-client for 2ch.hk imageboard
// written on C
// ========================================

typedef struct Post {
  const char* comment;
  const char* date;
  const char* name;
  const char* email;
  const char* files;
} Post;

const char* PATTERN_COMMENT = ",\"comment\":\"";
const char* PATTERN_DATE = ",\"date\":\"";
const char* PATTERN_SUBJECT = ",\"subject\":\"";
const char* PATTERN_NAME = ",\"name\":\"";
const char* PATTERN_EMAIL = ",\"email\":\"";
const char* PATTERN_FILES = ",\"files\":[{";

void freePost (Post* post);
Post* initPost (const char* post, const short postlen, const bool v);

int main (void) {
  setlocale (LC_ALL, "");

  char* thread = getThreadJSON ("abu", 42375, false);

  /*
  FILE* src = fopen ("thread.json", "r");
  fseek (src, 0, SEEK_END);
  const long src_size = ftell (src);
  fseek (src, 0, SEEK_SET);
  char* thread = (char*) calloc (sizeof(char), src_size);
  fread (thread, sizeof(char), src_size, src);
  fclose (src);
  */

  int postcount = 0;
  int* posts = findPostsInJSON (thread, &postcount, false);

  short one_post_len = strlen(thread)-posts[postcount-1];
  Post* one_post_struct = initPost (thread+posts[postcount-1],one_post_len,true);

  initscr();
  raw();
  keypad (stdscr, TRUE);
  noecho();
  printPost (one_post_struct,true,true);
  printw ("Push a key to exit");
  getch();
  endwin();
  freePost (one_post_struct);
  free (posts);
  free (thread);
  return 0;
}

Post* initPost (const char* post_string, const short postlen, const bool v) {
  fprintf (stderr, "]] Starting initPost");
  if (v) fprintf (stderr, " (verbose)"); fprintf (stderr, "\n");

  // Detecting comment:
  char* ptr_comment = strstr (post_string, PATTERN_COMMENT) + strlen (PATTERN_COMMENT);
  if (ptr_comment == NULL) {
    fprintf (stderr, "! Error: Bad post format: Comment pattern not found\n");
    return ERR_POST_FORMAT;
  }
  short comment_len = 0; bool stop = false;
  for (int i = ptr_comment-post_string; !stop && (i < postlen); i++) {
    if ((post_string[i] == '\"') && (post_string[i-1] != '\\')) {
      stop = true;
    }
    comment_len++;
  }
  comment_len--;
  if (comment_len == 0) {
    fprintf (stderr, "! Error: Bad post format: Null comment\n");
    return ERR_POST_FORMAT;
  }
  if (v) fprintf (stderr, "] Comment length: %d\n", comment_len);
  if (v) fprintf (stderr, "] Comment: \n");
  if (v) for (int i = 0; i < comment_len; i++)
    fprintf (stderr, "%c", ptr_comment[i]);
  if (v) fprintf (stderr, "\n] End of comment\n");

  // Detect date:

  char* ptr_date = strstr (ptr_comment+comment_len, PATTERN_DATE)+strlen(PATTERN_DATE);
  if (ptr_date == NULL) {
    fprintf (stderr, "! Error: Bad post format: Date pattern not found\n");
    return ERR_POST_FORMAT;
  }
  short date_len = strstr (ptr_date, "\"")-ptr_date;
  if (v) {
    fprintf (stderr, "] Date length: %d\n", date_len);
    fprintf (stderr, "] Date: ");
    for (int i = 0; i < date_len; i++)
      fprintf (stderr, "%c", ptr_date[i]);
    fprintf (stderr, "\n");

  }

  // Detect email:
  char* ptr_email = strstr (ptr_date+date_len, PATTERN_EMAIL)+strlen(PATTERN_EMAIL);
  if (ptr_email == NULL) {
    fprintf (stderr, "! Error: Bad post format: Email pattern not found\n");
    return ERR_POST_FORMAT;
  }
  short email_len = strstr (ptr_email, "\"")-ptr_email;
  if (v) if (email_len == 0) {
    fprintf (stderr, "] Email not specified\n");
  }
  else {
    fprintf (stderr, "] Email length: %d\n", email_len);
    fprintf (stderr, "] Email: ");
    for (int i = 0; i < email_len; i++)
      fprintf (stderr, "%c", ptr_email[i]);
    fprintf (stderr, "\n");
  }

  // Detect files:
  char* ptr_files = strstr (ptr_email+email_len, PATTERN_FILES)+strlen(PATTERN_FILES);
  short files_len = 0;
  // If NULL, simply no files in post
  if (ptr_files-strlen(PATTERN_FILES) == NULL) {
    if (v) fprintf (stderr, "] Files not specified\n");
  }
  else {
    files_len = strstr (ptr_files, "}]")-ptr_files;
    if (files_len == 0) {
      fprintf (stderr, "! Error: Bad post format: Files field specified but null\n");
      return ERR_POST_FORMAT;
    }
    else
      if (v) {
        fprintf(stderr, "] Files length: %d\n", files_len);
        fprintf (stderr, "] Files: \n");
        for (int i = 0; i < files_len; i++) {
          fprintf (stderr, "%c", ptr_files[i]);
        }
        fprintf (stderr, "\n] End of files\n");
      }
  }

  // Detect name:
  char* ptr_name = 0; char* ptr_name_diff = 0; short name_diff_len = 0;
  if (ptr_files-strlen(PATTERN_FILES) == NULL) {
    ptr_name_diff = ptr_email; // Files field may include "\"name\":" substring,
    name_diff_len = email_len; // so we must ignore the files field, if it exists.
  }                            // However, if it is not specified, we cannot use
  else {                       // ptr_files. So we split into 2 cases and use
    ptr_name_diff = ptr_files; // 2 variants of values for 2 new pointers.
    name_diff_len = files_len;
  }
  ptr_name = strstr (ptr_name_diff+name_diff_len, PATTERN_NAME)+strlen(PATTERN_NAME);
  if (ptr_name == NULL) {
    fprintf (stderr, "! Error: Bad post format: Name pattern not found\n");
    return ERR_POST_FORMAT;
  }
  short name_len = strstr (ptr_name, "\"")-ptr_name;
  if (v) fprintf (stderr, "] Name length: %d\n", name_len);
  if (v) fprintf (stderr, "] Name: ");
  if (v) for (int i = 0; i < name_len; i++)
    fprintf (stderr, "%c", ptr_name[i]);
  if (v) fprintf (stderr, "\n");

  if (v) fprintf (stderr, "] = All main fields detected\n");

  // Init struct:
  Post* post = (Post*) calloc (sizeof(Post),1);
  post->comment = (char*) calloc (sizeof(char), comment_len);
  post->date = (char*) calloc (sizeof(char), date_len);
  post->name = (char*) calloc (sizeof(char), name_len);
  if (email_len > 0) {
    post->email = (char*) calloc (sizeof(char), email_len);
  }
  else {
    post->email = 0;
  }
  if (files_len > 0) {
    post->files = (char*) calloc (sizeof(char), files_len);
  }
  else {
    post->files = 0;
  }

  if (post->comment == NULL) {
    fprintf (stderr, "! Error allocating memory (post.comment)\n");
    return ERR_MEMORY_LEAK;
  }
  if (post->date == NULL) {
    fprintf (stderr, "! Error allocating memory (post.date)\n");
    return ERR_MEMORY_LEAK;
  }
  if (post->name == NULL) {
    fprintf (stderr, "! Error allocating memory (post.name)\n");
    return ERR_MEMORY_LEAK;
  }
  if (post->email == NULL) {
    fprintf (stderr, "! Error allocating memory (post.email)\n");
    return ERR_MEMORY_LEAK;
  }
  if (post->files == NULL) {
    fprintf (stderr, "! Error allocating memory (post.files)\n");
    return ERR_MEMORY_LEAK;
  }
  if (v) fprintf (stderr, "] = Init struct done\n");
  fprintf (stderr, "]] Exiting initPost\n");
  return post;
}

void freePost (Post* post) {
  free (post->comment);
  free (post->date);
  free (post->name);
  free (post->email);
  free (post->files);
  free (post);
}

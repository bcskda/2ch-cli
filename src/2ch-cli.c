#include "makaba.h"
#include <ncurses.h>
#include <locale.h>

// ========================================
// File: 2ch-cli.c
// A CLI-client for 2ch.hk imageboard
// written on C
// ========================================

const char* PATTERN_COMMENT = ",\"comment\":\"";
const char* PATTERN_TIMESTAMP = ",\"timestamp\":\"";
const char* PATTERN_SUBJECT = ",\"subject\":\"";
const char* PATTERN_NAME = ",\"name\":\"";
const char* PATTERN_EMAIL = ",\"email\":\"";
const char* PATTERN_FILES = ",\"files\":[{\"";

int printPost (const char* post, const short postlen, const bool show_name,
 const bool show_email,const bool show_files, const bool v);
char* findPostPart (const char* post, const short offset, const short type);

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
  char* one_post = (char*) calloc (sizeof(char), one_post_len);
  one_post = memcpy (one_post, thread+posts[postcount-1], sizeof(char)*one_post_len);

  initscr();
  raw();
  keypad (stdscr, TRUE);
  noecho();
  printw ("Testing ncurses!\nPush a key..\n");
  refresh();
  getch();
  printw ("%s\n", one_post);
  getch();
  endwin();

  printPost (one_post,one_post_len,true,true,false,true);

  free (one_post);

  free (posts);
  free (thread);
  return 0;
}

int printPost (const char* post, const short postlen, const bool show_name,
 const bool show_email,const bool show_files, const bool v) {
  fprintf (stderr, "]] Starting printPost");
  if (v) fprintf (stderr, " (verbose)"); fprintf (stderr, "\n");
  const char* ptr_comment = strstr (post, PATTERN_COMMENT) + strlen (PATTERN_COMMENT);
  if (ptr_comment == NULL) {
    fprintf (stderr, "! Error: Bad post format: Comment pattern not found\n");
    return ERR_POST_FORMAT;
  }
  short commentlen = 0; bool stop = false;
  for (int i = ptr_comment-post; !stop && (i < postlen); i++) {
    if ((post[i] == '\"') && (post[i-1] != '\\')) {
      stop = true;
    }
    commentlen++;
  }
  commentlen--;
  if (commentlen == 0) {
    fprintf (stderr, "! Error: Bad post format: Null comment\n");
    return ERR_POST_FORMAT;
  }

  if (v) fprintf (stderr, "] Comment length: %d\n", commentlen);
  if (v) fprintf (stderr, "] Comment: \n");
  if (v) for (int i = 0; i < commentlen; i++) {
    fprintf (stderr, "%c", post[ptr_comment-post+i]);
  }
  if (v) fprintf (stderr, "\n] End of comment\n");

  char* ptr_name, ptr_email, ptr_files = 0;

  if (show_name) {
    ptr_name = strstr (ptr_comment+commentlen, PATTERN_NAME)+strlen(PATTERN_NAME);
    if (ptr_name == NULL) {
      fprintf (stderr, "! Error: Bad post format: Name pattern not found\n");
      return ERR_POST_FORMAT;
    }
  }

  // @TODO Get {name, email and file}-field lengths

  if (show_email) {
    ptr_email = strstr (ptr_comment+commentlen, PATTERN_EMAIL)+strlen(PATTERN_EMAIL);
    if (ptr_email == NULL) {
      fprintf (stderr, "! Error: Bad post format: Email pattern not found\n");
      return ERR_POST_FORMAT;
    }
  }

  if (show_files) {
    ptr_files = strstr (ptr_comment+commentlen, PATTERN_FILES)+strlen(PATTERN_FILES);
    // If NULL, simply no files in post
  }

  fprintf (stderr, "]] Exiting printPost\n");
  return 0;
}

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
const char* PATTERN_MAIL = ",\"email\":\"";
const char* PATTERN_FILES = ",\"files\":[{\"";

int printPost (const char* post, const short postlen, const bool show_name,
 const bool show_mail,const bool show_files, const bool v);

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

  printPost (one_post,one_post_len,false,false,false,true);

  free (one_post);

  free (posts);
  free (thread);
  return 0;
}

int printPost (const char* post, const short postlen, const bool show_name,
 const bool show_mail,const bool show_files, const bool v) {
  fprintf (stderr, "]] Starting printPost");
  if (v) fprintf (stderr, " (verbose)"); fprintf (stderr, "\n");
  const char* ptr_comment = strstr (post, PATTERN_COMMENT) + strlen (PATTERN_COMMENT);
  short commentlen = 0; bool stop = false;
  for (int i = ptr_comment-post; !stop && (i < postlen); i++) {
    if ((post[i] == '\"') && (post[i-1] != '\\')) {
      stop = true;
    }
    commentlen++;
  }
  commentlen--;

  if (v) fprintf (stderr, "] Comment length: %d\n", commentlen);
  if (v) fprintf (stderr, "] Comment: \n");
  if (v) for (int i = 0; i < commentlen; i++) {
    fprintf (stderr, "%c", post[ptr_comment-post+i]);
  }
  if (v) fprintf (stderr, "\n] End of comment\n");

  // @TODO Other fields

  fprintf (stderr, "]] Exiting printPost\n");
  return 0;
}

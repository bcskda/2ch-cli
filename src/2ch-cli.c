#include "makaba.h"
#include "ncurses.h"
#include "locale.h"

// ========================================
// File: 2ch-cli.c
// A CLI-client for 2ch.hk imageboard
// written on C
// ========================================

int main (void) {
  setlocale (LC_ALL, "");

  char* thread = getThreadJSON ("abu",42375,false);
  int postcount = 0;
  int* posts = findPostsInJSON (thread, &postcount);

  initscr ();
  printw ("Testing ncurses!\nPush a key..");
  refresh ();
  getch ();

  short one_post_len = posts[7]-posts[6];
  char* one_post = (char*) calloc (sizeof(char), one_post_len);
  one_post = memcpy (one_post, thread+posts[6], sizeof(char)*one_post_len);
  printw ("%s", one_post);
  refresh ();
  getch ();
  endwin ();

  fprintf (stderr, "%s\n", one_post);
  free (one_post);

  free (posts);
  free (thread);
  return 0;
}

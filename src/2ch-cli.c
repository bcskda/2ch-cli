#include "makaba.h"
#include <ncurses.h>
#include <locale.h>

// ========================================
// File: 2ch-cli.c
// A CLI-client for 2ch.hk imageboard
// written on C
// ========================================

int main (void) {
  setlocale (LC_ALL, "");

  char* thread = getThreadJSON ("abu", 42375, false);
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

  free (one_post);

  free (posts);
  free (thread);
  return 0;
}

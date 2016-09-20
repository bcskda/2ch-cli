#include "makaba.h"
#include "ncurses.h"

// ========================================
// File: 2ch-cli.c
// A CLI-client for 2ch.hk imageboard
// written on C
// ========================================

int main (void) {
  //getBoardCatalogJSON ("b",true);
  //printf ("\n\n\n");
  char* thread = getThreadJSON ("abu",42375,false);
  int postcount = 0;
  int* posts = findPostsInJSON (thread, &postcount);
  for (int i = 0; i < postcount-1; i+=1) {
    printf ("%2d ] ", i);
    int tmpsize = posts[i+1]-posts[i]-1;
    char* tmp = (char*) calloc (sizeof(char),tmpsize);
    tmp = memcpy (tmp, thread+posts[i], tmpsize*sizeof(char));
    printf ("%s\n",tmp);
    free (tmp);
  }
  printf ("%2d ] ", postcount-1);
  int tmpsize = strlen(thread)-posts[postcount-1]-1;
  char* tmp = (char*) calloc (sizeof(char),tmpsize);
  tmp = memcpy (tmp, thread+posts[postcount-1], tmpsize*sizeof(char));
  printf ("%s\n",tmp);
  free (tmp);
  free (posts);
  free (thread);
  return 0;
}

#include "makaba.h"

// ========================================
// A cli-client for 2ch.hk imageboard
// written on C with ncurses
// ========================================

int main (void) {
  getBoardPageJSON ("b",1,true);
  return 0;
}

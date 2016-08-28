#include "makaba.h"

// ========================================
// File: 2ch-cli.c
// A CLI-client for 2ch.hk imageboard
// written on C
// ========================================

int main (void) {
  getBoardCatalogJSON ("b",true);
  printf ("\n\n\n");
  getThreadJSON ("abu",42375,true);
  return 0;
}

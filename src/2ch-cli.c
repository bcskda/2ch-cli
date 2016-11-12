// ========================================
// File: 2ch-cli.c
// A CLI-client for 2ch.hk imageboard written on C
// (Implementation)
// ========================================

#include "2ch-cli.h"

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

	struct post** thread_parsed = (struct post**) calloc (postcount-1, sizeof(struct post*));
	short clen = posts[1]-posts[0];
	
	fprintf(stderr, "[0] = %d\n", posts[0]);
	thread_parsed[0] = initPost(thread+posts[0],clen,true);
	fprintf(stderr, "[!] Back in main after init (first)\n");
	
	for (int i = 1; i < postcount-1; i++) {
		fprintf(stderr, "[%d] = %d\n", i, posts[i]);
		clen = posts[i]-posts[i-1];
		thread_parsed[i] = initPost(thread+posts[i],clen,true);
		fprintf(stderr, "[!] Back in main after init #%d\n", i);
	}
	clen = strlen(thread)-posts[postcount-1];
	thread_parsed[postcount-1] = initPost(thread+posts[postcount-1],clen,true);
	fprintf(stderr, "[!] Back in main after init (last)\n");
	/*
	fprintf(stderr, "Comment.text = %s\n", one_post_struct->comment->text);
	fprintf(stderr, "Name = %s\n", one_post_struct->name);
	if (one_post_struct->email != -1) fprintf(stderr, "Email = %s\n", one_post_struct->email);
	fprintf(stderr, "Date = %s\n", one_post_struct->date);
	*/
	
	initscr();
	raw();
	keypad (stdscr, TRUE);
	noecho();
	printw ("Push [c] to clear screen, anything else to print another post\n");
	for (int i = 0; i < postcount-1; i++) {
		bool done = 0;
		while (! done)
			switch (getch()) { 
				case 'C': case 'c':
					fprintf(stderr, "Clearing screen at #&d\n", i);
					clear();
					printw ("Push [c] to clear screen, anything else to print another post\n");
					break;
				default:
					fprintf(stderr, "Printing post #%d\n", i);
					printPost (thread_parsed[i],true,true);
					refresh();
					done = 1;
					break;
			}
	}
	printw ("\nPush a key to exit\n");
	getch();
	endwin();
	
	for (int i = 1; i < postcount; i++) {
		printPost (thread_parsed[i], true, true);
		fprintf(stderr, "[!] Back in main after printing #%d\n", i);
	}
	
	for (int i = 1; i < postcount; i++) {
		freePost (thread_parsed[i]);
	}
	free (thread_parsed);
	free (posts);
	free (thread);
	return 0;
}


int printPost (struct post* post,const bool show_email,const bool show_files) {
	if (show_email && (post->email != NULL)) {
		printw ("[=== %s (%s) #%d %s ===]\n%s\n",
			post->name, post->email, post->num, post->date, post->comment->text);
	} else {
		printw ("[=== %s #%d %s ===]\n%s\n",
			post->name, post->num, post->date, post->comment->text);
	}
	return 0;
}

#include "makaba.h"
#include <ncurses.h>
#include <locale.h>

// ========================================
// File: 2ch-cli.c
// A CLI-client for 2ch.hk imageboard
// written on C
// ========================================
// TODO:
//[x] freeRefReply
//[x] freeComment
//[x] parseRef_Reply
//[x] init struct comment in commentParse
//[x] init comment in initPost
//[x] FREEING struct ref_reply AFTER INIT @ parseComment()
// ========================================

struct list {
	void* data;
	struct list* first;
	struct list* next;
};

struct ref_reply {
	char* link;
	unsigned thread;
	unsigned num;
};

struct comment {
	char* text;
	struct ref_reply* refs;
	unsigned nrefs;
};

struct post {
	struct comment* comment;
	char* date;
	char* name;
	char* email;
	char* files;
};

const char* PATTERN_COMMENT = ",\"comment\":\"";
const char* PATTERN_DATE = ",\"date\":\"";
const char* PATTERN_SUBJECT = ",\"subject\":\"";
const char* PATTERN_NAME = ",\"name\":\"";
const char* PATTERN_EMAIL = ",\"email\":\"";
const char* PATTERN_FILES = ",\"files\":[{";

const char* PATTERN_HREF_OPEN = "\\u003ca href=\\\"";
const char* PATTERN_HREF_CLOSE = "\\u003c/a\\u003e";
const char* PATTERN_REPLY_CLASS = "class=\\\"post-reply-link\\\"";
const char* PATTERN_REPLY_THREAD = "data-thread=\\\"";
const char* PATTERN_REPLY_NUM = "data-num=\\\"";
const char* PATTERN_GREEN = "span class=\\\"unkfunc\\\"";
const char* PATTERN_NEWLINE = "\\u003cbr\\u003e";


struct post* initPost (const char* post_string, const short postlen, const bool v);
struct ref_reply* parseRef_Reply (const char* ch_ref, const int ref_len, const bool v);
struct comment* parseComment (char* comment, const bool v);
char* cleanupComment (const char* src, const unsigned src_len, const bool v);

int printPost (struct post* post,const bool show_email,const bool show_files);

void freeRefReply (struct ref_reply* ref);
void freePost (struct post* post);
void freeComment (struct comment* arg);

int main (void) {
	setlocale (LC_ALL, "");

	//char* thread = getThreadJSON ("abu", 42375, false);

	///*
	FILE* src = fopen ("thread.json", "r");
	fseek (src, 0, SEEK_END);
	const long src_size = ftell (src);
	fseek (src, 0, SEEK_SET);
	char* thread = (char*) calloc (sizeof(char), src_size);
	fread (thread, sizeof(char), src_size, src);
	fclose (src);
	//*/
	
	int postcount = 0;
	int* posts = findPostsInJSON (thread, &postcount, false);

	struct post** thread_parsed = (struct post**) calloc (postcount-1, sizeof(struct post*));
	short clen = posts[0];
	
	//thread_parsed[0] = initPost(thread+posts[0],clen,true);
	//fprintf(stderr, "[!] Back in main after init (first)\n");
	
	for (int i = 1; i < postcount-1; i++) {
		fprintf(stderr, "[%d] = %d; \n", i, posts[i]);
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
	for (int i = 1; i < postcount-1; i++) {
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

struct post* initPost (const char* post_string, const short postlen, const bool v) {
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
	} else {
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
	} else {
		files_len = strstr (ptr_files, "}]")-ptr_files;
		if (files_len == 0) {
			fprintf (stderr, "! Error: Bad post format: Files field specified but null\n");
			return ERR_POST_FORMAT;
		} else
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
	} else {                     // However, if it is not specified, we cannot use
		ptr_name_diff = ptr_files; // ptr_files. So we split into 2 cases and use
		name_diff_len = files_len; // 2 variants of values for 2 new pointers.
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
	struct post* post = (struct post*) calloc (1,sizeof(struct post));

	char* comment_str = (char*) calloc (comment_len, sizeof(char));
	memcpy(comment_str, ptr_comment, comment_len);

	post->comment = parseComment (comment_str, true);
	free (comment_str);
	fprintf (stderr, "!! post.comment.nrefs = %d\n", post->comment->nrefs);
	fprintf (stderr, "!! post.comment.refs[0].link = %s\n", post->comment->refs[0].link);
	fprintf (stderr, "!! post.comment.text = %s\n", post->comment->text);

	post->date = (char*) calloc (sizeof(char), date_len+1);
	memcpy (post->date, ptr_date, sizeof(char)*date_len);
	post->name = (char*) calloc (sizeof(char), name_len+1);
	memcpy (post->name, ptr_name, sizeof(char)*name_len);
	if (email_len == 0) {
		post->email = -1;
	} else {
		post->email = (char*) calloc (sizeof(char), email_len+1);
		memcpy (post->email, ptr_email, sizeof(char)*email_len);
	}
	if (files_len == 0) {
		post->files = -1;
	} else {
		post->files = (char*) calloc (sizeof(char), files_len+1);
		memcpy (post->files, ptr_files, sizeof(char)*files_len);
	}

	// Check struct:
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

int printPost (struct post* post,const bool show_email,const bool show_files) {
	if (show_email && (post->email != -1)) {
		printw ("[=== %s (%s)   %s ===]\n%s\n", post->name, post->email, post->date, post->comment->text);
	} else {
		printw ("[=== %s   %s ===]\n%s\n", post->name, post->date, post->comment->text);
	}
	return 0;
}

struct comment* parseComment (char* comment, const bool v) {
	fprintf (stderr, "]] Started parseComment");
	if (v) fprintf (stderr, " (verbose)");
	fprintf (stderr, "\n");
	unsigned comment_len = strlen (comment);
	int nrefs = 0;
	char* cinst = strstr(comment,PATTERN_HREF_OPEN);
	size_t pos = 0;
	struct list* refs = calloc (1, sizeof(struct list));
	refs->first = refs;
	refs->data = 0;
	refs->next = 0;
	struct ref_reply* cref = 0;
	unsigned clean_len = 0;
	char* clean_comment = (char*) calloc (comment_len, sizeof(char));
	for ( ; cinst != NULL; cinst = strstr(pos+comment,PATTERN_HREF_OPEN)) {
		if (v) fprintf (stderr, "]]] New ref\n]]]] Opened @ [%d]\n", cinst-comment);
		pos = cinst-comment;
		char* cinst_end = strstr (cinst, PATTERN_HREF_CLOSE);
		if (cinst_end == NULL) {
			fprintf (stderr, "! Error: ref opened but not closed\n");
			fprintf (stderr, "-- Comment: \"%s\"\n", comment);
			return ERR_COMMENT_FORMAT;
		}
			if (v) fprintf (stderr, "]]]] Closed @ [%d]\n", cinst_end-comment);
			char* class = strstr(cinst, PATTERN_REPLY_CLASS);
			if (class != NULL) {
				if (v) fprintf (stderr, "]]]] Type: reply\n");
				nrefs++;
				cref = parseRef_Reply (cinst, cinst_end-cinst, true);
				memcpy (clean_comment+clean_len, ">>", sizeof(char)*2);
				clean_len += 2;
				short csize = strlen (unsigned2str(cref->num));
				memcpy (clean_comment+clean_len, unsigned2str(cref->num), sizeof(char)*csize);
				clean_len += csize;
				if (v) fprintf (stderr, "]]]] Init done:\n");
				if (v) fprintf (stderr, "]]]]] link=\"%s\"\n]]]]] thread=%d\n]]]]] num=%d\n",
												cref->link, cref->thread, cref->num);
				if (v) fprintf(stderr, "Adding new ref #%d\n", nrefs);
				if (nrefs > 1) {
					refs->next = (struct list*) calloc (1, sizeof(struct list));
					refs->next->first = refs->first;
					refs->next->next = 0;
					refs->next->data = calloc (1, sizeof(struct ref_reply));
					memcpy (refs->next->data, (void*) cref, sizeof(struct ref_reply));
					refs = refs->next;
					if (v) fprintf(stderr, "List member state: 1st = %p, cur = %p, next = %p\n", refs->first, refs, refs->next);
				} else {
					refs->next = 0;
					refs->data = calloc (1, sizeof(struct ref_reply));
					memcpy (refs->data, (void*) cref, sizeof(struct ref_reply));
					if (v) fprintf(stderr, "Filling 1st list member\n");
 					if (v) fprintf(stderr, "List member state: 1st = %p, cur = %p, next = %p\n", refs->first, refs, refs->next);
				}
			} else {
				fprintf (stderr, "]]]] Unknown ref type, treat as text\n");
			}
			short csize = cinst-(comment+pos);
			fprintf(stderr, "[!] From #%d: %s\n", pos, comment+pos);
			fprintf(stderr, "[!] To: %p+%d\n", clean_comment, clean_len);
			fprintf(stderr, "[!] Size: %d\n", csize);
			if (csize > 0) {
				memcpy (clean_comment+clean_len, comment+pos, sizeof(char)*csize);
				clean_len += csize;
			}
			pos = cinst_end-comment+strlen(PATTERN_HREF_CLOSE);
			free (cref);
	}
	short csize = comment_len-pos;
	fprintf(stderr, "[!] From #%d: %s\n", pos, comment+pos);
	fprintf(stderr, "[!] To: %p+%d\n", clean_comment, clean_len);
	fprintf(stderr, "[!] Size: %d\n", csize);
	if (csize > 0) {
		memcpy (clean_comment+clean_len, comment+pos, sizeof(char)*csize);
		clean_len += csize;
	}
	char* finally_clean_comment = cleanupComment (clean_comment, clean_len, true);
	if (v) fprintf(stderr, "]]] Total: %d refs\n", nrefs);
	struct comment* parsed = (struct comment*) calloc (1, sizeof(struct comment));
	parsed->text = (char*) calloc (comment_len, sizeof(char));
	memcpy (parsed->text, finally_clean_comment, sizeof(char)*strlen(finally_clean_comment));
	free (clean_comment);
	fprintf(stderr, "text ok: %s\n", parsed->text);
	parsed->nrefs = nrefs;
	fprintf(stderr, "nrefs ok: %d\n", parsed->nrefs);
	parsed->refs = (struct ref_reply*) calloc (nrefs, sizeof(struct ref_reply));
	refs = refs->first;
	for (int i = 0; i < nrefs; i++) {
		if (v) fprintf(stderr, "Copy ref #%d, first = %p, current = %p, next = %p\n", i+1, refs->first, refs, refs->next);
		memcpy (parsed->refs+i*sizeof(struct ref_reply), (void*) refs->data, sizeof(struct ref_reply));
	}
	fprintf (stderr, "]] Exiting parseComment\n");
	return parsed;
}

struct ref_reply* parseRef_Reply (const char* ch_ref, const int ref_len, const bool v) {
	if (v) fprintf (stderr, "-]] Started parseRef_Reply (verbose)\n");

	char* link_start = ch_ref + strlen (PATTERN_HREF_OPEN);
	short link_len = strstr (ch_ref, PATTERN_REPLY_CLASS) - 3 - link_start;
													// '-3' to exclude JSON 

	char* data_thread_start = strstr (ch_ref, PATTERN_REPLY_THREAD) + strlen(PATTERN_REPLY_THREAD);
	if (data_thread_start == NULL) { // '+ strlen' to exclude opening string
		fprintf (stderr, "! Error: no data-thread in reply-ref\n");
		return ERR_REF_FORMAT;
	}
	char* data_thread_end = strstr (data_thread_start, "\\\"") - 1;
	if (data_thread_end == NULL-1) {  // '- 1' to exclude '\'
		fprintf (stderr, "! Error: data-thread opened but not closed\n");
		return ERR_REF_FORMAT;
	}
	unsigned data_thread = str2unsigned (data_thread_start, data_thread_end-data_thread_start+1);
	if (v) fprintf (stderr, "-]]] Thread: %d\n", data_thread);

	char* data_num_start = strstr (ch_ref, PATTERN_REPLY_NUM) + strlen(PATTERN_REPLY_NUM);
	if (data_num_start == NULL) { // '+ strlen' to exclude opening string
		fprintf (stderr, "! Error: no data-num in reply-ref\n");
		return ERR_REF_FORMAT;
	}
	char* data_num_end = strstr (data_num_start, "\\\"") - 1;
	if (data_thread_end == NULL-1) {  // '- 1' to exclude '\'
		fprintf (stderr, "! Error: data-num opened but not closed\n");
		return ERR_REF_FORMAT;
	}
	unsigned data_num = str2unsigned (data_num_start, data_num_end-data_num_start+1);
	if (v) fprintf (stderr, "-]]] Postnum: %d\n", data_num);

	struct ref_reply* ref_parsed = (struct ref_reply*) calloc (1, sizeof(struct ref_reply));
	ref_parsed->link = (char*) calloc (link_len, sizeof(char) + 1);
	memcpy (ref_parsed->link, link_start, link_len*sizeof(char));
	ref_parsed->thread = data_thread;
	ref_parsed->num = data_num;

	fprintf (stderr, "-]] Exiting parseRef_Reply\n");

	return ref_parsed;
}

char* cleanupComment (const char* src, const unsigned src_len, const bool v) {
	fprintf(stderr, "]] Started cleanupComment\n");
	char* buf = (char*) calloc (src_len, sizeof(char));
	unsigned pos = 0, len = 0;
	for ( ; pos < src_len; ) {
		if ((src[pos] == '\\')   && (src[pos+1] == 'u') && (src[pos+2] == '0') && (src[pos+3] == '0')
			&& (src[pos+4] == '3') && (src[pos+5] == 'c') && (src[pos+6] == 'b') && (src[pos+7] == 'r')) {
			pos += 14;
			buf[len] = '\n';
			len++;
		} else if ((src[pos   ] == '\\') && (src[pos+1 ] == 'u' ) && (src[pos+2 ] == '0' ) && (src[pos+3 ] == '0')
				    && (src[pos+4 ] == '3' ) && (src[pos+5 ] == 'c' ) && (src[pos+6 ] == 's' ) && (src[pos+7 ] == 'p')
					  && (src[pos+8 ] == 'a' ) && (src[pos+9 ] == 'n' ) && (src[pos+10] == ' ' ) && (src[pos+11] == 'c')
					  && (src[pos+12] == 'l' ) && (src[pos+13] == 'a' ) && (src[pos+14] == 's' ) && (src[pos+15] == 's')
					  && (src[pos+16] == '=' ) && (src[pos+17] == '\\') && (src[pos+18] == '"' ) && (src[pos+19] == 'u')
					  && (src[pos+20] == 'n' ) && (src[pos+21] == 'k' ) && (src[pos+22] == 'f' ) && (src[pos+23] == 'u')
					  && (src[pos+24] == 'n' ) && (src[pos+25] == 'c' ) && (src[pos+26] == '\\') && (src[pos+27] == '"')) {
				pos += 38;
				// Color: green
		} else if ((src[pos   ] == '\\') && (src[pos+1 ] == 'u' ) && (src[pos+2 ] == '0' ) && (src[pos+3 ] == '0')
				    && (src[pos+4 ] == '3' ) && (src[pos+5 ] == 'c' ) && (src[pos+6 ] == '/' ) && (src[pos+7 ] == 's')
					  && (src[pos+8 ] == 'p' ) && (src[pos+9 ] == 'a' ) && (src[pos+10] == 'n' )) {
				pos += 17;
		} else {
			buf[len] = src[pos];
			len++;
			pos++;
		}
	}

	char* clean = (char*) calloc (len, sizeof(char));
	memcpy(clean, buf, sizeof(char)*len);
	free (buf);
	fprintf(stderr, "]] Exiting cleanupComment\n");
	return clean;
}

void freeRefReply (struct ref_reply* ref) {
	free (ref->link);
}

void freeComment (struct comment* arg) {
	free (arg->text);
	free (arg->refs);
}

void freePost (struct post* post) {
	freeComment (post->comment);
	free (post->date);
	free (post->name);
	if (post->email != -1)
		free (post->email);
	if (post->files != -1)
		free (post->files);
	free (post);
}
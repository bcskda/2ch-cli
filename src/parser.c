// ========================================
// File: parser.c
// API answer parsing functions
// (Implementation)
// ========================================

#pragma once
#include "parser.h"

long int* findPostsInJSON (const char* src, long int* postcount_res, const bool v) {
	fprintf (stderr, "]] Starting findPostsInJSON");
	FILE* LOCAL_LOG = NULL;

	if (v) fprintf (stderr, " (verbose, log in ./log/findPostsInJSON)"); fprintf (stderr, "\n");
	if (v) LOCAL_LOG = fopen("log/findPostsInJSON.log", "a");
	if (v) fprintf(LOCAL_LOG, "\n\n<< New Thread >>\n");

	int srclen = strlen (src);
	if (v) fprintf(LOCAL_LOG, "srclen = %d\n", srclen);
	long int* temp = (long int*) calloc (srclen/8, sizeof(long int));

	short depth = 0;
	int postcount = 0;
	bool comment_read = false;

	for (int i = 1; i < srclen; i+=1) {
		if (v) fprintf(LOCAL_LOG, "%c", src[i]);
		switch (depth) {
			case 5: {
				if (src[i] == '"' && src[i-1 != '\\']) {
					depth -= 1;
				}
				break;
			}
			case 4: {
				if (src[i] == '"' && src[i-1 != '\\']) {
					depth -= 1;
				}
				break;
			}
			case 3: {
				if (src[i] == '"' && src[i-1 != '\\']) {
					depth -= 1; // Выход из displayname/fullname
					if (v) fprintf (LOCAL_LOG, "Exiting name\n");
				}
				break;
			}
			case 2: {
				if (comment_read) { // in .post.files
					if ((src[i-2]=='"')&&(src[i-1]=='d')&&(src[i]=='i')&&
						(src[i+1]=='s')&&(src[i+2]=='p')) {
							depth += 3; // Вход в displayname
							if (v) fprintf (LOCAL_LOG, "Entering displayname\n");
							break;
						}
					if ((src[i-2]=='"')&&(src[i-1]=='f')&&(src[i]=='u')&&
						(src[i+1]=='l')&&(src[i+2]=='l')) {
							depth += 3; // Вход в fullname
							if (v) fprintf (LOCAL_LOG, "Entering fullname\n");
							break;
					}
					if (src[i] == ']') {
						if (v) fprintf (LOCAL_LOG, "Exiting files. ");
						depth -= 1;
						break;
					}
				}
				else { // in: .post.comment
					if ((src[i-1] != '\\') && (src[i] == '"')) {
						if (v) fprintf (LOCAL_LOG, "Exiting comment. ");
						comment_read = true;
						depth -= 1;
					}
				}
				break;
			}
			case 1: { // in: .post
				if (src[i] == '}') {
					if (v) fprintf (LOCAL_LOG, "Exiting post (@%d).\n", i);
					postcount += 1;
					depth -= 1;
					comment_read = false;
				}
				if ((src[i-2]=='"')&&(src[i-1]=='f')&&(src[i]=='i')&&
					(src[i+1]=='l')&&(src[i+2]=='e')) {
					if (v) fprintf (LOCAL_LOG, "Entering files. ");
					depth += 1;
				}
				if ((src[i-2]=='"')&&(src[i-1]=='c')&&(src[i]=='o')&&
					(src[i+1]=='m')&&(src[i+2]=='m')) {
					if (v) fprintf (LOCAL_LOG, "Entering comment. ");
					depth += 1;
				}
				continue;
			}
			case 0: { // in: .
				if (src[i] == '{') {
					if (v) fprintf (LOCAL_LOG, "Entering post #%d ", postcount+1);
					temp[postcount] = i;
					if (v) fprintf (LOCAL_LOG, "(@%d)\n", temp[postcount]);
					depth += 1;
				}
				continue;
			}
			default: {
				fprintf (stderr, "! Error: Unusual depth (%d)\n", depth);
				free (temp);
				if (v) fclose (LOCAL_LOG);
				return ERR_PARTTHREAD_DEPTH;
			}
		}
	}
	if (depth != 0) {
		fprintf (stderr, "! Error: Non-zero depth (%d) after cycle\n", depth);
		if (v) fclose (LOCAL_LOG);
		return ERR_PARTTHREAD_DEPTH;
	}

	if (v) fprintf (LOCAL_LOG, "%d posts found\n", postcount);

	long int* posts = (long int*) calloc (postcount, sizeof(long int));
	posts = memcpy (posts, temp, postcount*sizeof(long int));
	free (temp);
	if (v) fprintf (LOCAL_LOG, "Posts begin at:\n");
	if (v) for (int i = 0; i < postcount; i++)
		fprintf (LOCAL_LOG, "] %2d: %5d\n", i, posts[i]);

	*postcount_res = postcount;

	if (v) fprintf(LOCAL_LOG, "<< End of Thread >>\n");
	if (v) fclose(LOCAL_LOG);
	fprintf(stderr, "]] Exiting findPostsInJSON\n");
	return posts;
}

struct post* initPost (const char* post_string, const long int postlen, const bool v) {
	fprintf(stderr, "]] Starting initPost");
	FILE* LOCAL_LOG = NULL;
	if (v) LOCAL_LOG = fopen("log/initPost.log", "a");
	if (v) fprintf(stderr, " (verbose, log in log/initPost.log)"); fprintf (stderr, "\n");
	if (v) fprintf(LOCAL_LOG, "\n\n<< New Thread >>\n]] Args:\n== | post_string = ");
	if (v) for (int i = 0; i < postlen; i++)
		fprintf(LOCAL_LOG, "%c", post_string[i]);
	if (v) fprintf(LOCAL_LOG, "\n| postlen = %d\n", postlen);
	if (v) fflush(LOCAL_LOG);

	// Detecting comment:
	char* ptr_comment = strstr (post_string, PATTERN_COMMENT);
	if (ptr_comment == NULL) {
		fprintf (stderr, "! Error: Bad post format: Comment pattern not found\n");
		return ERR_POST_FORMAT;
	}
	ptr_comment += strlen(PATTERN_COMMENT);

	char* ptr_comment_end = strstr (ptr_comment, PATTERN_COMMENT_END);
	int comment_len = (int) (ptr_comment_end - ptr_comment);
	if (v) fprintf (LOCAL_LOG, "comment_len = %d\n=== Comment: ===\n", comment_len);
	if (v) for (int i = 0; i < comment_len; i++)
		fprintf (LOCAL_LOG, "%c", ptr_comment[i]);
	if (v) fprintf (LOCAL_LOG, "\n== End of comment ==\n");
	fflush(LOCAL_LOG);

	// Detect date:
	char* ptr_date = strstr (ptr_comment+comment_len, PATTERN_DATE)+strlen(PATTERN_DATE);
	if (ptr_date == NULL) {
		fprintf (stderr, "! Error: Bad post format: Date pattern not found\n");
		return ERR_POST_FORMAT;
	}
	int date_len = strstr (ptr_date, "\"") - ptr_date;
	if (v) {
		fprintf (LOCAL_LOG, "] Date length: %d\n", date_len);
		fprintf (LOCAL_LOG, "] Date: ");
		for (int i = 0; i < date_len; i++)
			fprintf (LOCAL_LOG, "%c", ptr_date[i]);
		fprintf (LOCAL_LOG, "\n");
	}
	fflush(LOCAL_LOG);

	// Detect email:
	char* ptr_email = strstr(ptr_date + (ptrdiff_t)date_len, PATTERN_EMAIL);
	if (ptr_email == NULL) {
		fprintf (stderr, "! Error: Bad post format: Email pattern not found\n");
		return ERR_POST_FORMAT;
	}
	ptr_email += (ptrdiff_t) strlen(PATTERN_EMAIL);
	int email_len = (int) (strstr (ptr_email, "\"") - ptr_email);
	if (v) {
		if (email_len == 0) {
			fprintf (LOCAL_LOG, "] Email not specified\n");
		} else {
			fprintf (LOCAL_LOG, "] Email length: %d\n", email_len);
			fprintf (LOCAL_LOG, "] Email: ");
			for (int i = 0; i < email_len; i++)
				fprintf (LOCAL_LOG, "%c", ptr_email[i]);
			fprintf (LOCAL_LOG, "\n");
		}
	}
	fflush(LOCAL_LOG);

	// Detect files:
	char* ptr_files = strstr(ptr_email + (ptrdiff_t)email_len, PATTERN_FILES);
	int files_len = 0;
	bool has_files = false;
	// If NULL, simply no files in post
	if ( (ptr_files == NULL) || ((long int) (ptr_files - post_string) >= postlen) ) {
		if (v) fprintf (LOCAL_LOG, "] Files not specified\n");
	} else {
		ptr_files += strlen(PATTERN_FILES);
		has_files = true;
		files_len = strstr(ptr_files, "}]") - ptr_files;
		if (files_len == 0) {
			fprintf (stderr, "! Error: Bad post format: Files field specified but null\n");
			return ERR_POST_FORMAT;
		} else
			if (v) {
				fprintf(LOCAL_LOG, "] Files length: %d\n", files_len);
				fprintf (LOCAL_LOG, "] Files: \n");
				for (int i = 0; i < files_len; i++) {
					fprintf (LOCAL_LOG, "%c", ptr_files[i]);
				}
				fprintf (LOCAL_LOG, "\n] End of files\n");
			}
	}
	fflush(LOCAL_LOG);

	// Detect name:
	char *ptr_name_diff = 0; int name_diff_len = 0;
	if (has_files) {
		ptr_name_diff = ptr_files; // ptr_files. So we split into 2 cases and use
		name_diff_len = files_len; // 2 variants of values for 2 new pointers.
	} else {                       // However, if it is not specified, we cannot use
		ptr_name_diff = ptr_email; // Files field may include "\"name\":" substring,
		name_diff_len = email_len; // so we must ignore the files field, if it exists.
	}
	char *ptr_name = strstr( ptr_name_diff + (ptrdiff_t)name_diff_len, PATTERN_NAME );
	if (ptr_name == NULL) {
		if (v) {
			fprintf(LOCAL_LOG, "Error - no name found\n");
			fclose(LOCAL_LOG);
		}
		fprintf (stderr, "! Error: Bad post format: Name pattern not found\n");
		return (char *) ERR_POST_FORMAT;
	}
	ptr_name += (ptrdiff_t)strlen(PATTERN_NAME);

	char* ptr_name_end = strstr(ptr_name, PATTERN_NAME_END);
	int name_len = (int) (ptr_name_end - ptr_name);
	if (v) fprintf (LOCAL_LOG, "] Name length: %d\n", name_len);
	if (v) fprintf (LOCAL_LOG, "] Name: ");
	if (v) for (int i = 0; i < name_len; i++)
		fprintf (LOCAL_LOG, "%c", ptr_name[i]);
	if (v) fprintf (LOCAL_LOG, "[name end]\n");
	fflush(LOCAL_LOG);

	// Detect postnum
	if ((long int) (ptr_name_end - post_string) >= postlen) {
		fprintf(stderr, "! Error: Out of post range\n");
		fprintf(stderr, "! Given as argument: %ld, locally calculated: %ld\n",
			postlen, (long int) (ptr_name_end - post_string));
		return (char *) ERR_POST_OUT_OF_RANGE;
	}
	char *ptr_num = strstr( ptr_name + (ptrdiff_t)name_len, PATTERN_NUM );
	if (ptr_num == NULL) {
		fprintf (stderr, "! Error: Bad post format: Num pattern not found\n");
		return (char *) ERR_POST_FORMAT;
	}
	if (v) fprintf (LOCAL_LOG, "[ptr_num] %c%c%c%c%c\n", ptr_num[0],ptr_num[1],ptr_num[2],ptr_num[3],ptr_num[4]);
	ptr_num += (ptrdiff_t) strlen(PATTERN_NUM);
	char *num_end =  strstr (ptr_num, ",");
	if (num_end == NULL) {
		fprintf(stderr, "! Error: Something strange with `num' field\n");
		return (char *) ERR_INTERNAL;
	}
	int num_len = (int) (num_end - ptr_num - 1); // Exclude ending '"'
	if (v) {
		fprintf (LOCAL_LOG, "] Num length: %d\n", num_len);
		fprintf (LOCAL_LOG, "] Num: |");
		for (int i = 0; i < num_len; i++) {
			fprintf (LOCAL_LOG, "%c", ptr_num[i]);
			fprintf (stderr,    "%c", ptr_num[i]);
		}
		fprintf (LOCAL_LOG, "|\n");
	}

	if (v) fprintf (LOCAL_LOG, "] = All main fields detected\n");

	// Init struct:
	struct post* post = (struct post*) malloc (sizeof(struct post));
	if (post == NULL) {
		if (v) {
			printf(LOCAL_LOG, "Memory corrupt - `struct post' calloc()\n");
			fflush(LOCAL_LOG);
		} else {
			fprintf(LOCAL_LOG, "Memory allocated (struct post)\n");
			fflush(LOCAL_LOG);
		}
		return (char *) ERR_MEMORY;
	}

	post->num = str2lint(ptr_num, num_len);

	char* comment_str = (char*) calloc (comment_len, sizeof(char));
	if (comment_str == NULL) {
		if (v) {
			fprintf(LOCAL_LOG, "Memory corrupt - `comment_str' calloc()\n");
			fflush(LOCAL_LOG);
		} else {
			fprintf(LOCAL_LOG, "Memory allocated (comment_str)\n");
			fflush(LOCAL_LOG);
		}
		return ERR_MEMORY;
	}
	memcpy(comment_str, ptr_comment, comment_len);
	post->comment = parseComment (comment_str, comment_len, true);
	fprintf(LOCAL_LOG, "Exited parseComment()\n");
	free (comment_str);
	fprintf(LOCAL_LOG, "Freed comment_str\n");
	if (v) {
		fprintf(stderr, "Clean text: %s\n", post->comment);
	}

	post->date = (char*) calloc (date_len+1, sizeof(char));
	if (post->date == NULL) {
		fprintf (stderr, "! Error allocating memory (post.date)\n");
		return ERR_MEMORY;
	}
	memcpy (post->date, ptr_date, sizeof(char)*date_len);

	post->name = (char*) calloc (name_len+1, sizeof(char));
	if (post->name == NULL) {
		fprintf (stderr, "! Error allocating memory (post.name)\n");
		return ERR_MEMORY;
	}
	memcpy (post->name, ptr_name, sizeof(char)*name_len);

	if (email_len == 0) {
		post->email = NULL;
	} else {
		post->email = (char*) calloc (email_len+1, sizeof(char));
		if (post->email == NULL) {
			fprintf (stderr, "! Error allocating memory (post.email)\n");
			return ERR_MEMORY;
		}
		memcpy (post->email, ptr_email, sizeof(char)*email_len);
	}

	if (files_len == 0) {
		post->files = NULL;
	} else {
		post->files = (char*) calloc (files_len+1, sizeof(char));
		if (post->files == NULL) {
			fprintf (stderr, "! Error allocating memory (post.files)\n");
			return ERR_MEMORY;
		}
		memcpy (post->files, ptr_files, sizeof(char)*files_len);
	}

	if (v) {
		fprintf(LOCAL_LOG, "] = Init struct done\n");
		fprintf(LOCAL_LOG, "<< End of Thread >>\n");
		fclose(LOCAL_LOG);
	}
	fprintf (stderr, "]] Exiting initPost\n");
	return post;
}

char *parseComment (char *comment, const long long  comment_len, const bool v) { // Пока что игнорируем разметку
	fprintf (stderr, "]] Started parseComment\n");
	fprintf(stderr, "Comment len: %d\n", comment_len);
	char *parsed = (char *) calloc(comment_len, sizeof(char));
	int depth = 0;
	long long parsed_len = 0;
	long long i;

	for (i = 0; i < comment_len; i++) {
		if (strncmp(&(comment[i]), PATTERN_TAG_OPEN, strlen(PATTERN_TAG_OPEN)) == 0) { // HTML tag open
			/*if (strncmp(&(comment[i]), , strlen(PATTERN_NEWLINE)) == 0) { // Post answer

			}
			else*/ {
				depth ++;
				if (strncmp(&(comment[i]), PATTERN_NEWLINE, strlen(PATTERN_NEWLINE)) == 0) { // <br>
					parsed[parsed_len] = '\n';
					parsed_len ++;
				}
				i += strlen(PATTERN_TAG_OPEN) - 1;
			}
		}
		else
			if (strncmp(&(comment[i]), PATTERN_TAG_CLOSE, strlen(PATTERN_TAG_CLOSE)) == 0) { // '>' char
					if (depth > 0) { // HTML tag close
						depth --;
					}
					else { // In-text '>'
						parsed[parsed_len] = '>';
						parsed_len ++;
					}
					i += strlen(PATTERN_TAG_CLOSE) - 1;
				}
				else
					if (depth == 0) { // Ordinary character
						parsed[parsed_len] = comment[i];
						parsed_len ++;
					}
	}
	fprintf(stderr, "]] Final length: %d\n", parsed_len);
	fprintf(stderr, "]] Exiting parseComment\n");
	return parsed;
}

char* cleanupComment (const char* src, const int src_len, int *new_len, const bool v) {
	fprintf(stderr, "]] Started cleanupComment");
	// FILE* LOCAL_LOG = NULL;
	// fprintf(stderr, "NULL done\n");
	// if (v) LOCAL_LOG = fopen("log/cleanupComment.log", "a");
	// fprintf(stderr, "fopen done\n");
	// if (v) fprintf(stderr, " (verbose, log in log/cleanupComment.log");
	// fprintf(stderr, "\n");
	fprintf(stderr, "\nCleanup === New Thread ===\nArgs:\n| src_len = %d\n", src_len);
	// fprintf(stderr, "\n\n=== New Thread ===\nArgs:\n| src = %s\n| src_len = %d\n", src, src_len);
	// if (v) fflush(LOCAL_LOG);

	char* buf = (char*) calloc (src_len, sizeof(char));
	fprintf(stderr, "Alloc done\n");
	long int pos = 0, len = 0;
	for ( ; pos < src_len; ) {
		if ((pos < src_len-7) && (src[pos] == '\\') && (src[pos+1] == 'u') && (src[pos+2] == '0') && (src[pos+3] == '0')
			                && (src[pos+4] == '3') && (src[pos+5] == 'c') && (src[pos+6] == 'b') && (src[pos+7] == 'r')) {
			pos += 14;
			buf[len] = '\n';
			len++;
		} else if ((pos < src_len-27)  && (src[pos   ] == '\\') && (src[pos+1 ] == 'u' ) && (src[pos+2 ] == '0' ) && (src[pos+3 ] == '0')
				     	 			   && (src[pos+4 ] == '3' ) && (src[pos+5 ] == 'c' ) && (src[pos+6 ] == 's' ) && (src[pos+7 ] == 'p')
					  	 			   && (src[pos+8 ] == 'a' ) && (src[pos+9 ] == 'n' ) && (src[pos+10] == ' ' ) && (src[pos+11] == 'c')
					  				   && (src[pos+12] == 'l' ) && (src[pos+13] == 'a' ) && (src[pos+14] == 's' ) && (src[pos+15] == 's')
					  				   && (src[pos+16] == '=' ) && (src[pos+17] == '\\') && (src[pos+18] == '"' ) && (src[pos+19] == 'u')
					  				   && (src[pos+20] == 'n' ) && (src[pos+21] == 'k' ) && (src[pos+22] == 'f' ) && (src[pos+23] == 'u')
					  				   && (src[pos+24] == 'n' ) && (src[pos+25] == 'c' ) && (src[pos+26] == '\\') && (src[pos+27] == '"')) {
			pos += 38; // TEST!
			buf[len] = '>';
			len++;
			// Color: green
		} else if ((pos < src_len-10) && (src[pos   ] == '\\') && (src[pos+1 ] == 'u' ) && (src[pos+2 ] == '0' ) && (src[pos+3 ] == '0')
				    				  && (src[pos+4 ] == '3' ) && (src[pos+5 ] == 'c' ) && (src[pos+6 ] == '/' ) && (src[pos+7 ] == 's')
					  				  && (src[pos+8 ] == 'p' ) && (src[pos+9 ] == 'a' ) && (src[pos+10] == 'n' )) {
			pos += 17;
		} else if ((pos < src_len-5) && (src[pos   ] == '\\') && (src[pos+1 ] == '\\' ) && (src[pos+2 ] == 'r') && (src[pos+3 ] == '\\')
									 && (src[pos+4 ] == '\\') && (src[pos+5 ] == 'n')) {
			pos += 6;
			buf[len] = '\n';
			len++;
		} else {
			buf[len] = src[pos];
			len++;
			pos++;
		}
	}
	fprintf(stderr, "[cleanupComment] len = %d\n[cleanupComment] doing calloc()\n",len);
	char* clean = (char*) calloc (len + 1, sizeof(char));
	fprintf(stderr, "[cleanupComment] calloc() done, doing memcpy()\n");
	memcpy(clean, buf, sizeof(char)*len);
	fprintf(stderr, "[cleanupComment] memcpy() done\n");
	free (buf);
	//if (v) fprintf(LOCAL_LOG, "=== End of Thread ===\n");
	//if (v) fclose(LOCAL_LOG);
	*new_len = len;
	fprintf(stderr, "]] Exiting cleanupComment\n");
	return clean;
}

struct ref_reply* parseRef_Reply (const char* ch_ref, const long int ref_len, const bool v) {
	fprintf (stderr, "-]] Started parseRef_Reply");
	//FILE* LOCAL_LOG = NULL;
	/*
	if (v) {
		LOCAL_LOG = fopen("log/parseRef_Reply.log", "a");
		fprintf(stderr, " (verbose, log in log/parseRef_Reply.log)"); fprintf (stderr, "\n");
		fprintf(LOCAL_LOG, "\n\n<< New Thread >>\n]] Args:\n== | ch_ref = ");
		for (int i = 0; i < 100; i++)
			fprintf(LOCAL_LOG, "%c", ch_ref[i]);
		fprintf(LOCAL_LOG, "\n| ref_len = %d\n", ref_len);
	}*/

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
	long int data_thread = str2lint (data_thread_start, data_thread_end-data_thread_start+1);
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
	long int data_num = str2lint (data_num_start, data_num_end-data_num_start+1);
	if (v) fprintf (stderr, "-]]] Postnum: %d\n", data_num);

	struct ref_reply* ref_parsed = (struct ref_reply*) calloc (1, sizeof(struct ref_reply));
	ref_parsed->link = (char*) calloc (link_len, sizeof(char) + 1);
	memcpy (ref_parsed->link, link_start, link_len*sizeof(char));
	ref_parsed->thread = data_thread;
	ref_parsed->num = data_num;

	if (v)	fprintf(stderr, "<< End of Thread >>\n");
	fprintf (stderr, "-]] Exiting parseRef_Reply\n");

	return ref_parsed;
}

struct thread* initThread (const char* thread_string, const long int thread_len, const bool v) {
	fprintf(stderr, "\n]] Started initThread ");
	FILE* LOCAL_LOG = NULL;
	if (v) {
		LOCAL_LOG = fopen("log/initThread.log", "a");
		fprintf(stderr, " (verbose, log in log/initThread.log)\n");
		fprintf(LOCAL_LOG, "\n\n== New Thread ==\n]] Args:\n| thread_string = %p\n| thread_len = %d\n",
			thread_string, thread_len);
	}
	fflush(LOCAL_LOG);

	long int nposts = 0;
	long int* post_diffs = findPostsInJSON( thread_string, &nposts, true );
	if (v) fprintf(LOCAL_LOG, "] nposts = %d\n", nposts);
	fflush(LOCAL_LOG);

	struct post** posts = (struct post**) calloc (nposts, sizeof(struct post*));
	for (int i = 0; i < ((int)nposts)-1; i++) {
		char *cpost_ptr = thread_string + (ptrdiff_t) post_diffs[i];
		long int cpost_len = post_diffs[i+1] - post_diffs[i];
		if (v) fprintf( LOCAL_LOG, "] Calling initPost #%d: from %d to %d, length %d\n",
			i, post_diffs[i], post_diffs[i+1], cpost_len );
		fflush(LOCAL_LOG);
		posts[i] = initPost(
							cpost_ptr,
							cpost_len,
							true
							);
		if (posts[i] == (char *) ERR_COMMENT_FORMAT) {
			fprintf(stderr, "! Error: initPost() returned ERR_COMMENT_FORMAT\n === Exiting ===");
			exit(3);
		}
		if (posts[i] == (char *) ERR_POST_OUT_OF_RANGE) {
			fprintf(stderr, "! Error: initPost() returned ERR_POST_OUT_OF_RANGE\n === Exiting ===");
			exit(4);
		}
	}
	posts[nposts-1] = initPost(
							   thread_string + post_diffs[nposts-1],
							   strlen(thread_string) - post_diffs[nposts-1],
							   true
							   );

	struct thread* thread = (struct thread*) calloc (1, sizeof(struct thread));
	thread->num = posts[0]->num;
	thread->nposts = nposts;
	thread->posts = posts;

	if (v) {
		fprintf(LOCAL_LOG, "] Struct init done:\n| num = %d\n| nposts = %d\n| posts = %p\n",
			thread->num, thread->nposts, thread->posts);
		fprintf(LOCAL_LOG, "== End of Thread ==\n");
		fclose(LOCAL_LOG);
	}
	fprintf(stderr, "]] Exiting initThread\n");

	return thread;
}

// ========================================
// Captcha
// ========================================

char* parse2chaptchaId (const char* capId_string) {
	fprintf (stderr, "]] Starting parse2chaptchaId\n");

	char* captcha_start = strstr (capId_string, PATTERN_CAPID);
	if (captcha_start == NULL) {
		fprintf(stderr, "[parse2chaptchaId]! Error: Bad captcha-JSON format\n");
		return ERR_CAPTCHA_FORMAT;
	}
	captcha_start += strlen(PATTERN_CAPID);
	short captcha_len = strstr(captcha_start,"\"") - captcha_start;
	char* captcha_id = (char*) calloc (captcha_len, sizeof(char));
	captcha_id = memcpy (captcha_id, captcha_start, captcha_len);
	
	fprintf(stderr, "]] Exiting parse2chaptchaId\n");

	return captcha_id;
}

// ========================================
// Freeing functions
// ========================================

void freeRefReply (struct ref_reply *ref) {
	free (ref->link);
}

void freeComment (char *arg) {
	free (arg);
}

void freePost (struct post *post) {
	freeComment (post->comment);
	free (post->date);
	free (post->name);
	if (post->email != -1)
		free (post->email);
	if (post->files != -1)
		free (post->files);
	free (post);
}

void freeThread (struct thread *thread) {
	for (int i = 1; i < thread->nposts; i++) {
		freePost (thread->posts[i]);
	}
	free(thread->posts);
	free(thread);
}

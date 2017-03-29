// ========================================
// File: parser.c
// API answer parsing functions
// (Implementation)
// ========================================

#pragma once
#include "parser.h"

long int *findPostsInJSON (const char *src, long int *postcount_res, const bool v) {
	fprintf (stderr, "]] Starting findPostsInJSON");

	if (v) fprintf (stderr, " (verbose");
	fprintf (stderr, "\n");
	if (v) fprintf(stderr, "\n\n<< New Thread >>\n");

	int srclen = strlen (src);
	if (v) fprintf(stderr, "srclen = %d\n", srclen);
	long int *temp = (long int*) calloc (srclen/8, sizeof(long int));

	short depth = 0;
	int postcount = 0;
	bool comment_read = false;

	for (int i = 1; i < srclen; i+=1) {
		if (v) fprintf(stderr, "%c", src[i]);
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
					if (v) fprintf (stderr, "Exiting name\n");
				}
				break;
			}
			case 2: {
				if (comment_read) { // in .post.files
					if ((src[i-2]=='"')&&(src[i-1]=='d')&&(src[i]=='i')&&
						(src[i+1]=='s')&&(src[i+2]=='p')) {
							depth += 3; // Вход в displayname
							if (v) fprintf (stderr, "Entering displayname\n");
							break;
						}
					if ((src[i-2]=='"')&&(src[i-1]=='f')&&(src[i]=='u')&&
						(src[i+1]=='l')&&(src[i+2]=='l')) {
							depth += 3; // Вход в fullname
							if (v) fprintf (stderr, "Entering fullname\n");
							break;
					}
					if (src[i] == ']') {
						if (v) fprintf (stderr, "Exiting files. ");
						depth -= 1;
						break;
					}
				}
				else { // in: .post.comment
					if ((src[i-1] != '\\') && (src[i] == '"')) {
						if (v) fprintf (stderr, "Exiting comment. ");
						comment_read = true;
						depth -= 1;
					}
				}
				break;
			}
			case 1: { // in: .post
				if (src[i] == '}') {
					if (v) fprintf (stderr, "Exiting post (@%d).\n", i);
					postcount += 1;
					depth -= 1;
					comment_read = false;
				}
				if ((src[i-2]=='"')&&(src[i-1]=='f')&&(src[i]=='i')&&
					(src[i+1]=='l')&&(src[i+2]=='e')) {
					if (v) fprintf (stderr, "Entering files. ");
					depth += 1;
				}
				if ((src[i-2]=='"')&&(src[i-1]=='c')&&(src[i]=='o')&&
					(src[i+1]=='m')&&(src[i+2]=='m')) {
					if (v) fprintf (stderr, "Entering comment. ");
					depth += 1;
				}
				continue;
			}
			case 0: { // in: .
				if (src[i] == '{') {
					if (v) fprintf (stderr, "Entering post #%d ", postcount+1);
					temp[postcount] = i;
					if (v) fprintf (stderr, "(@%d)\n", temp[postcount]);
					depth += 1;
				}
				continue;
			}
			default: {
				fprintf (stderr, "! Error: Unusual depth (%d)\n", depth);
				free (temp);
				makaba_errno = ERR_PARTTHREAD_DEPTH;
				return NULL;
			}
		}
	}
	if (depth != 0) {
		fprintf (stderr, "[findPostsInJSON]! Error: Non-zero depth (%d) after cycle\n", depth);
		makaba_errno = ERR_PARTTHREAD_DEPTH;
		return NULL;
	}

	if (v) fprintf (stderr, "%d posts found\n", postcount);

	long int *posts = (long int*) calloc (postcount, sizeof(long int));
	memcpy (posts, temp, postcount*sizeof(long int));
	free (temp);
	if (v) {
		fprintf (stderr, "Posts begin at:\n");
		for (int i = 0; i < postcount; i++)
			fprintf (stderr, "] %2d: %5d\n", i, posts[i]);
	}

	*postcount_res = postcount;

	if (v) fprintf(stderr, "<< End of Thread >>\n");
	fprintf(stderr, "]] Exiting findPostsInJSON\n");
	return posts;
}

struct post *initPost (const char *post_string, const long int postlen, const bool v) {
	fprintf(stderr, "]] Starting initPost");
	if (v) fprintf(stderr, " (verbose)");
	fprintf (stderr, "\n");
	if (v) fprintf(stderr, "\n\n<< New Thread >>\n]] Args:\n== | post_string = ");
	if (v) for (int i = 0; i < postlen; i++)
		fprintf(stderr, "%c", post_string[i]);
	if (v) fprintf(stderr, "\n| postlen = %d\n", postlen);

	// Detecting comment:
	char *ptr_comment = (char *) strstr (post_string, PATTERN_COMMENT);
	if (ptr_comment == NULL) {
		fprintf (stderr, "[initPost]! Error: Bad post format: Comment pattern not found\n");
		makaba_errno = ERR_POST_FORMAT;
		return NULL;
	}
	ptr_comment += strlen(PATTERN_COMMENT);

	char *ptr_comment_end = (char *) strstr (ptr_comment, PATTERN_COMMENT_END);
	int comment_len = (int) (ptr_comment_end - ptr_comment);
	if (v) fprintf (stderr, "comment_len = %d\n=== Comment: ===\n", comment_len);
	if (v) for (int i = 0; i < comment_len; i++)
		fprintf (stderr, "%c", ptr_comment[i]);
	if (v) fprintf (stderr, "\n== End of comment ==\n");

	// Detect date:
	char *ptr_date = strstr (ptr_comment+comment_len, PATTERN_DATE)+strlen(PATTERN_DATE);
	if (ptr_date == NULL) {
		fprintf (stderr, "[initPost]! Error: Bad post format: Date pattern not found\n");
		makaba_errno = ERR_POST_FORMAT;
		return NULL;
	}
	int date_len = strstr (ptr_date, "\"") - ptr_date;
	if (v) {
		fprintf (stderr, "] Date length: %d\n", date_len);
		fprintf (stderr, "] Date: ");
		for (int i = 0; i < date_len; i++)
			fprintf (stderr, "%c", ptr_date[i]);
		fprintf (stderr, "\n");
	}

	// Detect email:
	char *ptr_email = strstr(ptr_date + (ptrdiff_t)date_len, PATTERN_EMAIL);
	if (ptr_email == NULL) {
		fprintf (stderr, "[initPost]! Error: Bad post format: Email pattern not found\n");
		makaba_errno = ERR_POST_FORMAT;
		return NULL;
	}
	ptr_email += (ptrdiff_t) strlen(PATTERN_EMAIL);
	int email_len = (int) (strstr (ptr_email, "\"") - ptr_email);
	if (v) {
		if (email_len == 0) {
			fprintf (stderr, "] Email not specified\n");
		} else {
			fprintf (stderr, "] Email length: %d\n", email_len);
			fprintf (stderr, "] Email: ");
			for (int i = 0; i < email_len; i++)
				fprintf (stderr, "%c", ptr_email[i]);
			fprintf (stderr, "\n");
		}
	}

	// Detect files:
	char *ptr_files = strstr(ptr_email + (ptrdiff_t)email_len, PATTERN_FILES);
	int files_len = 0;
	bool has_files = false;
	// If NULL, simply no files in post
	if ( (ptr_files == NULL) || ((long int) (ptr_files - post_string) >= postlen) ) {
		if (v) fprintf (stderr, "] Files not specified\n");
	} else {
		ptr_files += strlen(PATTERN_FILES);
		has_files = true;
		files_len = strstr(ptr_files, "}]") - ptr_files;
		if (files_len == 0) {
			fprintf (stderr, "[initPost]! Error: Bad post format: Files field specified but null\n");
			makaba_errno = ERR_POST_FORMAT;
			return NULL;
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
		fprintf (stderr, "[initPost]! Error: Bad post format: Name pattern not found\n");
		makaba_errno = ERR_POST_FORMAT;
		return NULL;
	}
	ptr_name += (ptrdiff_t)strlen(PATTERN_NAME);

	char *ptr_name_end = strstr(ptr_name, PATTERN_NAME_END);
	int name_len = (int) (ptr_name_end - ptr_name);
	if (v) fprintf (stderr, "] Name length: %d\n", name_len);
	if (v) fprintf (stderr, "] Name: ");
	if (v) for (int i = 0; i < name_len; i++)
		fprintf (stderr, "%c", ptr_name[i]);
	if (v) fprintf (stderr, "[name end]\n");

	// Detect postnum
	char *ptr_num = strstr( ptr_name + (ptrdiff_t)name_len, PATTERN_NUM );
	if (ptr_num == NULL) {
		fprintf (stderr, "[initPost]! Error: Bad post format: Num pattern not found\n");
		makaba_errno = ERR_POST_FORMAT;
		return NULL;
	}
	if (v) fprintf (stderr, "[ptr_num] %c%c%c%c%c\n", ptr_num[0],ptr_num[1],ptr_num[2],ptr_num[3],ptr_num[4]);
	ptr_num += (ptrdiff_t) strlen(PATTERN_NUM);
	char *num_end =  strstr (ptr_num, ",");
	if (num_end == NULL) {
		fprintf(stderr, "[initPost]! Error: Something strange with `num' field\n");
		makaba_errno = ERR_INTERNAL;
		return NULL;
	}
	int num_len = (int) (num_end - ptr_num - 1); // Exclude ending '"'
	if (v) {
		fprintf (stderr, "] Num length: %d\n", num_len);
		fprintf (stderr, "] Num: |");
		for (int i = 0; i < num_len; i++) {
			fprintf (stderr, "%c", ptr_num[i]);
		}
		fprintf (stderr, "|\n");
	}

	if (v) fprintf (stderr, "] = All main fields detected\n");

	// Init struct:
	struct post *post = (struct post*) malloc (sizeof(struct post));
	if (post == NULL) {
		if (v) {
			fprintf(stderr, "Memory corrupt - `struct post' calloc()\n");
		} else {
			fprintf(stderr, "Memory allocated (struct post)\n");
		}
		makaba_errno = ERR_MEMORY;
		return NULL;
	}

	post->num = str2lint(ptr_num, num_len);

	char *comment_str = (char*) calloc (comment_len, sizeof(char));
	if (comment_str == NULL) {
		if (v) {
			fprintf(stderr, "Memory corrupt - `comment_str' calloc()\n");
		} else {
			fprintf(stderr, "Memory allocated (comment_str)\n");
		}
		makaba_errno = ERR_MEMORY;
		return NULL;
	}
	memcpy(comment_str, ptr_comment, comment_len);
	post->comment = parseComment (comment_str, comment_len, true);
	fprintf(stderr, "Exited parseComment()\n");
	free (comment_str);
	fprintf(stderr, "Freed comment_str\n");
	if (v) {
		fprintf(stderr, "Clean text: %s\n", post->comment);
	}

	post->date = (char*) calloc (date_len+1, sizeof(char));
	if (post->date == NULL) {
		fprintf (stderr, "[initPost]! Error allocating memory (post.date)\n");
		makaba_errno = ERR_MEMORY;
		return NULL;
	}
	memcpy (post->date, ptr_date, sizeof(char)*date_len);

	post->name = (char*) calloc (name_len+1, sizeof(char));
	if (post->name == NULL) {
		fprintf (stderr, "[initPost]! Error allocating memory (post.name)\n");
		makaba_errno = ERR_MEMORY;
		return NULL;
	}
	memcpy (post->name, ptr_name, sizeof(char)*name_len);

	if (email_len == 0) {
		post->email = NULL;
	} else {
		post->email = (char*) calloc (email_len+1, sizeof(char));
		if (post->email == NULL) {
			fprintf (stderr, "[initPost]! Error allocating memory (post.email)\n");
			makaba_errno = ERR_MEMORY;
			return NULL;
		}
		memcpy (post->email, ptr_email, sizeof(char)*email_len);
	}

	if (files_len == 0) {
		post->files = NULL;
	} else {
		post->files = (char*) calloc (files_len+1, sizeof(char));
		if (post->files == NULL) {
			fprintf (stderr, "[initPost]! Error allocating memory (post.files)\n");
			makaba_errno = ERR_MEMORY;
			return NULL;
		}
		memcpy (post->files, ptr_files, sizeof(char)*files_len);
	}

	if (v) {
		fprintf(stderr, "] = Init struct done\n");
		fprintf(stderr, "<< End of Thread >>\n");
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

char *cleanupComment (const char *src, const int src_len, int *new_len, const bool v) {
	fprintf(stderr, "]] Started cleanupComment");

	char *buf = (char*) calloc (src_len, sizeof(char));
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
	char *clean = (char*) calloc (len + 1, sizeof(char));
	fprintf(stderr, "[cleanupComment] calloc() done, doing memcpy()\n");
	memcpy(clean, buf, sizeof(char)*len);
	fprintf(stderr, "[cleanupComment] memcpy() done\n");
	free (buf);
	//if (v) fprintf(stderr, "=== End of Thread ===\n");
	*new_len = len;
	fprintf(stderr, "]] Exiting cleanupComment\n");
	return clean;
}

struct thread *initThread (const char *thread_string, const long int thread_len, const bool v) {
	fprintf(stderr, "\n]] Started initThread ");
	if (v) {
		fprintf(stderr, " (verbose)\n");
		fprintf(stderr, "\n\n== New Thread ==\n]] Args:\n| thread_string = %p\n| thread_len = %d\n",
			thread_string, thread_len);
	}

	long int nposts = 0;
	long int *post_diffs = findPostsInJSON( thread_string, &nposts, true );
	if (v) fprintf(stderr, "] nposts = %d\n", nposts);

	struct post **posts = (struct post**) calloc (nposts, sizeof(struct post*));
	for (int i = 0; i < ((int)nposts)-1; i++) {
		char *cpost_ptr = (char *) thread_string + (ptrdiff_t) post_diffs[i];
		long int cpost_len = post_diffs[i+1] - post_diffs[i];
		if (v) fprintf( stderr, "] Calling initPost #%d: from %d to %d, length %d\n",
			i, post_diffs[i], post_diffs[i+1], cpost_len );
		posts[i] = initPost(
							cpost_ptr,
							cpost_len,
							true
							);
		if ((char *) posts[i] == NULL) {
			fprintf(stderr, "[initThread]! Error: initPost() returned %d\n === Exiting ===", makaba_errno);
			exit(3);
		}
	}
	posts[nposts-1] = initPost(
							   (char *) thread_string + post_diffs[nposts-1],
							   strlen(thread_string) - post_diffs[nposts-1],
							   true
							   );

	struct thread *thread = (struct thread*) calloc (1, sizeof(struct thread));
	thread->num = posts[0]->num;
	thread->nposts = nposts;
	thread->posts = posts;

	if (v) {
		fprintf(stderr, "] Struct init done:\n| num = %d\n| nposts = %d\n| posts = %p\n",
			thread->num, thread->nposts, thread->posts);
		fprintf(stderr, "== End of Thread ==\n");
	}
	fprintf(stderr, "]] Exiting initThread\n");

	return thread;
}

// ========================================
// Captcha
// ========================================

char *parse2chaptchaId (const char *capId_string) {
	fprintf (stderr, "]] Starting parse2chaptchaId\n");

	char *captcha_start = (char *) strstr (capId_string, PATTERN_CAPID);
	if (captcha_start == NULL) {
		fprintf(stderr, "[parse2chaptchaId]! Error: Bad captcha-JSON format\n");
		makaba_errno = ERR_CAPTCHA_FORMAT;
		return NULL;
	}
	captcha_start += strlen(PATTERN_CAPID);
	short captcha_len = ((char *) strstr(captcha_start,"\"")) - captcha_start;
	char *captcha_id = (char*) calloc (captcha_len, sizeof(char));
	memcpy (captcha_id, captcha_start, captcha_len);

	fprintf(stderr, "]] Exiting parse2chaptchaId\n");

	return captcha_id;
}

// ========================================
// libjson
// ========================================

int json_callback(void *userdata, int type, const char *data, uint32_t length) {
    json_context *context = (json_context *) userdata;
    if (context->type == thread_new) {
        makaba_thread_cpp *thread = (makaba_thread_cpp *)context->memdest;
        switch (type) {
            case JSON_KEY:
                if (context->status == Status_in_post) {
                    if (fill_expected(context, (char *)data)) {
                        printf("! Error @ fill_expected()\n");
                        return 1;
                    }
                }
                // fill files later
                break;
            case JSON_ARRAY_BEGIN:
                switch(context->status) {
                    case Status_default:
                        //fprintf(stderr, ">>> Entering posts array, Status_in_thread\n");
                        context->status = Status_in_thread;
                        break;
                    case Status_in_post:
                        //fprintf(stderr, ">>> Entering files array, Status_in_files\n");
                        context->status = Status_in_files;
                        break;
                }
                break;
            case JSON_ARRAY_END:
                switch(context->status) {
                    case Status_in_thread:
                        //fprintf(stderr, "<<< Exiting posts array, Status_default\n");
                        context->status = Status_default;
                        fprintf(stderr, "==== Vector has %ld elements\n", thread->posts.size());
                        break;
                    case Status_in_files:
                        //fprintf(stderr, "]\n<<< Exiting files array, Status_in_post\n");
                        context->status = Status_in_post;
                        break;
                }
                break;
            case JSON_OBJECT_BEGIN:
                switch (context->status) {
                    case Status_in_thread:
                        //fprintf(stderr, ">>> Entering post, Status_in_post\n");
                        context->status = Status_in_post;
                        thread->nposts++;
                        thread->posts.push_back({ });
                        break;
                }
                break;
            case JSON_OBJECT_END:
                switch (context->status) {
                    case Status_in_post:
                        //printf("<<< Exiting post, Status_in_thread\n");
                        context->status = Status_in_thread;
                        break;
                }
                break;
            case JSON_STRING:
                if (context->status == Status_in_post) {
                    //fprintf(stderr, "Will fill as string %d\n", context->expect);
                    if (fill_as_string(thread->posts[thread->posts.size() - 1], context->expect, data)) {
                        fprintf(stderr, "! Error @ fill_as_string\n");
                        return 1;
                    }
                }
                break;
            case JSON_INT:
            case JSON_FLOAT:
                if (context->status == Status_in_post) {
                    //fprintf(stderr, "Will fill as int %d\n", context->expect);
                    if (fill_as_int(thread->posts[thread->posts.size() - 1], context->expect, data)) {
                        fprintf(stderr, "! Error @ fill_as_int\n");
                        return 1;
                    }
                }
                break;
            default:
                printf("[unhandled] \"%s\"\n", (char *)userdata);
                break;
        }
        return 0;
    }
}

int fill_expected(json_context *context, const char *data) {
    //fprintf(stderr, "Will expect %s\n", data);
    if (strncmp(data, Key_banned, strlen(Key_banned)) == 0) {
        context->expect = Expect_banned;
    }
    else if (strncmp(data, Key_closed, strlen(Key_closed)) == 0) {
        context->expect = Expect_closed;
    }
    else if (strncmp(data, Key_comment, strlen(Key_comment)) == 0) {
        context->expect = Expect_comment;
    }
    else if (strncmp(data, Key_date, strlen(Key_date)) == 0) {
        context->expect = Expect_date;
    }
    else if (strncmp(data, Key_email, strlen(Key_email)) == 0) {
        context->expect = Expect_email;
    }
    else if (strncmp(data, Key_files, strlen(Key_files)) == 0) {
        context->expect = Expect_files;
    }
    else if (strncmp(data, Key_lasthit, strlen(Key_lasthit)) == 0) {
        context->expect = Expect_lasthit;
    }
    else if (strncmp(data, Key_name, strlen(Key_name)) == 0) {
        context->expect = Expect_name;
    }
    else if (strncmp(data, Key_num, strlen(Key_num)) == 0) {
        context->expect = Expect_num;
    }
    else if (strncmp(data, Key_op, strlen(Key_op)) == 0) {
        context->expect = Expect_op;
    }
    else if (strncmp(data, Key_parent, strlen(Key_parent)) == 0) {
        context->expect = Expect_parent;
    }
    else if (strncmp(data, Key_sticky, strlen(Key_sticky)) == 0) {
        context->expect = Expect_sticky;
    }
    else if (strncmp(data, Key_subject, strlen(Key_subject)) == 0) {
        context->expect = Expect_subject;
    }
    else if (strncmp(data, Key_tags, strlen(Key_tags)) == 0) {
        context->expect = Expect_tags;
    }
    else if (strncmp(data, Key_timestamp, strlen(Key_timestamp)) == 0) {
        context->expect = Expect_timestamp;
    }
    else if (strncmp(data, Key_trip, strlen(Key_trip)) == 0) {
        context->expect = Expect_trip;
    }
    else if (strncmp(data, Key_trip_type, strlen(Key_trip_type)) == 0) {
        context->expect = Expect_trip_type;
    }
    else if (strncmp(data, Key_unique_posters, strlen(Key_unique_posters)) == 0) {
        context->expect = Expect_unique_posters;
    }
    else {
        fprintf(stderr, "! Error @ fill_expected: unknown key %s\n", data);
        return 1;
    }
    return 0;
}

int fill_as_int(makaba_post_cpp &post, const int expect, const char *data) {
    switch (expect) {
        case Expect_banned:
            post.banned = atoi(data);
            return 0;
        case Expect_closed:
            post.closed = atoi(data);
            return 0;
        case Expect_lasthit:
            post.lasthit = atoi(data);
            return 0;
        // libjson определяет длинные номера постов как строки;
        // парсятся в fill_as_string()
        /*
        case Expect_num:
        case Expect_parent:
        */
        case Expect_op:
            post.op = atoi(data);
            return 0;
        case Expect_sticky:
            post.sticky = atoi(data);
            return 0;
        case Expect_timestamp:
            post.timestamp = atoi(data);
            return 0;
        // unique_posters тоже как string
        /*
        case Expect_unique_posters:
        */
    }
    return 1;
}

int fill_as_string(makaba_post_cpp &post, const int expect, const char *data) {
    switch (expect) {
        //fprintf(stderr, "Expect %d ...\n", expect);
        case Expect_comment:
            post.comment = (char *) calloc(strlen(data) + 1, sizeof(char));
            memcpy(post.comment, data, strlen(data));
            return 0;
        case Expect_date:
            post.date = (char *) calloc(strlen(data) + 1, sizeof(char));
            memcpy(post.date, data, strlen(data));
            return 0;
        case Expect_email:
            post.email = (char *) calloc(strlen(data) + 1, sizeof(char));
            memcpy(post.email, data, strlen(data));
            return 0;
        case Expect_name:
            post.name = (char *) calloc(strlen(data) + 1, sizeof(char));
            memcpy(post.name, data, strlen(data));
            return 0;
        case Expect_num: // libjson определяет длинные номера постов как строки:
            post.num = atoi(data);
            return 0;
        case Expect_parent: // это тоже номер поста
            post.parent = atoi(data);
            return 0;
        case Expect_subject:
            post.subject = (char *) calloc(strlen(data) + 1, sizeof(char));
            memcpy(post.subject, data, strlen(data));
            return 0;
        case Expect_tags:
            post.tags = (char *) calloc(strlen(data) + 1, sizeof(char));
            memcpy(post.tags, data, strlen(data));
            return 0;
        case Expect_trip:
            post.trip = (char *) calloc(strlen(data) + 1, sizeof(char));
            memcpy(post.trip, data, strlen(data));
            return 0;
        case Expect_trip_type:
            post.trip_type = (char *) calloc(strlen(data) + 1, sizeof(char));
            memcpy(post.trip_type, data, strlen(data));
            return 0;
        case Expect_unique_posters: // unique_posters тоже как string
            post.unique_posters = atoi(data);
            return 0;
    }
    fprintf(stderr, "! Error @ fill_as_string: unknown context.expect value: %d\n", expect);
    return 1;
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
	if (post->email != NULL)
		free (post->email);
	if (post->files != NULL)
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

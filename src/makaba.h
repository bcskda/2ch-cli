// ========================================
// File: makaba.h
// High-level imageboard API
// ========================================


#include <iostream>
#include <string>
#include <vector>
#include <json/json.h>
#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include <memory>
#include "API.h"
#include "external.h"
#include "error.h"
#pragma once


extern const char *PATTERN_TAG_OPEN;
extern const char *PATTERN_TAG_CLOSE;
extern const char *PATTERN_HREF_OPEN;
extern const char *PATTERN_HREF_CLOSE;
extern const char *PATTERN_REPLY_CLASS;
extern const char *PATTERN_REPLY_THREAD;
extern const char *PATTERN_REPLY_NUM;
extern const char *PATTERN_NEWLINE;
extern const char *PATTERN_LT;
extern const char *PATTERN_GT;
extern const char *PATTERN_SLASH;
extern const char *PATTERN_BCKSLASH;
extern const char *PATTERN_NBSP;
extern const char *PATTERN_SINGLE_QUOT;
extern const char *PATTERN_DOUBLE_QUOT;
extern const char *PATTERN_AMP;


extern const char *CaptchaPngFilename;
extern const char *CaptchaUtfFilename;


namespace Makaba {
	class Post {
		bool isNull_;
	public:
		bool banned;
		bool closed;
		std::string comment;
		std::string date;
		std::string email;
		Json::Value files;
		long long lasthit;
		std::string name;
		long long num;
		bool op;
		long long parent;
		bool sticky;
		std::string subject;
		std::string tags;
		long long timestamp;
		std::string trip;
		std::string trip_type; // enum?
		long long unique_posters;
		long long rel_num;
		/* === */
		Post();
		Post(const std::string &vcomment, const std::string &vemail,
			 const std::string &vname,    const std::string &vsubject,
			 const std::string &vtags,    const std::string &vtrip);
		Post(const char *vcomment, const char *vemail,
			 const char *vname,    const char *vsubject,
			 const char *vtags,    const char *vtrip);
		Post(Json::Value &val);
		Post(const std::string &raw);
		/* === */
		Post &operator=(const Post &rhs);
		/* === */
		bool isNull();
	};

	class Thread {
		bool isNull_;
		struct {
			void *userdata;
			void *(*on_update)(void *userdata, const char *raw);
			bool set;
		} hook_;
	public:
		long long num;
		long long nposts;
		std::string board;
		std::vector<Post> posts;
		/* === */
		Thread();
		Thread(const std::string &board, const long long &num, const bool inst_dl = true);
		Thread(const std::string &board, const std::string &raw);
		/* === */
		Thread &operator = (const Thread &rhs);
		Thread &operator << (const char *rhs);
		/* === */
		bool isNull();
		bool has_hook();
		void set_hook(
			void *userdata,
			void *(*on_update)(void *userdata, const char *raw)
		);
		/* === */
		int append(const char *raw); // Надо бы ее приватной
		int update();
		std::string send_post(const Post &post);
		const long long find(const long long &pnum);
		std::vector<Post &> find(const std::string comment); // @TODO
	};

	static const Thread NullThread;

	class Captcha_2ch {
		bool isNull_;
		std::string png_url;
		std::string error_;
		/* === */
		Captcha_2ch();
		/* === */
		int get_id(const std::string &board, const long long &threadnum);
		int form_url();
	public:
		std::string id;
		std::string value;
		/* === */
		Captcha_2ch(const std::string &board, const long long &threadnum);
		Captcha_2ch(const Thread& thread); // @TODO
		/* === */
		bool isNull();
		int get_png();
		const std::string &error();
	};
}

char *parseHTML (const char *raw, const long long &raw_len, const bool &v);

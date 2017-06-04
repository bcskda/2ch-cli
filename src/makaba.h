// ========================================
// File: makaba.h
// High-level imageboard API
// ========================================


#include <iostream>
#include <string>
#include <vector>
#include <json/json.h>
#include "API.h"
#include "parser.h"
#include "external.h"
#include "error.h"
#pragma once


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
		Post();
		Post(const std::string &vcomment, const std::string &vemail,
			const std::string &vname,    const std::string &vsubject,
			const std::string &vtags,    const std::string &vtrip);
		Post(const char *vcomment, const char *vemail,
			const char *vname,    const char *vsubject,
			const char *vtags,    const char *vtrip);
		Post(Json::Value &val);
		Post(const char *raw); // @TODO
		bool isNull();
	};

	class Thread {
		bool isNull_;
		Thread();
		int append(const char *raw);
	public:
		long long num;
		long long nposts;
		std::string board;
		std::vector<Makaba::Post> posts;
		Thread(const std::string &board, const long long &num);
		int update();
		bool isNull();
	};

	class Captcha_2ch {
		bool isNull_;
		std::string png_url;
		Captcha_2ch();
		int get_id(const std::string &board, const long long &threadnum);
		int form_url();
	public:
		std::string id;
		std::string value;
		std::string error; // @TODO ro
		Captcha_2ch(const std::string &board, const long long &threadnum);
		Captcha_2ch(const Makaba::Thread& thread); // @TODO
		bool isNull();
		int get_png();
	};

}

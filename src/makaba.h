// ========================================
// File: makaba.h
// High-level imageboard API
// ========================================


#include <iostream>
#include <string>
#include <vector>
#include <json/json.h>
#include <cstdlib>
#include <cstddef>
#include <memory>
#include "API.h"
#include "error.h"
#pragma once

using std::clog;
using std::endl;
using std::string;
using std::vector;

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


namespace Makaba {
    class Post;
    class Thread;
    class Captcha_2ch;
    
    class Post {
        bool isNull_;
        /* === */
        Post();
    public:
        bool banned;
        bool closed;
        string comment;
        string date;
        string email;
        Json::Value files;
        long long lasthit;
        string name;
        long long num;
        bool op;
        long long parent;
        bool sticky;
        string subject;
        string tags;
        long long timestamp;
        string trip;
        string trip_type; // enum?
        long long rel_num;
        /* === */
        //Post();
        Post(const string &vcomment, const string &vemail,
             const string &vname,    const string &vsubject,
             const string &vtags,    const string &vtrip);
        Post(const char *vcomment, const char *vemail,
             const char *vname,    const char *vsubject,
             const char *vtags,    const char *vtrip);
        Post(Json::Value &val);
        Post(const string &raw);
        /* === */
        Post &operator=(const Post &rhs);
        /* === */
        bool isNull() const;
    };
    
    class Thread {
        bool isNull_;
        struct {
            void *userdata;
            void *(*on_update)(void *userdata, const char *raw);
            bool set;
        } hook_;
        /* Нужно ли?
        * bool autodel_captcha_;
        */
        vector<Post *> posts_; // (sic)
    public:
        long long num;
        long long nposts;
        string board;
        Captcha_2ch *captcha;
        /* === */
        Thread();
        Thread(const string &board, const long long &num, const bool inst_dl = true);
        Thread(const string &board, const string &raw);
        ~Thread();
        /* === */
        Thread &operator = (const Thread &rhs);
        Post & operator [] (size_t i) const;
        Post & operator [] (size_t i);
        Thread &operator << (const char *rhs);
        /* === */
        bool isNull() const;
        bool has_hook() const;
        void set_hook(
            void *userdata,
            void *(*on_update)(void *userdata, const char *raw)
        );
        /* === */
        int append(const char *raw); // Надо бы ее приватной
        int update();
        string send_post(const Post &post);
        const long long find(const long long &pnum) const;
        const vector<const Post *> find(const string& comment) const;
    };
    
    class Captcha_2ch {
        bool isNull_;
        bool hasPng_;
        string png_url;
        string error_;
        /* === */
        Captcha_2ch();
        /* === */
        int get_id(const string &board, const long long &threadnum);
        int form_url();
    public:
        string id;
        string value;
        /* === */
        Captcha_2ch(const string &board, const long long &threadnum);
        Captcha_2ch(const Thread &thread);
        /* === */
        bool isNull() const;
        bool hasPng() const;
        int get_png();
        const string &error() const;
    };
    
    static const Thread NullThread;
}

char *parseHTML (const char *raw, const long long &raw_len, const bool &v);

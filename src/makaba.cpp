// ========================================
// File: makaba.cpp
// High-level imageboard API
// ========================================


#include "makaba.h"


const char *PATTERN_TAG_OPEN = "<";
const char *PATTERN_TAG_CLOSE = ">";
const char *PATTERN_HREF_OPEN = "<a href=\"";
const char *PATTERN_HREF_CLOSE = "</a>";
const char *PATTERN_REPLY_CLASS = "class=\"post-reply-link\"";
const char *PATTERN_REPLY_THREAD = "data-thread=\"";
const char *PATTERN_REPLY_NUM = "data-num=\"";
const char *PATTERN_NEWLINE = "<br>";
const char *PATTERN_LT = "&lt";
const char *PATTERN_GT = "&gt";
const char *PATTERN_SLASH = "&#47";
const char *PATTERN_BCKSLASH = "&#92";
const char *PATTERN_NBSP = "&nbsp";
const char *PATTERN_SINGLE_QUOT = "&#39";
const char *PATTERN_DOUBLE_QUOT = "&quot";
const char *PATTERN_AMP = "&amp";


const char *CaptchaPngFilename = "/tmp/2ch-captcha.png";


// ========================================
// Посты
// ========================================


Makaba::Post::Post():
    isNull_(true)
    {}


Makaba::Post::Post(const std::string &vcomment, const std::string &vemail,
                const std::string &vname,    const std::string &vsubject,
                const std::string &vtags,    const std::string &vtrip):
    isNull_(false),
    comment(std::string(vcomment)), email  (std::string(vemail)),
    name   (std::string(vname)),    subject(std::string(vsubject)),
    tags   (std::string(vtags)),    trip   (std::string(vtrip))
    {}


Makaba::Post::Post(const char *vcomment, const char *vemail,
                const char *vname,    const char *vsubject,
                const char *vtags,    const char *vtrip):
    isNull_(false                      ),
    comment(vcomment), email  (vemail  ),
    name   (vname   ), subject(vsubject),
    tags   (vtags   ), trip   (vtrip   )
    {}


Makaba::Post::Post(const std::string &raw):
    isNull_(true)
{
    #ifdef MAKABA_DEBUG
    std::cerr << "[[ " << __PRETTY_FUNCTION__ << " ]] " << this << std::endl;
    #endif // ifdef MAKABA_DEBUG
    Json::CharReaderBuilder rbuilder; // TODO Single global
    std::unique_ptr<Json::CharReader> const reader(rbuilder.newCharReader());
    std::string errs;
    Json::Value jval;
    if (! reader->parse(raw.data(), raw.data() + raw.length(), &jval, &errs)) {
        fprintf(stderr, "[%s] Error:\n"
                        "Json::CharReader::parse():\n  %s\n",
                __PRETTY_FUNCTION__, errs.data());
        makaba_errno = ERR_GENERAL_FORMAT;
        return;
    }
    isNull_ = false;
}


Makaba::Post::Post(Json::Value &val):
    isNull_       ( false                                          ),
    banned        ( atoi( val["banned"        ].asString().data()) ),
    closed        ( atoi( val["closed"        ].asString().data()) ),
    comment       ( ""                                             ),
    date          (       val["date"          ].asString()         ),
    email         (       val["email"         ].asString()         ),
    files         (       val["files"         ]                    ),
    lasthit       ( atoi( val["lasthit"       ].asString().data()) ),
    name          ( ""                                             ),
    num           ( atoi( val["num"           ].asString().data()) ),
    op            ( atoi( val["op"            ].asString().data()) ),
    parent        ( atoi( val["parent"        ].asString().data()) ),
    sticky        ( atoi( val["sticky"        ].asString().data()) ),
    subject       (       val["subject"       ].asString()         ),
    tags          (       val["tags"          ].asString()         ),
    timestamp     ( atoi( val["timestamp"     ].asString().data()) ),
    trip          (       val["trip"          ].asString()         ),
    trip_type     (       val["trip_type"     ].asString()         ),
    unique_posters( atoi( val["unique_posters"].asString().data()) ),
    rel_num       ( 0                                              )
{
    #ifdef MAKABA_DEBUG
    std::cerr << "[[ " << __PRETTY_FUNCTION__ << " ]] " << this << std::endl;
    #endif // ifdef MAKABA_DEBUG
    const char *comment_raw = val["comment"].asCString();
    char *comment_parsed = parseHTML(comment_raw, strlen(comment_raw), false);
    this->comment = comment_parsed;
    free(comment_parsed);
    const char *name_raw = val["name"].asCString();
    char *name_parsed = parseHTML(name_raw, strlen(name_raw), false);
    this->name = name_parsed;
    free(name_parsed);
    #ifdef MAKABA_DEBUG
    fprintf(stderr, "<init (new) post #%10lld>\n", this->num);
    #endif // ifdef MAKABA_DEBUG
}


Makaba::Post &Makaba::Post::operator = (const Makaba::Post &rhs)
{
    #ifdef MAKABA_DEBUG
    std::cerr << "[[ " << __PRETTY_FUNCTION__ << " ]] " << this << std::endl;
    #endif // ifdef MAKABA_DEBUG
    this->isNull_        = rhs.isNull_;
    this->banned         = rhs.banned;
    this->comment        = rhs.comment;
    this->date           = rhs.date;
    this->email          = rhs.email;
    this->files          = rhs.files;
    this->lasthit        = rhs.lasthit;
    this->name           = rhs.name;
    this->num            = rhs.num;
    this->op             = rhs.op;
    this->parent         = rhs.parent;
    this->sticky         = rhs.sticky;
    this->subject        = rhs.subject;
    this->tags           = rhs.tags;
    this->timestamp      = rhs.timestamp;
    this->trip           = rhs.trip;
    this->trip_type      = rhs.trip_type;
    this->unique_posters = rhs.unique_posters;
    this->rel_num        = rhs.rel_num;
    return *this;
}


bool Makaba::Post::isNull() const
{
    return this->isNull_;
}


// ========================================
// Треды
// ========================================


Makaba::Thread::Thread():
    isNull_(true),
    hook_  ({ NULL, NULL, false }),
    //autodel_captcha_(false),
    captcha(NULL)
{
    #ifdef MAKABA_DEBUG
    std::cerr << "[[ " << __PRETTY_FUNCTION__ << " ]] " << this << std::endl;
    #endif // ifdef MAKABA_DEBUG
}


Makaba::Thread::Thread (
    const std::string &board,
    const std::string &raw
):
    isNull_(true ),
    hook_  ({ NULL, NULL, false }),
    //autodel_captcha_(false),
    num    (0    ),
    nposts (0    ),
    board  (board),
    captcha(NULL)
{
    #ifdef MAKABA_DEBUG
    std::cerr << "[[ " << __PRETTY_FUNCTION__ << " ]] " << this << std::endl;
    #endif // ifdef MAKABA_DEBUG
    Json::CharReaderBuilder rbuilder; // TODO Single global (dup Makaba::Post::Post())
    std::unique_ptr<Json::CharReader> const reader(rbuilder.newCharReader());
    std::string errs;
    Json::Value array;
    if (! reader->parse(raw.data(), raw.data() + raw.length(), &array, &errs)) {
        fprintf(stderr, "[%s] Error:\n"
                        "Json::CharReader::parse():\n  %s\n",
                __PRETTY_FUNCTION__, errs.data());
        makaba_errno = ERR_GENERAL_FORMAT;
        return;
    }
    for (auto obj : array) {
        this->nposts++;
        Makaba::Post *post = new Makaba::Post(obj);
        post->rel_num = this->nposts;
        this->posts_.push_back(post);
    }
    this->num = this->posts_[0]->num;
}


Makaba::Thread::Thread (
    const std::string &board,
    const long long &num,
    const bool inst_dl
):
    isNull_(true ),
    hook_  ({ NULL, NULL, false }),
    //autodel_captcha_(false),
    num    (num  ),
    nposts (0    ),
    board  (board),
    captcha(NULL)
{
    #ifdef MAKABA_DEBUG
    std::cerr << "[[ " << __PRETTY_FUNCTION__ << " ]] " << this << std::endl;
    #endif // ifdef MAKABA_DEBUG
    if (inst_dl == false)
        return;
    char *raw;
    long long size = -1;
    raw = getThread(this->board.data(), this->num,
                    1, &size, false);
    if (raw == NULL) {
        fprintf(stderr, "[%s] Error @ getThread()\n", __PRETTY_FUNCTION__);
        return;
    }
    if (this->has_hook())
        this->hook_.on_update(this->hook_.userdata, raw);
    if (this->append(raw))
        fprintf(stderr, "[%s] Error @ this->append()\n", __PRETTY_FUNCTION__);
    this->isNull_ = false;
}


Makaba::Thread::~Thread()
{
    #ifdef MAKABA_DEBUG
    std::cerr << "[[ " << __PRETTY_FUNCTION__ << " ]] " << this << std::endl;
    #endif // ifdef MAKABA_DEBUG
    for (auto p : this->posts_)
        delete p;
}

// Копирование
Makaba::Thread &Makaba::Thread::operator = (const Makaba::Thread &rhs)
{
    #ifdef MAKABA_DEBUG
    std::cerr << "[[ " << __PRETTY_FUNCTION__ << " ]] " << this << std::endl;
    #endif // ifdef MAKABA_DEBUG
    this->isNull_ = rhs.isNull_;
    this->hook_   = rhs.hook_;
    this->num     = rhs.num;
    this->nposts  = rhs.nposts;
    this->board   = rhs.board;
    this->posts_   = rhs.posts_;
    fprintf(stderr, "<copy thread #%10lld>\n", this->num);
    return *this;
}


Makaba::Post Makaba::Thread::operator [] (size_t i) const
{
    #ifdef MAKABA_DEBUG
    std::cerr << "[[ " << __PRETTY_FUNCTION__ << " ]] " << this << std::endl;
    #endif // ifdef MAKABA_DEBUG
    return *(this->posts_.at(i));
}


Makaba::Post & Makaba::Thread::operator [] (size_t i)
{
    #ifdef MAKABA_DEBUG
    std::cerr << "[[ " << __PRETTY_FUNCTION__ << " ]] " << this << std::endl;
    #endif // ifdef MAKABA_DEBUG
    return *(this->posts_.at(i));
}


// Дополнение постами
Makaba::Thread &Makaba::Thread::operator << (const char *rhs)
{
    #ifdef MAKABA_DEBUG
    std::cerr << "[[ " << __PRETTY_FUNCTION__ << " ]] " << this << std::endl;
    #endif // ifdef MAKABA_DEBUG
    this->append(rhs);
    return *this;
}

std::vector<Makaba::Post *> Makaba::Thread::find(const std::string& comment) const
{
    std::vector<Post *> results;
    for (auto post : this->posts_)
    {
        if (post->comment.find(comment) != std::string::npos)
            results.push_back(post);
    }
    return results;
}


bool Makaba::Thread::isNull() const
{
    return this->isNull_;
}


bool Makaba::Thread::has_hook() const
{
    return this->hook_.set;
}


void Makaba::Thread::set_hook(
            void *userdata,
            void *(*on_update)(void *userdata, const char *raw)
        )
{
    this->hook_.userdata = userdata;
    this->hook_.on_update = on_update;
    this->hook_.set = true;
}


/*
* bool Makaba::Thread::autodel_captcha() const
* {
* 	return this->autodel_captcha_;
* }
*/


/*
* void Makaba::Thread::autodel_captcha(bool del)
* {
* 	this->autodel_captcha_ = del;
* }
*/


int Makaba::Thread::append(const char *raw)
{
    Json::CharReaderBuilder rbuilder;
    std::unique_ptr<Json::CharReader> const reader(rbuilder.newCharReader());
    std::string errs;
    Json::Value array;
    if (! reader->parse(raw, raw + strlen(raw), &array, &errs)) {
        fprintf(stderr, "[%s] Error:\n"
                        "Json::CharReader::parse():\n  %s\n",
                __PRETTY_FUNCTION__, errs.data());
        makaba_errno = ERR_GENERAL_FORMAT;
        return -1;
    }
    if (! array.isArray()) { // Не массив = объект с ошибкой
        std::cerr << __PRETTY_FUNCTION__ << " Error: raw json not an array\n:";
        std::cerr << array << std::endl;
        switch(array["Code"].asInt()) {
            case -404:
                makaba_errno = ERR_API_THREAD_NOT_FOUND;
                break;
            default:
                makaba_errno = ERR_API_GENERAL;
        }
        return -1;
    }
    for (auto obj : array) {
        this->nposts++;
        Makaba::Post *post = new Makaba::Post(obj);
        post->rel_num = this->nposts;
        this->posts_.push_back(post);
    }
    return 0;
}


int Makaba::Thread::update()
{
    long long size;
    char *raw = getThread(this->board.data(), this->num,
                        this->nposts + 1, &size, false);
    if (raw == NULL) {
        fprintf(stderr, "[%s] Error @ getThread()\n", __PRETTY_FUNCTION__);
        return -1;
    }
    if (this->has_hook())
        this->hook_.on_update(this->hook_.userdata, raw);
    if (this->append(raw)) {
        fprintf(stderr, "[%s] Error @ this->append()\n", __PRETTY_FUNCTION__);
        return -1;
    }
    if (this->isNull()) {
        this->isNull_ = false;
    }
    return 0;
}


std::string Makaba::Thread::send_post(const Makaba::Post &post)
{
    if (this->captcha == NULL) {
        fprintf(stderr, "[%s] Error: null captcha\n",
                        __PRETTY_FUNCTION__);
        return std::string("");
    }
    if (captcha->get_png()) {
        fprintf(stderr, "[%s] Error: Makaba::Captcha_2ch::get_png()\n"
                        "  error = %d\n  description = %s\n",
                        __PRETTY_FUNCTION__, makaba_errno, makaba_strerror(makaba_errno));
        return std::string("");
    }
    
    /* Не относится к API напрямую, должно быть в app.cpp
    * caca_display_t *display = show_img(CaptchaUtfFilename);
    * caca_canvas_t *canvas = caca_get_canvas(display);
    * caca_put_str(canvas,
    * 			 1, Converter_height_i + 1,
    * 			 "Ответ на капчу (секурность уровня sudo): ");
    * caca_refresh_display(display);
    * std::cin >> captcha.value;
    * caca_free_display(display);
    */
    
    char *api_result = sendPost(
            this->board.data(), this->num,
            post.comment.data(), post.subject.data(), post.name.data(), post.email.data(),
            this->captcha->id.data(), this->captcha->value.data()
        );
    
    /*
    * if (this->autodel_captcha_)
    * 	delete this->captcha;
    */
    
    return std::string(api_result);
}


const long long Makaba::Thread::find(const long long &pnum) const
{
    for (size_t i = 0; i < this->posts_.size(); i++) {
        if (this->posts_[i]->num == pnum) // TODO binary search?
            return i;
    }
    return -1;
}


// ========================================
// Капча
// ========================================


bool Makaba::Captcha_2ch::isNull() const
{
    return this->isNull_;
}


Makaba::Captcha_2ch::Captcha_2ch():
    isNull_(true),
    id     (std::string())
    {}



Makaba::Captcha_2ch::Captcha_2ch(const Makaba::Thread &thread):
    isNull_(true),
    id     (std::string())
{
    if (this->get_id(thread.board, thread.num)) {
        fprintf(stderr, "[%s]: Error: this->get_id() failed\n",
                __PRETTY_FUNCTION__);
        return;
    }
    if (this->form_url()) {
        fprintf(stderr, "[%s]: Error: this->form_url() failed\n",
                __PRETTY_FUNCTION__);
        return;
    }
    isNull_ = false;
}


Makaba::Captcha_2ch::Captcha_2ch(const std::string &board, const long long &threadnum):
    isNull_(true),
    id     (std::string())
{
    if (this->get_id(board, threadnum)) {
        fprintf(stderr, "[%s]: Error: this->get_id() failed\n",
                __PRETTY_FUNCTION__);
        return;
    }
    if (this->form_url()) {
        fprintf(stderr, "[%s]: Error: this->form_url() failed\n",
                __PRETTY_FUNCTION__);
        return;
    }
    isNull_ = false;
}


int Makaba::Captcha_2ch::get_id(const std::string &board, const long long &threadnum)
{
    if (this->id.length()) {
        fprintf(stderr, "[%s] Note: already has ID\n"
                        "  board = %s, thread = %lld\n",
                __PRETTY_FUNCTION__, board.data(), threadnum);
        return -1;
    }
    char *id_raw = get2chaptchaId(board.data(), threadnum, false);
    if (id_raw == NULL) {
        fprintf(stderr, "[%s] Error: get2chaptchaId(): %d\n",
                __PRETTY_FUNCTION__, makaba_errno);
        return -1;
    }
    Json::CharReaderBuilder rbuilder;
    std::unique_ptr<Json::CharReader> const reader(rbuilder.newCharReader());
    std::string errs;
    Json::Value ans;
    if (! reader->parse(id_raw, id_raw + strlen(id_raw), &ans, &errs)) {
        fprintf(stderr, "[%s] Error:\n"
                        "Json::CharReader::parse():\n  %s\n",
                __PRETTY_FUNCTION__, errs.data());
        makaba_errno = ERR_GENERAL_FORMAT;
        return -1;
    }
    std::cerr << ans << std::endl;
    if (! ans["error"].isNull()) {
        fprintf(stderr, "[%s] Error: \n"
                        "API returned \"error\":\"%d\"\n",
                        __PRETTY_FUNCTION__, ans["error"].asInt());
        this->error_ = ans["description"].asString();
        return -1;
    }
    this->id = ans["id"].asString();
    return 0;
}


int Makaba::Captcha_2ch::form_url()
{
    if (this->id.length() == 0) {
        fprintf(stderr, "[%s] Error: ID is null\n", __PRETTY_FUNCTION__);
        return -1;
    }
    if (this->png_url.length()) {
        fprintf(stderr, "[%s] Note: already has png_url, not re-doing\n",
                __PRETTY_FUNCTION__);
        return -1;
    }
    this->png_url.resize(strlen(BASE_URL) + strlen(CAPTCHA_2CHAPTCHA) +
                        this->id.length() + 10);
    this->png_url = BASE_URL;
    this->png_url += '/';
    this->png_url += CAPTCHA_2CHAPTCHA;
    this->png_url += "/image/";
    this->png_url += this->id;
    return 0;
}


int Makaba::Captcha_2ch::get_png() {
    if (this->id.length() == 0) {
        fprintf(stderr, "[%s] Error: ID is null\n", __PRETTY_FUNCTION__);
        return -1;
    }
    if (this->png_url.length() == 0) {
        fprintf(stderr, "[%s] Error: png_url is null\n", __PRETTY_FUNCTION__);
        return -1;
    }
    long long pic_size;
    char *pic = get2chaptchaPicPNG(this->png_url.data(), &pic_size);
    if (pic == NULL) {
        fprintf(stderr, "[%s] Error: get2chaptchaPicPNG() failed: %d\n",
                __PRETTY_FUNCTION__, makaba_errno);
        return -1;
    }
    FILE *pic_file = fopen(CaptchaPngFilename, "w");
    fwrite(pic, sizeof(char), pic_size, pic_file);
    fclose(pic_file);
    /* Не относится к API напрямую, должно быть в app.cpp
    * convert_img(CaptchaPngFilename, CaptchaUtfFilename, false);
    */
    return 0;
}


const std::string &Makaba::Captcha_2ch::error() const
{
    return this->error_;
}


char *parseHTML (const char *raw, const long long &raw_len, const bool &v) { // Пока что игнорируем разметку
    if (v) {
        fprintf(stderr, "]] Started parseHTML\n");
        fprintf(stderr, "Raw len: %lld\n", raw_len);
        fprintf(stderr, "Raw:\n%s\nEnd of raw\n", raw);
    }

    char *parsed = (char *) calloc(raw_len, sizeof(char));
    int depth = 0;
    long long parsed_len = 0;
    long long i;

    for (i = 0; i < raw_len; i++) {
        if (strncmp(&(raw[i]), PATTERN_TAG_OPEN, strlen(PATTERN_TAG_OPEN)) == 0) { // HTML tag open
            /*if (strncmp(&(raw[i]), , strlen(PATTERN_NEWLINE)) == 0) { // Post answer
                // @TODO
            }
            else*/ {
                depth ++;
                if (strncmp(&(raw[i]), PATTERN_NEWLINE, strlen(PATTERN_NEWLINE)) == 0) { // <br>
                    if (v) fprintf(stderr, "NL ");
                    parsed[parsed_len] = '\n';
                    parsed_len ++;
                }
                i += strlen(PATTERN_TAG_OPEN) - 1;
            }
            continue;
        }
        if (strncmp(&(raw[i]), PATTERN_TAG_CLOSE, strlen(PATTERN_TAG_CLOSE)) == 0) { // '>' char
            if (depth > 0) { // HTML tag close
                depth --;
            }
            else { // text '>'
                parsed[parsed_len] = '>';
                parsed_len ++;
            }
            i += strlen(PATTERN_TAG_CLOSE) - 1;
            continue;
        }
        if (depth == 0) {
            if (strncmp(&(raw[i]), PATTERN_LT, strlen(PATTERN_LT)) == 0) { // '<' char
                if (v) fprintf(stderr, "< ");
                parsed[parsed_len] = '<';
                parsed_len ++;
                i += strlen(PATTERN_LT);
                continue;
            }
            if (strncmp(&(raw[i]), PATTERN_GT, strlen(PATTERN_GT)) == 0) { // '>' char
                if (v) fprintf(stderr, "> ");
                parsed[parsed_len] = '>';
                parsed_len ++;
                i += strlen(PATTERN_GT);
                continue;
            }
            if (strncmp(&(raw[i]), PATTERN_SLASH, strlen(PATTERN_SLASH)) == 0) { // '/' char
                if (v) fprintf(stderr, "/ ");
                parsed[parsed_len] = '/';
                parsed_len ++;
                i += strlen(PATTERN_SLASH);
                continue;
            }
            if (strncmp(&(raw[i]), PATTERN_BCKSLASH, strlen(PATTERN_BCKSLASH)) == 0) { // '\' char
                if (v) fprintf(stderr, "\\ ");
                parsed[parsed_len] = '\\';
                parsed_len ++;
                i += strlen(PATTERN_BCKSLASH);
                continue;
            }
            if (strncmp(&(raw[i]), PATTERN_NBSP, strlen(PATTERN_NBSP)) == 0) { // ' ' char
                if (v) fprintf(stderr, "nbsp ");
                parsed[parsed_len] = ' ';
                parsed_len ++;
                i += strlen(PATTERN_NBSP);
                continue;
            }
            if (strncmp(&(raw[i]), PATTERN_SINGLE_QUOT, strlen(PATTERN_SINGLE_QUOT)) == 0) { // '\'' char
                if (v) fprintf(stderr, "\' ");
                parsed[parsed_len] = '\'';
                parsed_len ++;
                i += strlen(PATTERN_SINGLE_QUOT);
                continue;
            }
            if (strncmp(&(raw[i]), PATTERN_DOUBLE_QUOT, strlen(PATTERN_DOUBLE_QUOT)) == 0) { // '\"' char
                if (v) fprintf(stderr, "\" ");
                parsed[parsed_len] = '\"';
                parsed_len ++;
                i += strlen(PATTERN_DOUBLE_QUOT);
                continue;
            }
            if (strncmp(&(raw[i]), PATTERN_AMP, strlen(PATTERN_AMP)) == 0) { // '&' char
                if (v) fprintf(stderr, "& ");
                parsed[parsed_len] = '&';
                parsed_len ++;
                i += strlen(PATTERN_AMP);
                continue;
            }
            else { // Ordinary char
                parsed[parsed_len] = raw[i];
                parsed_len ++;
            }
        }
    }
    if (v) fprintf(stderr, "]] Final length: %lld\n", parsed_len);
    if (v) fprintf(stderr, "]] Exiting parseHTML\n");
    return parsed;
}

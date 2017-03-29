#pragma once
enum makaba_error {
    ERR_MEMORY,
    ERR_UNKNOWN,
    ERR_CURL_INIT,
    ERR_CURL_PERFORM,
    ERR_MAKABA_SETUP,
    ERR_ARGS,
    ERR_PARTTHREAD_DEPTH,
    ERR_POST_FORMAT,
    ERR_COMMENT_FORMAT,
    ERR_REF_FORMAT,
    ERR_COMMENT_PARSING,
    ERR_CAPTCHA_FORMAT,
    ERR_INTERNAL,
    ERR_POST_OUT_OF_RANGE,
    ERR_JSON_INIT,
    ERR_JSON_PARSE
};
typedef enum makaba_error t_makaba_error;

t_makaba_error makaba_errno;

// ========================================
// File: error.h
// Global error codes
// ========================================

#include <cstdio>

#pragma once
enum makaba_error {
    ERR_MEMORY,
    ERR_UNKNOWN, //
    ERR_CURL_INIT, //
    ERR_CURL_PERFORM, //
    ERR_MAKABA_SETUP,
    ERR_ARGS,
    ERR_POST_FORMAT,
    ERR_REF_FORMAT,
    ERR_GENERAL_FORMAT,
    ERR_INTERNAL,
    ERR_JSON_INIT,
    ERR_JSON_PARSE,
    ERR_CACHE_NOENT,
    ERR_CACHE_READ,
    ERR_GETENV
};
typedef enum makaba_error makaba_error_t;

extern makaba_error_t makaba_errno;

const char *makaba_strerror(makaba_error_t err);

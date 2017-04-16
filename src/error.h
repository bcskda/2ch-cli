// ========================================
// File: error.h
// Global error codes
// ========================================

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
    ERR_INTERNAL,
    ERR_JSON_INIT,
    ERR_JSON_PARSE,
    ERR_CACHE_NOENT,
    ERR_CACHE_READ,
    ERR_GETENV
};
typedef enum makaba_error makaba_error_t;

makaba_error_t makaba_errno;

const char *makaba_strerror(makaba_error_t err) {
    fprintf(stderr, "[makaba_strerror] Started with %d\n", err);
    switch (err) {
        case ERR_UNKNOWN:
            return "Неизвестная ошибка";
        case ERR_CURL_INIT:
            return "Внутренняя ошибка cURL";
        case ERR_CURL_PERFORM:
            return "Ошибка соединения с сервером";
        case ERR_MEMORY:
        case ERR_MAKABA_SETUP:
        case ERR_ARGS:
        case ERR_POST_FORMAT:
        case ERR_REF_FORMAT:
        case ERR_INTERNAL:
            return "Внутренняя ошибка";
        case ERR_JSON_INIT:
        case ERR_JSON_PARSE:
            return "Внутренняя ошибка libjson";
        case ERR_CACHE_NOENT:
            return "Ошибка: файл кэша не существует";
        ERR_CACHE_READ:
            return "Ошибка при чтении файла кэша";
        ERR_GETENV:
            return "Ошибка работы со средой";
    }
}

// ========================================
// File: error.cpp
// Library errors
// ========================================

#include "error.h"

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
        case ERR_ARGS:
        case ERR_REF_FORMAT:
        case ERR_INTERNAL:
            return "Внутренняя ошибка";
        case ERR_POST_FORMAT:
            return "Ошибка: некорректный пост после обработки";
        case ERR_GENERAL_FORMAT:
            return "Ошибка в формате ответа сервера";
        case ERR_CACHE_NOENT:
            return "Ошибка: файл кэша не существует";
        case ERR_CACHE_READ:
            return "Ошибка при чтении файла кэша";
        case ERR_GETENV:
            return "Ошибка работы со средой";
    }
    return "Ошибка makaba_strerror()";
}

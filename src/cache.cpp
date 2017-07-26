// ========================================
// File: cache.cpp
// Manage raw JSON cache
// (Implementation)
// ========================================

#include "cache.h"


const size_t Json_cache_buf_size = 2e6;
const size_t Json_cache_dir_size = 200;
const char *Json_cache_suff_armed = "active";
char *Json_cache_buf = NULL;
char *Json_cache_dir = NULL;



int initJsonCache()
{
    if (Env_HOME.length() == 0) {
        std::cerr << "call setup_env()\n" << std::endl;
        setup_env();
    }
    if (Json_cache_dir == NULL)
        Json_cache_dir = (char *) calloc(Json_cache_dir_size, sizeof(char));
    sprintf(Json_cache_dir, "%s/.cache/2ch-cli", Env_HOME.data());
    if (access(Json_cache_dir, F_OK)) {
        if (mkdir(Json_cache_dir, S_IRWXU) == -1) { // Директория -> 0700
            fprintf(stderr, "[initJsonCache]! Error: mkdir() failed: %s\n",
                strerror(errno));
            makaba_errno = ERR_UNKNOWN;
            return -1;
        }
    }
    std::cerr << "Exiting " << __PRETTY_FUNCTION__ << std::endl;
    return 0;
}

bool checkJsonCache(const Makaba::Thread &thread)
{
    char filename[200] = "";
    sprintf(filename, "%s/thread-%s-%lld",
        Json_cache_dir, thread.board.data(), thread.num);
    return access(filename, F_OK) == 0;
}

void armJsonCache(const Makaba::Thread &thread)
{
    char filename_old[70] = "";
    sprintf(filename_old, "%s/thread-%s-%lld",
        Json_cache_dir, thread.board.data(), thread.num);
    char filename_new[70] = "";
    sprintf(filename_new, "%s-%s", filename_old, Json_cache_suff_armed);
    rename(filename_old, filename_new);
}

void disarmJsonCache(const Makaba::Thread &thread)
{
    char filename_new[70] = "";
    sprintf(filename_new, "%s/thread-%s-%lld",
        Json_cache_dir, thread.board.data(), thread.num);
    char filename_old[70] = "";
    sprintf(filename_old, "%s-%s", filename_new, Json_cache_suff_armed);
    rename(filename_old, filename_new);
}

char *readJsonCache(const Makaba::Thread &thread, long long *threadsize)
{
    char filename[70] = "";
    sprintf(filename, "%s/thread-%s-%lld",
        Json_cache_dir, thread.board.data(), thread.num);
    if (strlen(Json_cache_dir) == 0) {
        if (initJsonCache() == -1) {
            fprintf(stderr, "[readJsonCache]! Error: @ initJsonCache()\n");
        }
    }
    if (! checkJsonCache(thread)) {
        fprintf(stderr, "[readJsonCache]! Error: cache file %s doesn`t exist\n",
                filename);
        makaba_errno = ERR_CACHE_NOENT;
        return NULL;
    }

    FILE *fd = fopen(filename, "r");
    fseek(fd, 0, SEEK_END);
    *threadsize = ftell(fd);
    if (Json_cache_buf == NULL) {
        Json_cache_buf = (char *) calloc(Json_cache_buf_size, sizeof(char));
    }

    Json_cache_buf[0] = '[';
    fseek(fd, 0, SEEK_SET);
    fread(Json_cache_buf + 1, sizeof(char), *threadsize, fd);
    Json_cache_buf[*threadsize + 1] = ']';
    if (ferror(fd)) {
        fprintf(stderr, "[readJsonCache]! Error while reading cache file: %d\n",
                ferror(fd));
        fclose(fd);
        makaba_errno = ERR_CACHE_READ;
        return NULL;
    }
    fclose(fd);

    return Json_cache_buf;
}

int writeJsonCache(const Makaba::Thread &thread, const char *thread_ch)
{
    if (strlen(Json_cache_dir) == 0) {
        if (initJsonCache() == -1) {
            fprintf(stderr, "[writeJsonCache]! Error: @ initJsonCache()\n");
        }
    }
    if (strlen(thread_ch) <= 2)
        return 0;

    char filename[70] = "";
    sprintf(filename, "%s/thread-%s-%lld-%s",
        Json_cache_dir, thread.board.data(), thread.num, Json_cache_suff_armed);

    FILE *fd = fopen(filename, "a+");
    long long fsize = 0;
    fseek(fd, 0, SEEK_END);
    fsize = ftell(fd);
    if (fsize > 0) {
        fputc(',', fd);
    }
    fwrite(thread_ch + 1, sizeof(char), strlen(thread_ch) - 2, fd);
    fclose(fd);
    return 0;
}

int cleanJsonCache() {
    if (strlen(Json_cache_dir) == 0) {
        if (initJsonCache() == -1) {
            fprintf(stderr, "[cleanJsonCache]! Error: @ initJsonCache()\n");
        }
    }

    if (chdir(Json_cache_dir)) {
        switch(errno) {
            case ENOENT:
                fprintf(stderr, "[cleanJsonCache]! Error: cache directory %s doesn`t exist after initJsonCache()\n",
                        Json_cache_dir);
                return 0;
            default:
                fprintf(stderr, "[cleanJsonCache]! Error: can`t chdir() to %s: %s\n",
                    Json_cache_dir, strerror(errno));
                return -1;
        }
    }

    DIR *dir;
    struct dirent *entry;
    dir = opendir("./");

    if (dir != NULL) {
        for (int i = 0; (entry = readdir(dir)) != 0; i++) {
            if (i > 1) {
                if (! strstr(entry->d_name, Json_cache_suff_armed)) {
                    fprintf(stderr, "[cleanJsonCache] Delete %s/%s\n",
                        Json_cache_dir, entry->d_name);
                    remove(entry->d_name);
                }
            }
        }
    closedir(dir);
    }
    else {
        fprintf(stderr, "[cleanJsonCache]! Error: Can`t open cache directory: %s\n",
            Json_cache_dir);
        return -1;
    }

    free(Json_cache_buf);
    free(Json_cache_dir);

    return 0;
}

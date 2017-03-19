// ========================================
// File: image.c
// Image processing functions
// (Implementation)
// ========================================

#include "image.h"

void convert_img(const char *filename, const char *ofile, const bool v) {
    fprintf (stderr, "]] Starting convert_img\n");
    /*
     * Форкаем и вызываем img2txt из caca-utils,
     * чтобы конвертировать в удобный для libcaca формат
     */
    if (v) fprintf(stderr, "(verbose)\n");
    pid_t pid = fork();
    if (pid) {
        if (v) fprintf(stderr, "[convert_img][parent] Child PID %d\n", pid);
        waitpid(0, NULL, 0);
        if (v) fprintf(stderr, "[convert_img][parent] Child exited\n");
    }
    else {
        // @TODO mktemp()
        freopen(ofile, "w", stdout);
        char *converter_args[] = {
            Converter,
            "-W",
            Converter_width_s,
            "-H",
            Converter_height_s,
            "-f",
            Converter_format,
            filename,
            NULL
        };
        if (v)  {
            fprintf(stderr, "[convert_img][child ] Started\n");
            fprintf(stderr, "[convert_img][child ] Exec returned %d\n", perf_exec(converter_args));
        }
        else {
            perf_exec(converter_args);
        }
    }
    fprintf(stderr, "]] Exiting convert_img\n");
}

int perf_exec(const char *args) {
    return execvp(Converter, args);
}

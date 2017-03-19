// ========================================
// File: image.c
// Image processing functions
// (Implementation)
// ========================================

#include "image.h"

int show_img(const char *filename) {
    fprintf(stderr, "]] Starting show_img\n");

    caca_canvas_t *canvas;
    caca_display_t *display;
    // Подразумевается, что на данном этапе уже есть сессия ncurses
    canvas = caca_create_canvas(0, 0);
    if (! canvas) {
        fprintf(stderr, "[show_img]! Error: could not open libcaca canvas\n");
        return Ret_show_create_canvas;
    }
    ssize_t import_ret = caca_import_canvas_from_file(canvas, filename, "");
    if (import_ret == NULL) {
        fprintf(stderr, "[show_img]! Error: could not import canvas from %s\n", filename);
        return Ret_show_import_canvas;
    }
    display = caca_create_display(canvas);
    if (! display) {
        fprintf(stderr, "[show_img]! Error: could not open libcaca display\n");
        return Ret_show_create_display;
    }

    caca_refresh_display(display);

    caca_event_t event;
    caca_get_event(display, CACA_EVENT_KEY_PRESS, &event, -1);

    caca_free_display(display);
    fprintf(stderr, "]] Exiting show_img\n");
    return Ret_show_OK;
}

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
        const char *converter_args[] = {
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

#include "external.h"

void setup_env() {
	if (Env_EDITOR.length() == 0)
		Env_EDITOR = getenv("EDITOR");
	if (Env_EDITOR.length() == 0)
		Env_EDITOR = DEFAULT_EDITOR; // Макрос в makefile
	if (Env_HOME.length() == 0)
		Env_HOME = getenv("HOME");
	std::cerr << "[after init] btw HOME = \"" << Env_HOME.data() << std::endl;
	if (Comment_tmpfile.length() == 0) {
		Comment_tmpfile = Env_HOME;
		Comment_tmpfile += "/.cache/2ch-cli/comment";
    } 
}

void fork_and_edit(std::string &dest) {
	if (Env_EDITOR.length() == 0 || Comment_tmpfile.length() == 0)
			setup_env();
	pid_t pid = fork();
	if (pid == 0) { // Потомок
		char shcmd[500];
		sprintf(shcmd, "[ -f %s ] || echo \'%s\' > %s",
				Comment_tmpfile.data(), Comment_tmpmesg.data(), Comment_tmpfile.data());
		system(shcmd);
		execlp(Env_EDITOR.data(), Env_EDITOR.data(), Comment_tmpfile.data(), NULL);
	}
	else { // Родитель
		int status;
		wait(&status);
		if (WIFEXITED(status)) {
			if (WEXITSTATUS(status)) {
				fprintf(stderr, "[fork_and_edit()] [P] Error: child returned %d\n",
						WEXITSTATUS(status));
				return;
			}
			else {
				fprintf(stderr, "[fork_and_edit()] [P] Note: child returned 0\n");
				fprintf(stderr, "[fork_and_edit()] [P] tmpfile = \"%s\"\n",
						Comment_tmpfile.data());
				std::ifstream is(Comment_tmpfile);
				dest = std::string((std::istreambuf_iterator<char>(is)),
								std::istreambuf_iterator<char>());
			}
		}
		fprintf(stderr, "[fork_and_edit()] [P] Error: child didn`t exit\n");
		return;
	}
}

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
    if (import_ret == -1) {
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

int perf_exec(const char **args) {
    return execvp(Converter, (char *const *)args);
}

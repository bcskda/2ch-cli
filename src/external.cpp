#include "external.h"

// Global defs

const char *Converter          = "img2txt";
const char *Converter_format   = "caca";
const char *Converter_width_s  = "140";
const char *Converter_height_s = "35";
const int   Converter_width_i  = 140;
const int   Converter_height_i = 35;

const char *CaptchaUtfFilename = "/tmp/2ch-captcha.utf8";

const int Ret_show_OK = 0;
const int Ret_show_create_canvas = 1;
const int Ret_show_import_canvas = 2;
const int Ret_show_create_display = 3;

const  std::string Comment_tmpmesg = "Здесь будет текст поста";

std::string Comment_tmpfile = "";
std::string Email_tmpfile   = "";
std::string Name_tmpfile    = "";
std::string Trip_tmpfile    = "";

std::string Env_HOME = "";
std::string Env_EDITOR = "";

// End Global defs

void setup_env() {
    std::cerr << "Enter setup_env()" << std::endl;
    char *env = NULL;
    if (Env_EDITOR.length() == 0) {
        env = getenv("EDITOR");
        if (env != NULL)
            Env_EDITOR = env;
    }
    if (Env_EDITOR.length() == 0)
        Env_EDITOR = DEFAULT_EDITOR; // Макрос в makefile
    if (Env_HOME.length() == 0) {
        env = getenv("HOME");
        if (env != NULL)
            Env_HOME = env;
        else {
            printf("Error: HOME not set");
            exit(ERR_GETENV);
        }
    }
    std::cerr << "[[ " << __PRETTY_FUNCTION__ << "]] HOME = \"" << Env_HOME.data() << "\"\n";
    if (Comment_tmpfile.length() == 0) {
        Comment_tmpfile = Env_HOME;
        Comment_tmpfile += "/.cache/2ch-cli/comment";
    }
    if (Email_tmpfile.length() == 0) {
        Email_tmpfile = Env_HOME;
        Email_tmpfile += "/.cache/2ch-cli/email";
    }
    if (Name_tmpfile.length() == 0) {
        Name_tmpfile = Env_HOME;
        Name_tmpfile += "/.cache/2ch-cli/name";
    }
    if (Trip_tmpfile.length() == 0) {
        Trip_tmpfile = Env_HOME;
        Trip_tmpfile += "/.cache/2ch-cli/trip";
    }
}

void edit(std::string &dest, const enum edit_task task) {
    char shcmd[500];
    switch(task) {
        case Task_comment:
            sprintf(shcmd, "[ -f %s ] || echo \'%s\' > %s",
                Comment_tmpfile.data(), Comment_tmpmesg.data(), Comment_tmpfile.data());
            fork_and_edit(dest, Comment_tmpfile);
            break;
        case Task_email:
            fork_and_edit(dest, Email_tmpfile);
            break;
        case Task_name:
            fork_and_edit(dest, Name_tmpfile);
            break;
        case Task_trip:
            fork_and_edit(dest, Trip_tmpfile);
            break;
    }
    // '\n' из-за EOF:
    if (dest.back() == '\n')
        dest.erase(dest.end() - 1);
}

void read(std::string &dest, const enum edit_task task) {
    std::ifstream is;
    switch(task) {
        case Task_comment:
            if (Comment_tmpfile.length() == 0)
                setup_env();
            is = std::ifstream(Comment_tmpfile);
            break;
        case Task_email:
            if (Email_tmpfile.length() == 0)
                setup_env();
            is = std::ifstream(Email_tmpfile);
            break;
        case Task_name:
            if (Name_tmpfile.length() == 0)
                setup_env();
            is = std::ifstream(Name_tmpfile);
            break;
        case Task_trip:
            if (Trip_tmpfile.length() == 0)
                setup_env();
            is = std::ifstream(Trip_tmpfile);
            break;
    }
    dest = std::string((std::istreambuf_iterator<char>(is)),
                        std::istreambuf_iterator<char>());
}

void fork_and_edit(std::string &dest, const std::string &filename) {
    if (Env_EDITOR.length() == 0)
        setup_env();
    pid_t pid = fork();
    if (pid == 0) { // Потомок
            execlp(Env_EDITOR.data(), Env_EDITOR.data(), filename.data(), NULL);
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
                        filename.data());
                std::ifstream is(filename);
                dest = std::string((std::istreambuf_iterator<char>(is)),
                                std::istreambuf_iterator<char>());
                return;
            }
        }
        fprintf(stderr, "[fork_and_edit()] [P] Error: child didn`t exit\n");
        return;
    }
}

caca_display_t *show_img(const char *filename) {
    fprintf(stderr, "]] Starting show_img\n");

    caca_canvas_t *canvas;
    caca_display_t *display;
    // Подразумевается, что на данном этапе уже есть сессия ncurses
    canvas = caca_create_canvas(0, 0);
    if (! canvas) {
        fprintf(stderr, "[show_img]! Error: could not open libcaca canvas\n");
        return NULL;
    }
    ssize_t import_ret = caca_import_canvas_from_file(canvas, filename, "");
    if (import_ret == -1) {
        fprintf(stderr, "[show_img]! Error: could not import canvas from %s\n", filename);
        return NULL;
    }
    display = caca_create_display(canvas);
    if (! display) {
        fprintf(stderr, "[show_img]! Error: could not open libcaca display\n");
        return NULL;
    }

    caca_refresh_display(display);

    fprintf(stderr, "]] Exiting show_img\n");
    return display;
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
        // img2txt выведет всё в stdout, мы перенаправляем в файл
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

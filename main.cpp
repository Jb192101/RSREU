/*
 * minix_xfm_v7.c
 * Simple X11 file manager for Minix 3.4.0 - Version 7.0
 * Added file type detection: [TXT] or [BIN]
 * + resize fix, keyboard navigation
 * + Fixed hang when opening /dev directory
 * + Added keyboard shortcuts
 * + Added process management (close applications)
 * + Fixed help system
 * Pure C version
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#define LINE_HEIGHT 16
#define MARGIN 5
#define DOUBLE_CLICK_DELAY 300  /* milliseconds */
#define MAX_PROCESSES 50

typedef struct Entry {
    char name[256];
    int is_dir;
    char perms[11];
    mode_t mode;
    char type_label[8]; /* [DIR], [TXT], [BIN] */
} Entry;

typedef struct Process {
    pid_t pid;
    char command[256];
    time_t start_time;
} Process;

static Display *dpy;
static Window win;
static GC gc;
static XFontStruct *fontinfo;

static Entry entries[1000];
static int nentries = 0;
static char cwd[1024];
static int selected_idx = -1;
static struct timespec last_click_time = {0};
static int last_click_idx = -1;

static Process processes[MAX_PROCESSES];
static int nprocesses = 0;
static int show_processes = 0; /* 0 = files, 1 = processes */

static int win_w = 700;
static int win_h = 400;
static int scroll_offset = 0;

/* Forward declarations */
static void draw_list(void);
static void read_dir(const char *path);
static void update_process_list(void);
static void show_help(void);

static void mode_to_str(mode_t mode, char *out)
{
    out[0] = S_ISDIR(mode) ? 'd' : '-';
    out[1] = (mode & S_IRUSR) ? 'r' : '-';
    out[2] = (mode & S_IWUSR) ? 'w' : '-';
    out[3] = (mode & S_IXUSR) ? 'x' : '-';
    out[4] = (mode & S_IRGRP) ? 'r' : '-';
    out[5] = (mode & S_IWGRP) ? 'w' : '-';
    out[6] = (mode & S_IXGRP) ? 'x' : '-';
    out[7] = (mode & S_IROTH) ? 'r' : '-';
    out[8] = (mode & S_IWOTH) ? 'w' : '-';
    out[9] = (mode & S_IXOTH) ? 'x' : '-';
    out[10] = '\0';
}

/* Определяем, является ли файл текстовым */
static int is_text_file(const char *path)
{
    struct stat st;
    if (stat(path, &st) != 0) return 0;
    
    /* Не пытаемся читать специальные файлы (устройства, FIFO, сокеты и т.д.) */
    if (!S_ISREG(st.st_mode)) return 0;
    
    FILE *f = fopen(path, "rb");
    if (!f) return 0;
    
    unsigned char buf[256];
    size_t n = fread(buf, 1, sizeof(buf), f);
    fclose(f);
    
    if (n == 0) return 1; /* пустые считаем текстовыми */

    for (size_t i = 0; i < n; i++) {
        unsigned char c = buf[i];
        if (c == 9 || c == 10 || c == 13) continue; /* tab, LF, CR */
        if (c < 32 || c > 126) return 0; /* непечатаемый символ */
    }
    return 1;
}

static void read_dir(const char *path)
{
    DIR *d;
    struct dirent *de;
    struct stat st;
    char full[1024];

    d = opendir(path);
    if (!d) {
        perror("opendir");
        return;
    }

    nentries = 0;
    selected_idx = -1;
    scroll_offset = 0;

    if (strcmp(path, "/") != 0) {
        strcpy(entries[nentries].name, "..");
        entries[nentries].is_dir = 1;
        strcpy(entries[nentries].perms, "drwx------");
        strcpy(entries[nentries].type_label, "[DIR]");
        entries[nentries].mode = S_IFDIR | 0700;
        nentries++;
    }

    while ((de = readdir(d)) != NULL && nentries < 1000) {
        if (strcmp(de->d_name, ".") == 0) continue;
        snprintf(full, sizeof(full), "%s/%s", path, de->d_name);
        if (stat(full, &st) == 0) {
            strncpy(entries[nentries].name, de->d_name, 255);
            entries[nentries].is_dir = S_ISDIR(st.st_mode);
            entries[nentries].mode = st.st_mode;
            mode_to_str(st.st_mode, entries[nentries].perms);

            if (entries[nentries].is_dir) {
                strcpy(entries[nentries].type_label, "[DIR]");
            } else {
                /* Для обычных файлов определяем тип (текст/бинарник) */
                if (S_ISREG(st.st_mode)) {
                    if (is_text_file(full))
                        strcpy(entries[nentries].type_label, "[TXT]");
                    else
                        strcpy(entries[nentries].type_label, "[BIN]");
                } else {
                    /* Для специальных файлов (устройств и т.д.) показываем тип файла */
                    if (S_ISCHR(st.st_mode)) strcpy(entries[nentries].type_label, "[CHR]");
                    else if (S_ISBLK(st.st_mode)) strcpy(entries[nentries].type_label, "[BLK]");
                    else if (S_ISFIFO(st.st_mode)) strcpy(entries[nentries].type_label, "[FIFO]");
                    else if (S_ISLNK(st.st_mode)) strcpy(entries[nentries].type_label, "[LNK]");
                    else if (S_ISSOCK(st.st_mode)) strcpy(entries[nentries].type_label, "[SOCK]");
                    else strcpy(entries[nentries].type_label, "[???]");
                }
            }
            nentries++;
        }
    }
    closedir(d);
}

/* Обновить список процессов */
static void update_process_list(void)
{
    /* Очищаем старый список */
    int i;
    for (i = 0; i < nprocesses; i++) {
        /* Проверяем, жив ли процесс */
        if (kill(processes[i].pid, 0) != 0) {
            /* Процесс завершился, удаляем из списка */
            int j;
            for (j = i; j < nprocesses - 1; j++) {
                processes[j] = processes[j + 1];
            }
            nprocesses--;
            i--;
        }
    }
}

/* Добавить процесс в список */
static void add_process(pid_t pid, const char *command)
{
    if (nprocesses >= MAX_PROCESSES) {
        /* Удаляем самый старый процесс */
        int i;
        for (i = 0; i < nprocesses - 1; i++) {
            processes[i] = processes[i + 1];
        }
        nprocesses--;
    }
    
    processes[nprocesses].pid = pid;
    strncpy(processes[nprocesses].command, command, 255);
    processes[nprocesses].start_time = time(NULL);
    nprocesses++;
}

/* Завершить процесс */
static void kill_process(int idx)
{
    if (idx < 0 || idx >= nprocesses) return;
    
    pid_t pid = processes[idx].pid;
    
    if (kill(pid, SIGTERM) == 0) {
        printf("Sent SIGTERM to process %d (%s)\n", pid, processes[idx].command);
        
        /* Ждем немного для завершения */
        sleep(1);
        
        /* Если процесс еще жив, посылаем SIGKILL */
        if (kill(pid, 0) == 0) {
            kill(pid, SIGKILL);
            printf("Sent SIGKILL to process %d\n", pid);
        }
        
        /* Удаляем процесс из списка */
        int i;
        for (i = idx; i < nprocesses - 1; i++) {
            processes[i] = processes[i + 1];
        }
        nprocesses--;
        
        if (show_processes) {
            update_process_list();
            draw_list();
        }
    } else {
        perror("kill");
    }
}

static void draw_list(void)
{
    XClearWindow(dpy, win);
    
    /* Отображаем текущий режим в заголовке */
    char title[1200];
    if (show_processes) {
        snprintf(title, sizeof(title), "Minix FM v7.0 - Running Processes (%d) - F1: Help", nprocesses);
    } else {
        snprintf(title, sizeof(title), "Minix FM v7.0 - %s - F1: Help", cwd);
    }
    XStoreName(dpy, win, title);
    
    int visible_lines = win_h / LINE_HEIGHT;
    int i;
    
    /* Рисуем заголовок */
    if (show_processes) {
        XDrawString(dpy, win, gc, MARGIN, MARGIN + fontinfo->ascent, 
                   "PID        Command", 18);
    } else {
        XDrawString(dpy, win, gc, MARGIN, MARGIN + fontinfo->ascent, 
                   "Permissions Type    Name", 25);
    }
    
    /* Рисуем разделитель */
    XDrawLine(dpy, win, gc, 0, MARGIN + LINE_HEIGHT, win_w, MARGIN + LINE_HEIGHT);
    
    for (i = 0; i < visible_lines; i++) {
        int entry_idx = i + scroll_offset;
        int y = MARGIN + (i + 1) * LINE_HEIGHT + fontinfo->ascent;
        
        if (show_processes) {
            if (entry_idx >= nprocesses) break;
            
            char display[400];
            char time_str[20];
            time_t now = time(NULL);
            int seconds = now - processes[entry_idx].start_time;
            snprintf(time_str, sizeof(time_str), "%02d:%02d", seconds / 60, seconds % 60);
            
            snprintf(display, sizeof(display), "%-8d [%s] %s", 
                    processes[entry_idx].pid, time_str, processes[entry_idx].command);

            if (entry_idx == selected_idx) {
                XSetForeground(dpy, gc, 0xFFC0C0); /* Красноватый для процессов */
                XFillRectangle(dpy, win, gc,
                               0, MARGIN + (i + 1) * LINE_HEIGHT,
                               win_w, LINE_HEIGHT);
                XSetForeground(dpy, gc, BlackPixel(dpy, 0));
            }

            XDrawString(dpy, win, gc, MARGIN, y, display, strlen(display));
        } else {
            if (entry_idx >= nentries) break;

            char display[400];
            snprintf(display, sizeof(display), "%-11s %s %s",
                    entries[entry_idx].perms,
                    entries[entry_idx].type_label,
                    entries[entry_idx].name);

            if (entry_idx == selected_idx) {
                XSetForeground(dpy, gc, 0xC0C0C0);
                XFillRectangle(dpy, win, gc,
                               0, MARGIN + (i + 1) * LINE_HEIGHT,
                               win_w, LINE_HEIGHT);
                XSetForeground(dpy, gc, BlackPixel(dpy, 0));
            }

            XDrawString(dpy, win, gc, MARGIN, y, display, strlen(display));
        }
    }
    
    /* Рисуем кнопку переключения режима */
    char mode_button[50];
    if (show_processes) {
        snprintf(mode_button, sizeof(mode_button), "[F9] Files");
    } else {
        snprintf(mode_button, sizeof(mode_button), "[F9] Processes");
    }
    XDrawString(dpy, win, gc, win_w - 100, MARGIN + fontinfo->ascent, 
               mode_button, strlen(mode_button));
}

/* Показать справку в отдельном окне */
static void show_help(void)
{
    int help_pid = fork();
    if (help_pid == 0) {
        /* Создаем временный файл со справкой */
        const char *help_text =
            "=== Minix File Manager v7.0 - Keyboard Shortcuts ===\n\n"
            "Navigation:\n"
            "  Up/Down arrows    - Navigate through files/processes\n"
            "  Page Up/Page Down - Scroll page by page\n"
            "  Home/End          - Jump to first/last item\n"
            "  Enter             - Open selected file/directory\n\n"
            "File Operations:\n"
            "  F5 or R           - Refresh directory\n"
            "  F2 or F           - Open file in editor\n"
            "  Delete or D       - Delete selected file (with confirmation)\n\n"
            "Directory Operations:\n"
            "  Backspace         - Go to parent directory\n"
            "  T                 - Open terminal in current directory\n"
            "  H                 - Go to home directory\n"
            "  /                 - Go to root directory\n\n"
            "Process Management:\n"
            "  F9                - Toggle files/processes view\n"
            "  K                 - Kill selected process\n"
            "  F8                - Refresh process list\n\n"
            "Application:\n"
            "  F1 or ?           - Show this help\n"
            "  Q or Ctrl+Q       - Quit application\n"
            "  Ctrl+L            - Refresh display\n\n"
            "Press any key to close this window...";
        
        /* Создаем временный файл */
        char temp_file[] = "/tmp/minixfm_help_XXXXXX";
        int fd = mkstemp(temp_file);
        if (fd != -1) {
            write(fd, help_text, strlen(help_text));
            close(fd);
            
            /* Открываем в xterm с cat для просмотра */
            execlp("xterm", "xterm", "-title", "Minix FM Help", 
                   "-geometry", "80x25", "-e", "cat", temp_file, NULL);
            
            /* Если execlp не сработал, удаляем временный файл */
            unlink(temp_file);
        }
        exit(1);
    } else if (help_pid > 0) {
        add_process(help_pid, "xterm (help)");
    }
}

static int has_read_access(const char *path)
{
    return access(path, R_OK) == 0;
}

static void open_entry(int idx)
{
    char newpath[1024];
    if (idx < 0 || idx >= nentries) return;

    if (entries[idx].is_dir) {
        if (strcmp(entries[idx].name, "..") == 0) {
            char *p = strrchr(cwd, '/');
            if (!p || p == cwd)
                strcpy(cwd, "/");
            else
                *p = '\0';
        } else {
            snprintf(newpath, sizeof(newpath), "%s/%s",
                     strcmp(cwd, "/") == 0 ? "" : cwd,
                     entries[idx].name);
            strcpy(cwd, newpath);
        }
        read_dir(cwd);
        draw_list();
    } else {
        char filepath[1024];
        snprintf(filepath, sizeof(filepath), "%s/%s",
                 strcmp(cwd, "/") == 0 ? "" : cwd,
                 entries[idx].name);

        if (!has_read_access(filepath)) {
            fprintf(stderr, "Permission denied: %s\n", filepath);
            return;
        }

        /* Проверяем, является ли файл обычным файлом (не устройством и т.д.) */
        struct stat st;
        if (stat(filepath, &st) == 0 && S_ISREG(st.st_mode)) {
            int pid = fork();
            if (pid == 0) {
                execlp("xterm", "xterm", "-e", "vi", filepath, NULL);
                exit(1);
            } else if (pid > 0) {
                add_process(pid, "xterm (vi editor)");
            }
        } else {
            fprintf(stderr, "Cannot open non-regular file: %s\n", filepath);
        }
    }
}

static void open_terminal(void)
{
    int pid = fork();
    if (pid == 0) {
        chdir(cwd);
        execlp("xterm", "xterm", NULL);
        exit(1);
    } else if (pid > 0) {
        add_process(pid, "xterm");
    }
}

static int confirm_dialog(const char *message)
{
    /* Простой диалог подтверждения в консоли */
    printf("%s (y/N): ", message);
    fflush(stdout);
    
    char response[10];
    if (fgets(response, sizeof(response), stdin)) {
        return (response[0] == 'y' || response[0] == 'Y');
    }
    return 0;
}

static void delete_selected(void)
{
    if (selected_idx < 0 || selected_idx >= nentries) return;
    
    char filepath[1024];
    snprintf(filepath, sizeof(filepath), "%s/%s",
             strcmp(cwd, "/") == 0 ? "" : cwd,
             entries[selected_idx].name);
    
    if (entries[selected_idx].is_dir) {
        if (!confirm_dialog("Delete directory? This cannot be undone!")) return;
        if (rmdir(filepath) == 0) {
            read_dir(cwd);
            draw_list();
        } else {
            perror("rmdir");
        }
    } else {
        if (!confirm_dialog("Delete file? This cannot be undone!")) return;
        if (unlink(filepath) == 0) {
            read_dir(cwd);
            draw_list();
        } else {
            perror("unlink");
        }
    }
}

static void go_home(void)
{
    char *home = getenv("HOME");
    if (home) {
        strcpy(cwd, home);
        read_dir(cwd);
        draw_list();
    }
}

static long diff_ms(struct timespec a, struct timespec b)
{
    return (a.tv_sec - b.tv_sec) * 1000 + (a.tv_nsec - b.tv_nsec) / 1000000;
}

static void handle_click(int y)
{
    int visible_lines = win_h / LINE_HEIGHT;
    int idx = (y - MARGIN - LINE_HEIGHT) / LINE_HEIGHT + scroll_offset;
    if (idx < 0) return;
    
    /* Проверяем, не кликнули ли на кнопку переключения режима */
    if (y < MARGIN + LINE_HEIGHT && y > MARGIN) {
        if (win_w - 120 < win_w) { /* Упрощенная проверка координат */
            show_processes = !show_processes;
            selected_idx = -1;
            scroll_offset = 0;
            draw_list();
            return;
        }
    }

    if (show_processes) {
        if (idx >= nprocesses) return;
    } else {
        if (idx >= nentries) return;
    }

    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    long diff = diff_ms(now, last_click_time);

    selected_idx = idx;
    draw_list();

    if (idx == last_click_idx && diff < DOUBLE_CLICK_DELAY) {
        if (!show_processes) {
            open_entry(idx);
        }
        last_click_idx = -1;
    } else {
        last_click_idx = idx;
        last_click_time = now;
    }
}

static void scroll_up(void) {
    if (scroll_offset > 0) scroll_offset--;
    draw_list();
}

static void scroll_down(void) {
    int visible_lines = win_h / LINE_HEIGHT;
    if (show_processes) {
        if (scroll_offset + visible_lines < nprocesses)
            scroll_offset++;
    } else {
        if (scroll_offset + visible_lines < nentries)
            scroll_offset++;
    }
    draw_list();
}

static void scroll_page_up(void) {
    int visible_lines = win_h / LINE_HEIGHT;
    scroll_offset -= visible_lines;
    if (scroll_offset < 0) scroll_offset = 0;
    draw_list();
}

static void scroll_page_down(void) {
    int visible_lines = win_h / LINE_HEIGHT;
    scroll_offset += visible_lines;
    if (show_processes) {
        if (scroll_offset + visible_lines > nprocesses)
            scroll_offset = nprocesses - visible_lines;
    } else {
        if (scroll_offset + visible_lines > nentries)
            scroll_offset = nentries - visible_lines;
    }
    if (scroll_offset < 0) scroll_offset = 0;
    draw_list();
}

static void scroll_to_top(void) {
    scroll_offset = 0;
    selected_idx = 0;
    draw_list();
}

static void scroll_to_bottom(void) {
    int visible_lines = win_h / LINE_HEIGHT;
    if (show_processes) {
        selected_idx = nprocesses - 1;
        scroll_offset = nprocesses - visible_lines;
    } else {
        selected_idx = nentries - 1;
        scroll_offset = nentries - visible_lines;
    }
    if (scroll_offset < 0) scroll_offset = 0;
    draw_list();
}

static void toggle_process_view(void)
{
    show_processes = !show_processes;
    selected_idx = -1;
    scroll_offset = 0;
    if (show_processes) {
        update_process_list();
    }
    draw_list();
}

static void handle_key(XKeyEvent *key)
{
    KeySym ks = XLookupKeysym(key, 0);
    int state = key->state;
    
    /* Проверяем Ctrl комбинации */
    int ctrl_pressed = (state & ControlMask);
    
    switch (ks) {
        /* Навигация */
        case XK_Up:
            if (selected_idx > 0) selected_idx--;
            if (selected_idx < scroll_offset)
                scroll_offset = selected_idx;
            draw_list();
            break;

        case XK_Down: {
            int visible_lines = win_h / LINE_HEIGHT;
            int max_idx;
            if (show_processes) {
                max_idx = nprocesses - 1;
            } else {
                max_idx = nentries - 1;
            }
            
            if (selected_idx < max_idx) selected_idx++;
            if (selected_idx >= scroll_offset + visible_lines)
                scroll_offset++;
            draw_list();
            break;
        }

        case XK_Page_Up:
            scroll_page_up();
            break;

        case XK_Page_Down:
            scroll_page_down();
            break;
            
        case XK_Home:
            scroll_to_top();
            break;
            
        case XK_End:
            scroll_to_bottom();
            break;

        /* Основные операции */
        case XK_Return:
            if (selected_idx >= 0 && !show_processes)
                open_entry(selected_idx);
            break;
            
        case XK_BackSpace:
            if (!show_processes) {
                /* Имитируем клик на ".." */
                int i;
                for (i = 0; i < nentries; i++) {
                    if (strcmp(entries[i].name, "..") == 0) {
                        open_entry(i);
                        break;
                    }
                }
            }
            break;

        /* Функциональные клавиши */
        case XK_F1:
        case XK_question:
            show_help();
            break;
            
        case XK_F2:
        case XK_f:
        case XK_F:
            if (selected_idx >= 0 && !show_processes && !entries[selected_idx].is_dir)
                open_entry(selected_idx);
            break;
            
        case XK_F5:
        case XK_r:
        case XK_R:
            if (show_processes) {
                update_process_list();
            } else {
                read_dir(cwd);
            }
            draw_list();
            break;
            
        case XK_F8:
            update_process_list();
            draw_list();
            break;
            
        case XK_F9:
            toggle_process_view();
            break;
            
        case XK_Delete:
        case XK_d:
        case XK_D:
            if (!show_processes) {
                delete_selected();
            }
            break;
            
        case XK_k:
        case XK_K:
            if (show_processes && selected_idx >= 0) {
                kill_process(selected_idx);
            }
            break;

        /* Директории */
        case XK_t:
        case XK_T:
            if (!show_processes) {
                open_terminal();
            }
            break;
            
        case XK_h:
        case XK_H:
            if (!show_processes) {
                go_home();
            }
            break;
            
        case XK_slash:
            if (!show_processes) {
                strcpy(cwd, "/");
                read_dir(cwd);
                draw_list();
            }
            break;

        /* Приложение */
        case XK_q:
        case XK_Q:
            if (ctrl_pressed) {
                printf("Goodbye!\n");
                exit(0);
            }
            break;
            
        case XK_l:
        case XK_L:
            if (ctrl_pressed) {
                draw_list(); /* Ctrl+L - refresh display */
            }
            break;
    }
}

int main(int argc, char **argv)
{
    printf("Minix File Manager v7.0 started\n");
    printf("Press F1 or ? for keyboard shortcuts\n");
    
    /* Инициализация списка процессов */
    nprocesses = 0;
    show_processes = 0;
    
    if (getcwd(cwd, sizeof(cwd)) == NULL)
        strcpy(cwd, "/");

    dpy = XOpenDisplay(NULL);
    if (!dpy) {
        fprintf(stderr, "Can't open display\n");
        return 1;
    }

    int screen = DefaultScreen(dpy);
    win = XCreateSimpleWindow(dpy, RootWindow(dpy, screen),
                              0, 0, win_w, win_h, 1,
                              BlackPixel(dpy, screen), WhitePixel(dpy, screen));

    XSizeHints hints;
    hints.flags = PSize | PMinSize | PMaxSize;
    hints.min_width = 300;
    hints.min_height = 200;
    hints.max_width = 2000;
    hints.max_height = 1200;
    XSetNormalHints(dpy, win, &hints);

    XSelectInput(dpy, win, ExposureMask | ButtonPressMask |
                 StructureNotifyMask | KeyPressMask);
    XStoreName(dpy, win, "Minix FM v7.0 (Press F1 for help)");
    XMapWindow(dpy, win);

    fontinfo = XLoadQueryFont(dpy, "fixed");
    if (!fontinfo) fontinfo = XLoadQueryFont(dpy, "6x13");
    gc = XCreateGC(dpy, win, 0, NULL);
    if (fontinfo) XSetFont(dpy, gc, fontinfo->fid);

    read_dir(cwd);

    XEvent ev;
    while (1) {
        XNextEvent(dpy, &ev);
        if (ev.type == Expose)
            draw_list();
        else if (ev.type == ButtonPress) {
            if (ev.xbutton.button == Button4)
                scroll_up();
            else if (ev.xbutton.button == Button5)
                scroll_down();
            else
                handle_click(ev.xbutton.y);
        }
        else if (ev.type == KeyPress)
            handle_key(&ev.xkey);
        else if (ev.type == ConfigureNotify) {
            win_w = ev.xconfigure.width;
            win_h = ev.xconfigure.height;
            draw_list();
        }
    }

    return 0;
}
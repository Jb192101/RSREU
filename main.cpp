/*
 * minix_xfm_v8.c
 * Two-panel X11 file manager for Minix 3.4.0 - Version 8.0
 * Dual panel file manager with split screen
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

#define LINE_HEIGHT 20  /* Увеличенная высота строки */
#define MARGIN 8        /* Увеличенные отступы */
#define DOUBLE_CLICK_DELAY 300
#define MAX_PROCESSES 50
#define PANEL_SEPARATOR_WIDTH 4

typedef struct Entry {
    char name[256];
    int is_dir;
    char perms[11];
    mode_t mode;
    char type_label[8];
} Entry;

typedef struct Process {
    pid_t pid;
    char command[256];
    time_t start_time;
} Process;

typedef struct Panel {
    Entry entries[1000];
    int nentries;
    char cwd[1024];
    int selected_idx;
    int scroll_offset;
    int has_focus;
} Panel;

static Display *dpy;
static Window win;
static GC gc;
static XFontStruct *fontinfo;

static Panel left_panel, right_panel;
static Panel *active_panel;
static struct timespec last_click_time = {0};
static int last_click_idx = -1;
static int last_click_panel = 0; // 0 = left, 1 = right

static Process processes[MAX_PROCESSES];
static int nprocesses = 0;
static int show_processes = 0;

static int win_w = 1200;  /* Увеличенное разрешение */
static int win_h = 700;   /* Увеличенное разрешение */
static int separator_x;

/* Forward declarations */
static void draw_interface(void);
static void read_dir(Panel *panel, const char *path);
static void update_process_list(void);
static void show_help(void);
static void init_panel(Panel *panel, const char *initial_path);
static int show_copy_move_dialog(const char *filename, char *new_name, int *operation);
static int rename_file_dialog(const char *old_name, char *new_name);

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

static int is_text_file(const char *path)
{
    struct stat st;
    if (stat(path, &st) != 0) return 0;
    if (!S_ISREG(st.st_mode)) return 0;
    
    FILE *f = fopen(path, "rb");
    if (!f) return 0;
    
    unsigned char buf[256];
    size_t n = fread(buf, 1, sizeof(buf), f);
    fclose(f);
    
    if (n == 0) return 1;

    for (size_t i = 0; i < n; i++) {
        unsigned char c = buf[i];
        if (c == 9 || c == 10 || c == 13) continue;
        if (c < 32 || c > 126) return 0;
    }
    return 1;
}

static void init_panel(Panel *panel, const char *initial_path)
{
    panel->nentries = 0;
    panel->selected_idx = -1;
    panel->scroll_offset = 0;
    panel->has_focus = 0;
    strcpy(panel->cwd, initial_path);
    read_dir(panel, initial_path);
}

static void read_dir(Panel *panel, const char *path)
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

    panel->nentries = 0;
    panel->selected_idx = -1;
    panel->scroll_offset = 0;

    if (strcmp(path, "/") != 0) {
        strcpy(panel->entries[panel->nentries].name, "..");
        panel->entries[panel->nentries].is_dir = 1;
        strcpy(panel->entries[panel->nentries].perms, "drwx------");
        strcpy(panel->entries[panel->nentries].type_label, "[DIR]");
        panel->entries[panel->nentries].mode = S_IFDIR | 0700;
        panel->nentries++;
    }

    while ((de = readdir(d)) != NULL && panel->nentries < 1000) {
        if (strcmp(de->d_name, ".") == 0) continue;
        snprintf(full, sizeof(full), "%s/%s", path, de->d_name);
        if (stat(full, &st) == 0) {
            Entry *entry = &panel->entries[panel->nentries];
            strncpy(entry->name, de->d_name, 255);
            entry->is_dir = S_ISDIR(st.st_mode);
            entry->mode = st.st_mode;
            mode_to_str(st.st_mode, entry->perms);

            if (entry->is_dir) {
                strcpy(entry->type_label, "[DIR]");
            } else {
                if (S_ISREG(st.st_mode)) {
                    if (is_text_file(full))
                        strcpy(entry->type_label, "[TXT]");
                    else
                        strcpy(entry->type_label, "[BIN]");
                } else {
                    if (S_ISCHR(st.st_mode)) strcpy(entry->type_label, "[CHR]");
                    else if (S_ISBLK(st.st_mode)) strcpy(entry->type_label, "[BLK]");
                    else if (S_ISFIFO(st.st_mode)) strcpy(entry->type_label, "[FIFO]");
                    else if (S_ISLNK(st.st_mode)) strcpy(entry->type_label, "[LNK]");
                    else if (S_ISSOCK(st.st_mode)) strcpy(entry->type_label, "[SOCK]");
                    else strcpy(entry->type_label, "[???]");
                }
            }
            panel->nentries++;
        }
    }
    closedir(d);
}

static void update_process_list(void)
{
    int i;
    for (i = 0; i < nprocesses; i++) {
        if (kill(processes[i].pid, 0) != 0) {
            int j;
            for (j = i; j < nprocesses - 1; j++) {
                processes[j] = processes[j + 1];
            }
            nprocesses--;
            i--;
        }
    }
}

static void add_process(pid_t pid, const char *command)
{
    if (nprocesses >= MAX_PROCESSES) {
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

static void kill_process(int idx)
{
    if (idx < 0 || idx >= nprocesses) return;
    
    pid_t pid = processes[idx].pid;
    
    if (kill(pid, SIGTERM) == 0) {
        printf("Sent SIGTERM to process %d (%s)\n", pid, processes[idx].command);
        sleep(1);
        
        if (kill(pid, 0) == 0) {
            kill(pid, SIGKILL);
            printf("Sent SIGKILL to process %d\n", pid);
        }
        
        int i;
        for (i = idx; i < nprocesses - 1; i++) {
            processes[i] = processes[i + 1];
        }
        nprocesses--;
        
        if (show_processes) {
            update_process_list();
            draw_interface();
        }
    } else {
        perror("kill");
    }
}

/* Функция для показа диалога копирования/перемещения */
static int show_copy_move_dialog(const char *filename, char *new_name, int *operation)
{
    char temp_file[] = "/tmp/minixfm_dialog_XXXXXX";
    int fd = mkstemp(temp_file);
    if (fd == -1) return 0;
    
    char dialog_script[1024];
    snprintf(dialog_script, sizeof(dialog_script),
        "#!/bin/sh\n"
        "xterm -title 'File Operation' -geometry 60x10 -e sh -c \""
        "echo 'File: %s';"
        "echo 'Choose operation:';"
        "echo '1 - Copy to other panel';"
        "echo '2 - Move to other panel';"
        "echo '3 - Cancel';"
        "echo -n 'Enter choice (1-3): ';"
        "read choice;"
        "if [ \\\"$choice\\\" = \\\"1\\\" ]; then"
        "  echo -n 'Enter new filename (or press Enter for same name): ';"
        "  read newname;"
        "  echo 'COPY' > %s;"
        "  echo \\\"$newname\\\" >> %s;"
        "elif [ \\\"$choice\\\" = \\\"2\\\" ]; then"
        "  echo -n 'Enter new filename (or press Enter for same name): ';"
        "  read newname;"
        "  echo 'MOVE' > %s;"
        "  echo \\\"$newname\\\" >> %s;"
        "else"
        "  echo 'CANCEL' > %s;"
        "fi"
        "\"",
        filename, temp_file, temp_file, temp_file, temp_file, temp_file);
    
    int result = system(dialog_script);
    if (result == -1) {
        close(fd);
        unlink(temp_file);
        return 0;
    }
    
    FILE *f = fopen(temp_file, "r");
    if (!f) {
        close(fd);
        unlink(temp_file);
        return 0;
    }
    
    char op[10];
    if (fgets(op, sizeof(op), f)) {
        op[strcspn(op, "\n")] = 0;
        if (strcmp(op, "COPY") == 0) {
            *operation = 1;
        } else if (strcmp(op, "MOVE") == 0) {
            *operation = 2;
        } else {
            fclose(f);
            unlink(temp_file);
            return 0;
        }
        
        if (fgets(new_name, 256, f)) {
            new_name[strcspn(new_name, "\n")] = 0;
        } else {
            strcpy(new_name, "");
        }
        
        fclose(f);
        unlink(temp_file);
        return 1;
    }
    
    fclose(f);
    unlink(temp_file);
    return 0;
}

/* Функция для переименования файла при перемещении */
static int rename_file_dialog(const char *old_name, char *new_name)
{
    char temp_file[] = "/tmp/minixfm_rename_XXXXXX";
    int fd = mkstemp(temp_file);
    if (fd == -1) return 0;
    
    char dialog_script[512];
    snprintf(dialog_script, sizeof(dialog_script),
        "#!/bin/sh\n"
        "xterm -title 'Rename File' -geometry 60x8 -e sh -c \""
        "echo 'Moving file: %s';"
        "echo -n 'Enter new filename (or press Enter to keep same name): ';"
        "read newname;"
        "echo \\\"$newname\\\" > %s"
        "\"",
        old_name, temp_file);
    
    int result = system(dialog_script);
    if (result == -1) {
        close(fd);
        unlink(temp_file);
        return 0;
    }
    
    FILE *f = fopen(temp_file, "r");
    if (!f) {
        close(fd);
        unlink(temp_file);
        return 0;
    }
    
    if (fgets(new_name, 256, f)) {
        new_name[strcspn(new_name, "\n")] = 0;
        fclose(f);
        unlink(temp_file);
        return 1;
    }
    
    fclose(f);
    unlink(temp_file);
    return 0;
}

/* Функция для копирования файла/директории */
static void copy_file_or_dir(const char *src, const char *dst)
{
    pid_t pid = fork();
    if (pid == 0) {
        struct stat st;
        if (stat(src, &st) == 0 && S_ISDIR(st.st_mode)) {
            execlp("cp", "cp", "-r", src, dst, NULL);
        } else {
            execlp("cp", "cp", src, dst, NULL);
        }
        exit(1);
    } else if (pid > 0) {
        waitpid(pid, NULL, 0);
    }
}

/* Функция для перемещения файла/директории */
static void move_file_or_dir(const char *src, const char *dst)
{
    pid_t pid = fork();
    if (pid == 0) {
        execlp("mv", "mv", src, dst, NULL);
        exit(1);
    } else if (pid > 0) {
        waitpid(pid, NULL, 0);
    }
}

static void draw_interface(void)
{
    XClearWindow(dpy, win);
    
    /* Calculate separator position */
    separator_x = win_w / 2;
    int panel_width = separator_x - PANEL_SEPARATOR_WIDTH / 2;
    
    /* Draw title */
    char title[1200];
    if (show_processes) {
        snprintf(title, sizeof(title), "Minix FM v8.0 - Dual Panel - Running Processes (%d) - F1: Help", nprocesses);
    } else {
        snprintf(title, sizeof(title), "Minix FM v8.0 - Dual Panel - %s - F1: Help", 
                active_panel == &left_panel ? "LEFT" : "RIGHT");
    }
    XStoreName(dpy, win, title);
    
    if (show_processes) {
        /* Draw processes view */
        XDrawString(dpy, win, gc, MARGIN, MARGIN + fontinfo->ascent, 
                   "PID        Command", 18);
        XDrawLine(dpy, win, gc, 0, MARGIN + LINE_HEIGHT, win_w, MARGIN + LINE_HEIGHT);
        
        int visible_lines = (win_h - LINE_HEIGHT - MARGIN) / LINE_HEIGHT;
        int i;
        for (i = 0; i < visible_lines; i++) {
            int entry_idx = i + (active_panel == &left_panel ? left_panel.scroll_offset : right_panel.scroll_offset);
            if (entry_idx >= nprocesses) break;
            
            int y = MARGIN + (i + 1) * LINE_HEIGHT + fontinfo->ascent;
            char display[400];
            char time_str[20];
            time_t now = time(NULL);
            int seconds = now - processes[entry_idx].start_time;
            snprintf(time_str, sizeof(time_str), "%02d:%02d", seconds / 60, seconds % 60);
            
            snprintf(display, sizeof(display), "%-8d [%s] %s", 
                    processes[entry_idx].pid, time_str, processes[entry_idx].command);

            if (entry_idx == (active_panel == &left_panel ? left_panel.selected_idx : right_panel.selected_idx)) {
                XSetForeground(dpy, gc, 0xFFC0C0);
                XFillRectangle(dpy, win, gc,
                               0, MARGIN + (i + 1) * LINE_HEIGHT,
                               win_w, LINE_HEIGHT);
                XSetForeground(dpy, gc, BlackPixel(dpy, 0));
            }

            XDrawString(dpy, win, gc, MARGIN, y, display, strlen(display));
        }
    } else {
        /* Draw dual panel file view */
        int visible_lines = (win_h - LINE_HEIGHT - MARGIN) / LINE_HEIGHT;
        
        /* Draw panel headers */
        char left_header[256], right_header[256];
        snprintf(left_header, sizeof(left_header), "LEFT: %s", left_panel.cwd);
        snprintf(right_header, sizeof(right_header), "RIGHT: %s", right_panel.cwd);
        
        /* Left panel header */
        if (active_panel == &left_panel) {
            XSetForeground(dpy, gc, 0x0000FF);
        }
        XDrawString(dpy, win, gc, MARGIN, MARGIN + fontinfo->ascent, left_header, strlen(left_header));
        if (active_panel == &left_panel) {
            XSetForeground(dpy, gc, BlackPixel(dpy, 0));
        }
        
        /* Right panel header */
        if (active_panel == &right_panel) {
            XSetForeground(dpy, gc, 0x0000FF);
        }
        XDrawString(dpy, win, gc, separator_x + MARGIN, MARGIN + fontinfo->ascent, right_header, strlen(right_header));
        if (active_panel == &right_panel) {
            XSetForeground(dpy, gc, BlackPixel(dpy, 0));
        }
        
        /* Draw column headers */
        XDrawString(dpy, win, gc, MARGIN, MARGIN + LINE_HEIGHT + fontinfo->ascent, 
                   "Permissions Type    Name", 25);
        XDrawString(dpy, win, gc, separator_x + MARGIN, MARGIN + LINE_HEIGHT + fontinfo->ascent, 
                   "Permissions Type    Name", 25);
        
        /* Draw separator line between headers and content */
        XDrawLine(dpy, win, gc, 0, MARGIN + LINE_HEIGHT * 2, win_w, MARGIN + LINE_HEIGHT * 2);
        
        /* Draw vertical separator */
        XSetForeground(dpy, gc, 0x808080);
        XFillRectangle(dpy, win, gc, separator_x - PANEL_SEPARATOR_WIDTH / 2, 0, 
                      PANEL_SEPARATOR_WIDTH, win_h);
        XSetForeground(dpy, gc, BlackPixel(dpy, 0));
        
        /* Draw left panel content */
        int i;
        for (i = 0; i < visible_lines; i++) {
            int entry_idx = i + left_panel.scroll_offset;
            if (entry_idx >= left_panel.nentries) break;
            
            int y = MARGIN + (i + 2) * LINE_HEIGHT + fontinfo->ascent;
            char display[400];
            snprintf(display, sizeof(display), "%-11s %s %s",
                    left_panel.entries[entry_idx].perms,
                    left_panel.entries[entry_idx].type_label,
                    left_panel.entries[entry_idx].name);

            if (entry_idx == left_panel.selected_idx && active_panel == &left_panel) {
                XSetForeground(dpy, gc, 0xC0C0C0);
                XFillRectangle(dpy, win, gc,
                               0, MARGIN + (i + 2) * LINE_HEIGHT,
                               panel_width, LINE_HEIGHT);
                XSetForeground(dpy, gc, BlackPixel(dpy, 0));
            }

            XDrawString(dpy, win, gc, MARGIN, y, display, strlen(display));
        }
        
        /* Draw right panel content */
        for (i = 0; i < visible_lines; i++) {
            int entry_idx = i + right_panel.scroll_offset;
            if (entry_idx >= right_panel.nentries) break;
            
            int y = MARGIN + (i + 2) * LINE_HEIGHT + fontinfo->ascent;
            char display[400];
            snprintf(display, sizeof(display), "%-11s %s %s",
                    right_panel.entries[entry_idx].perms,
                    right_panel.entries[entry_idx].type_label,
                    right_panel.entries[entry_idx].name);

            if (entry_idx == right_panel.selected_idx && active_panel == &right_panel) {
                XSetForeground(dpy, gc, 0xC0C0C0);
                XFillRectangle(dpy, win, gc,
                               separator_x + PANEL_SEPARATOR_WIDTH / 2, 
                               MARGIN + (i + 2) * LINE_HEIGHT,
                               panel_width, LINE_HEIGHT);
                XSetForeground(dpy, gc, BlackPixel(dpy, 0));
            }

            XDrawString(dpy, win, gc, separator_x + MARGIN, y, display, strlen(display));
        }
    }
    
    /* Draw mode button */
    char mode_button[50];
    if (show_processes) {
        snprintf(mode_button, sizeof(mode_button), "[F9] Files");
    } else {
        snprintf(mode_button, sizeof(mode_button), "[F9] Processes");
    }
    XDrawString(dpy, win, gc, win_w - 100, MARGIN + fontinfo->ascent, 
               mode_button, strlen(mode_button));
}

static void show_help(void)
{
    int help_pid = fork();
    if (help_pid == 0) {
        const char *help_text =
            "=== Minix File Manager v8.0 - Dual Panel - Keyboard Shortcuts ===\n\n"
            "Panel Navigation:\n"
            "  Tab               - Switch between left/right panels\n"
            "  Left/Right arrows - Switch between panels\n"
            "  Up/Down arrows    - Navigate through files\n"
            "  Page Up/Down      - Scroll page by page\n"
            "  Home/End          - Jump to first/last file\n"
            "  Enter             - Open selected file/directory\n\n"
            "File Operations:\n"
            "  F5 or R           - Refresh current panel\n"
            "  F2 or F           - Open file in editor\n"
            "  Delete or D       - Delete selected file (with confirmation)\n"
            "  C                 - Copy file to other panel (with rename option)\n"
            "  M                 - Move file to other panel (with rename option)\n\n"
            "Directory Operations:\n"
            "  Backspace         - Go to parent directory\n"
            "  T                 - Open terminal in current directory\n"
            "  H                 - Go to home directory in active panel\n"
            "  /                 - Go to root directory in active panel\n\n"
            "Process Management:\n"
            "  F9                - Toggle files/processes view\n"
            "  K                 - Kill selected process\n"
            "  F8                - Refresh process list\n\n"
            "Application:\n"
            "  F1 or ?           - Show this help\n"
            "  Q or Ctrl+Q       - Quit application\n"
            "  Ctrl+L            - Refresh display\n\n"
            "Press any key to close this window...";
        
        char temp_file[] = "/tmp/minixfm_help_XXXXXX";
        int fd = mkstemp(temp_file);
        if (fd != -1) {
            write(fd, help_text, strlen(help_text));
            close(fd);
            
            execlp("xterm", "xterm", "-title", "Minix FM Help", 
                   "-geometry", "80x25", "-e", "cat", temp_file, NULL);
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

static void open_entry(Panel *panel, int idx)
{
    char newpath[1024];
    if (idx < 0 || idx >= panel->nentries) return;

    if (panel->entries[idx].is_dir) {
        if (strcmp(panel->entries[idx].name, "..") == 0) {
            char *p = strrchr(panel->cwd, '/');
            if (!p || p == panel->cwd)
                strcpy(panel->cwd, "/");
            else
                *p = '\0';
        } else {
            snprintf(newpath, sizeof(newpath), "%s/%s",
                     strcmp(panel->cwd, "/") == 0 ? "" : panel->cwd,
                     panel->entries[idx].name);
            strcpy(panel->cwd, newpath);
        }
        read_dir(panel, panel->cwd);
        draw_interface();
    } else {
        char filepath[1024];
        snprintf(filepath, sizeof(filepath), "%s/%s",
                 strcmp(panel->cwd, "/") == 0 ? "" : panel->cwd,
                 panel->entries[idx].name);

        if (!has_read_access(filepath)) {
            fprintf(stderr, "Permission denied: %s\n", filepath);
            return;
        }

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
        chdir(active_panel->cwd);
        execlp("xterm", "xterm", NULL);
        exit(1);
    } else if (pid > 0) {
        add_process(pid, "xterm");
    }
}

static int confirm_dialog(const char *message)
{
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
    Panel *panel = active_panel;
    if (panel->selected_idx < 0 || panel->selected_idx >= panel->nentries) return;
    
    char filepath[1024];
    snprintf(filepath, sizeof(filepath), "%s/%s",
             strcmp(panel->cwd, "/") == 0 ? "" : panel->cwd,
             panel->entries[panel->selected_idx].name);
    
    if (panel->entries[panel->selected_idx].is_dir) {
        if (!confirm_dialog("Delete directory? This cannot be undone!")) return;
        if (rmdir(filepath) == 0) {
            read_dir(panel, panel->cwd);
            draw_interface();
        } else {
            perror("rmdir");
        }
    } else {
        if (!confirm_dialog("Delete file? This cannot be undone!")) return;
        if (unlink(filepath) == 0) {
            read_dir(panel, panel->cwd);
            draw_interface();
        } else {
            perror("unlink");
        }
    }
}

static void go_home(void)
{
    char *home = getenv("HOME");
    if (home) {
        strcpy(active_panel->cwd, home);
        read_dir(active_panel, active_panel->cwd);
        draw_interface();
    }
}

/* Функция для обработки копирования/перемещения */
static void handle_copy_move(int is_move)
{
    Panel *source_panel = active_panel;
    Panel *target_panel = (active_panel == &left_panel) ? &right_panel : &left_panel;
    
    if (source_panel->selected_idx < 0 || source_panel->selected_idx >= source_panel->nentries) {
        return;
    }
    
    Entry *entry = &source_panel->entries[source_panel->selected_idx];
    char source_path[1024];
    snprintf(source_path, sizeof(source_path), "%s/%s",
             strcmp(source_panel->cwd, "/") == 0 ? "" : source_panel->cwd,
             entry->name);
    
    char new_name[256];
    int operation;
    
    if (show_copy_move_dialog(entry->name, new_name, &operation)) {
        char target_name[256];
        if (strlen(new_name) > 0) {
            strcpy(target_name, new_name);
        } else {
            strcpy(target_name, entry->name);
        }
        
        char target_path[1024];
        snprintf(target_path, sizeof(target_path), "%s/%s",
                 strcmp(target_panel->cwd, "/") == 0 ? "" : target_panel->cwd,
                 target_name);
        
        if (operation == 1) { /* Copy */
            copy_file_or_dir(source_path, target_path);
        } else if (operation == 2) { /* Move */
            move_file_or_dir(source_path, target_path);
        }
        
        /* Refresh both panels */
        read_dir(source_panel, source_panel->cwd);
        read_dir(target_panel, target_panel->cwd);
        draw_interface();
    }
}

static long diff_ms(struct timespec a, struct timespec b)
{
    return (a.tv_sec - b.tv_sec) * 1000 + (a.tv_nsec - b.tv_nsec) / 1000000;
}

static void handle_click(int x, int y)
{
    if (show_processes) {
        /* Process view click handling */
        int visible_lines = (win_h - LINE_HEIGHT - MARGIN) / LINE_HEIGHT;
        int idx = (y - MARGIN - LINE_HEIGHT) / LINE_HEIGHT + 
                 (active_panel == &left_panel ? left_panel.scroll_offset : right_panel.scroll_offset);
        
        if (idx >= 0 && idx < nprocesses) {
            struct timespec now;
            clock_gettime(CLOCK_MONOTONIC, &now);
            long diff = diff_ms(now, last_click_time);

            if (active_panel == &left_panel) {
                left_panel.selected_idx = idx;
            } else {
                right_panel.selected_idx = idx;
            }
            draw_interface();

            if (idx == last_click_idx && diff < DOUBLE_CLICK_DELAY && last_click_panel == (active_panel == &left_panel ? 0 : 1)) {
                kill_process(idx);
                last_click_idx = -1;
            } else {
                last_click_idx = idx;
                last_click_panel = (active_panel == &left_panel ? 0 : 1);
                last_click_time = now;
            }
        }
    } else {
        /* File view click handling */
        int panel_width = win_w / 2 - PANEL_SEPARATOR_WIDTH / 2;
        Panel *clicked_panel = (x < panel_width) ? &left_panel : &right_panel;
        
        /* Switch active panel if clicked on different panel */
        if (clicked_panel != active_panel) {
            active_panel = clicked_panel;
            draw_interface();
            return;
        }
        
        int visible_lines = (win_h - LINE_HEIGHT * 2 - MARGIN) / LINE_HEIGHT;
        int idx = (y - MARGIN - LINE_HEIGHT * 2) / LINE_HEIGHT + active_panel->scroll_offset;
        
        if (idx >= 0 && idx < active_panel->nentries) {
            struct timespec now;
            clock_gettime(CLOCK_MONOTONIC, &now);
            long diff = diff_ms(now, last_click_time);

            active_panel->selected_idx = idx;
            draw_interface();

            if (idx == last_click_idx && diff < DOUBLE_CLICK_DELAY && 
                last_click_panel == (active_panel == &left_panel ? 0 : 1)) {
                open_entry(active_panel, idx);
                last_click_idx = -1;
            } else {
                last_click_idx = idx;
                last_click_panel = (active_panel == &left_panel ? 0 : 1);
                last_click_time = now;
            }
        }
    }
}

static void scroll_panel(Panel *panel, int direction)
{
    int visible_lines = (win_h - LINE_HEIGHT * 2 - MARGIN) / LINE_HEIGHT;
    
    if (direction > 0) {
        /* Scroll down */
        if (panel->scroll_offset + visible_lines < panel->nentries)
            panel->scroll_offset++;
    } else {
        /* Scroll up */
        if (panel->scroll_offset > 0)
            panel->scroll_offset--;
    }
    draw_interface();
}

static void scroll_page(Panel *panel, int direction)
{
    int visible_lines = (win_h - LINE_HEIGHT * 2 - MARGIN) / LINE_HEIGHT;
    
    if (direction > 0) {
        /* Page down */
        panel->scroll_offset += visible_lines;
        if (panel->scroll_offset + visible_lines > panel->nentries)
            panel->scroll_offset = panel->nentries - visible_lines;
    } else {
        /* Page up */
        panel->scroll_offset -= visible_lines;
        if (panel->scroll_offset < 0) panel->scroll_offset = 0;
    }
    draw_interface();
}

static void switch_panel(void)
{
    active_panel = (active_panel == &left_panel) ? &right_panel : &left_panel;
    draw_interface();
}

static void toggle_process_view(void)
{
    show_processes = !show_processes;
    left_panel.selected_idx = -1;
    right_panel.selected_idx = -1;
    left_panel.scroll_offset = 0;
    right_panel.scroll_offset = 0;
    if (show_processes) {
        update_process_list();
    }
    draw_interface();
}

static void handle_key(XKeyEvent *key)
{
    KeySym ks = XLookupKeysym(key, 0);
    int state = key->state;
    int ctrl_pressed = (state & ControlMask);
    
    switch (ks) {
        /* Panel Navigation */
        case XK_Tab:
        case XK_Left:
        case XK_Right:
            switch_panel();
            break;

        case XK_Up:
            if (active_panel->selected_idx > 0) active_panel->selected_idx--;
            if (active_panel->selected_idx < active_panel->scroll_offset)
                active_panel->scroll_offset = active_panel->selected_idx;
            draw_interface();
            break;

        case XK_Down: {
            int visible_lines = (win_h - LINE_HEIGHT * 2 - MARGIN) / LINE_HEIGHT;
            int max_idx = show_processes ? nprocesses - 1 : active_panel->nentries - 1;
            
            if (active_panel->selected_idx < max_idx) active_panel->selected_idx++;
            if (active_panel->selected_idx >= active_panel->scroll_offset + visible_lines)
                active_panel->scroll_offset++;
            draw_interface();
            break;
        }

        case XK_Page_Up:
            scroll_page(active_panel, -1);
            break;

        case XK_Page_Down:
            scroll_page(active_panel, 1);
            break;
            
        case XK_Home:
            active_panel->scroll_offset = 0;
            active_panel->selected_idx = 0;
            draw_interface();
            break;
            
        case XK_End: {
            int visible_lines = (win_h - LINE_HEIGHT * 2 - MARGIN) / LINE_HEIGHT;
            int max_idx = show_processes ? nprocesses - 1 : active_panel->nentries - 1;
            active_panel->selected_idx = max_idx;
            active_panel->scroll_offset = max_idx - visible_lines + 1;
            if (active_panel->scroll_offset < 0) active_panel->scroll_offset = 0;
            draw_interface();
            break;
        }

        /* Основные операции */
        case XK_Return:
            if (active_panel->selected_idx >= 0 && !show_processes)
                open_entry(active_panel, active_panel->selected_idx);
            break;
            
        case XK_BackSpace:
            if (!show_processes) {
                int i;
                for (i = 0; i < active_panel->nentries; i++) {
                    if (strcmp(active_panel->entries[i].name, "..") == 0) {
                        open_entry(active_panel, i);
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
            if (active_panel->selected_idx >= 0 && !show_processes && 
                !active_panel->entries[active_panel->selected_idx].is_dir)
                open_entry(active_panel, active_panel->selected_idx);
            break;
            
        case XK_F5:
        case XK_r:
        case XK_R:
            if (show_processes) {
                update_process_list();
            } else {
                read_dir(active_panel, active_panel->cwd);
            }
            draw_interface();
            break;
            
        case XK_F8:
            update_process_list();
            draw_interface();
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
            if (show_processes && active_panel->selected_idx >= 0) {
                kill_process(active_panel->selected_idx);
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
                strcpy(active_panel->cwd, "/");
                read_dir(active_panel, active_panel->cwd);
                draw_interface();
            }
            break;

        /* Копирование и перемещение */
        case XK_c:
        case XK_C:
            if (!show_processes) {
                handle_copy_move(0); /* Copy */
            }
            break;
            
        case XK_m:
        case XK_M:
            if (!show_processes) {
                handle_copy_move(1); /* Move */
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
                draw_interface();
            }
            break;
    }
}

int main(int argc, char **argv)
{
    printf("Minix File Manager v8.0 - Dual Panel started\n");
    printf("Press F1 or ? for keyboard shortcuts\n");
    
    /* Initialize panels */
    nprocesses = 0;
    show_processes = 0;
    
    char start_cwd[1024];
    if (getcwd(start_cwd, sizeof(start_cwd)) == NULL)
        strcpy(start_cwd, "/");
    
    init_panel(&left_panel, start_cwd);
    init_panel(&right_panel, start_cwd);
    active_panel = &left_panel;

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
    hints.min_width = 800;   /* Увеличен минимальный размер */
    hints.min_height = 600;  /* Увеличен минимальный размер */
    hints.max_width = 2000;
    hints.max_height = 1200;
    XSetNormalHints(dpy, win, &hints);

    XSelectInput(dpy, win, ExposureMask | ButtonPressMask |
                 StructureNotifyMask | KeyPressMask);
    XStoreName(dpy, win, "Minix FM v8.0 - Dual Panel (Press F1 for help)");
    XMapWindow(dpy, win);

    /* Загрузка большего шрифта */
    fontinfo = XLoadQueryFont(dpy, "9x15");
    if (!fontinfo) fontinfo = XLoadQueryFont(dpy, "8x13");
    if (!fontinfo) fontinfo = XLoadQueryFont(dpy, "fixed");
    if (!fontinfo) fontinfo = XLoadQueryFont(dpy, "6x13");
    
    gc = XCreateGC(dpy, win, 0, NULL);
    if (fontinfo) XSetFont(dpy, gc, fontinfo->fid);

    draw_interface();

    XEvent ev;
    while (1) {
        XNextEvent(dpy, &ev);
        if (ev.type == Expose)
            draw_interface();
        else if (ev.type == ButtonPress) {
            if (ev.xbutton.button == Button4)
                scroll_panel(active_panel, -1);
            else if (ev.xbutton.button == Button5)
                scroll_panel(active_panel, 1);
            else
                handle_click(ev.xbutton.x, ev.xbutton.y);
        }
        else if (ev.type == KeyPress)
            handle_key(&ev.xkey);
        else if (ev.type == ConfigureNotify) {
            win_w = ev.xconfigure.width;
            win_h = ev.xconfigure.height;
            draw_interface();
        }
    }

    return 0;
}
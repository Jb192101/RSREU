/*
 * minix_xfm_v8_modified.c
 * Modified according to user requirements:
 * 1) Increased default window size, larger font, made window freely resizable
 * 2) When transferring a selected file from one panel to another a modal dialog
 *    appears with options: Move, Copy, Cancel (and a rename input)
 * 3) Add ability to rename the file during transfer
 *
 * Note: This remains a single-file, plain Xlib C program in the original style.
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
#include <fcntl.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#define MARGIN 5
#define DOUBLE_CLICK_DELAY 300
#define MAX_PROCESSES 50
#define PANEL_SEPARATOR_WIDTH 4
#define MAX_ENTRIES 1000

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
    Entry entries[MAX_ENTRIES];
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
static int line_height = 16; /* computed at runtime */

static Panel left_panel, right_panel;
static Panel *active_panel;
static struct timespec last_click_time = {0};
static int last_click_idx = -1;
static int last_click_panel = 0; // 0 = left, 1 = right

static Process processes[MAX_PROCESSES];
static int nprocesses = 0;
static int show_processes = 0;

/* Default window size increased */
static int win_w = 1024;
static int win_h = 700;
static int separator_x;

/* Forward declarations */
static void draw_interface(void);
static void read_dir(Panel *panel, const char *path);
static void update_process_list(void);
static void show_help(void);
static void init_panel(Panel *panel, const char *initial_path);

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
    strncpy(panel->cwd, initial_path, sizeof(panel->cwd)-1);
    panel->cwd[sizeof(panel->cwd)-1] = '\0';
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
        strncpy(panel->entries[panel->nentries].name, "..", 255);
        panel->entries[panel->nentries].is_dir = 1;
        strncpy(panel->entries[panel->nentries].perms, "drwx------", 11);
        strncpy(panel->entries[panel->nentries].type_label, "[DIR]", 7);
        panel->entries[panel->nentries].mode = S_IFDIR | 0700;
        panel->nentries++;
    }

    while ((de = readdir(d)) != NULL && panel->nentries < MAX_ENTRIES) {
        if (strcmp(de->d_name, ".") == 0) continue;
        snprintf(full, sizeof(full), "%s/%s", path, de->d_name);
        if (stat(full, &st) == 0) {
            Entry *entry = &panel->entries[panel->nentries];
            strncpy(entry->name, de->d_name, 255);
            entry->name[255] = '\0';
            entry->is_dir = S_ISDIR(st.st_mode);
            entry->mode = st.st_mode;
            mode_to_str(st.st_mode, entry->perms);

            if (entry->is_dir) {
                strncpy(entry->type_label, "[DIR]", 7);
            } else {
                if (S_ISREG(st.st_mode)) {
                    if (is_text_file(full))
                        strncpy(entry->type_label, "[TXT]", 7);
                    else
                        strncpy(entry->type_label, "[BIN]", 7);
                } else {
                    if (S_ISCHR(st.st_mode)) strncpy(entry->type_label, "[CHR]", 7);
                    else if (S_ISBLK(st.st_mode)) strncpy(entry->type_label, "[BLK]", 7);
                    else if (S_ISFIFO(st.st_mode)) strncpy(entry->type_label, "[FIFO]", 7);
                    else if (S_ISLNK(st.st_mode)) strncpy(entry->type_label, "[LNK]", 7);
                    else if (S_ISSOCK(st.st_mode)) strncpy(entry->type_label, "[SOCK]", 7);
                    else strncpy(entry->type_label, "[???]", 7);
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

/* Utility: copy regular file */
static int copy_file(const char *src, const char *dst)
{
    int ifd = open(src, O_RDONLY);
    if (ifd < 0) return -1;
    int ofd = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (ofd < 0) { close(ifd); return -1; }

    char buf[8192];
    ssize_t r;
    while ((r = read(ifd, buf, sizeof(buf))) > 0) {
        ssize_t w = write(ofd, buf, r);
        if (w != r) { close(ifd); close(ofd); return -1; }
    }
    close(ifd);
    close(ofd);
    return (r == 0) ? 0 : -1;
}

/* Blocking modal dialog for transfer with simple rename input and buttons */
static int transfer_dialog(Window parent, const char *src_name, char *out_newname, int out_newname_len, int allow_rename)
{
    /* Returns: 0 = Cancel, 1 = Move, 2 = Copy */
    int dialog_w = 480;
    int dialog_h = 160;
    int sx = (win_w - dialog_w) / 2;
    int sy = (win_h - dialog_h) / 2;

    Window dlg = XCreateSimpleWindow(dpy, win, sx, sy, dialog_w, dialog_h, 2, BlackPixel(dpy, 0), WhitePixel(dpy, 0));
    XSelectInput(dpy, dlg, ExposureMask | ButtonPressMask | KeyPressMask);
    XMapWindow(dpy, dlg);
    XFlush(dpy);

    char title[512];
    snprintf(title, sizeof(title), "Transfer: %s", src_name);
    XStoreName(dpy, dlg, title);

    /* layout: text showing source, input line for new name, three buttons */
    int button_w = 100; int button_h = 30;
    int bx1 = dialog_w/2 - button_w - 10;
    int bx2 = dialog_w/2 + 10;
    int by = dialog_h - button_h - 15;

    /* initial newname = src_name */
    strncpy(out_newname, src_name, out_newname_len-1);
    out_newname[out_newname_len-1] = '\0';

    int result = 0;
    int running = 1;

    /* simple keyboard input state */
    int cursor_pos = strlen(out_newname);

    while (running) {
        XEvent ev;
        XNextEvent(dpy, &ev);
        if (ev.type == Expose && ev.xexpose.window == dlg) {
            /* draw dialog contents */
            int y = 20 + fontinfo->ascent;
            char line1[512];
            snprintf(line1, sizeof(line1), "Transfer file: %s", src_name);
            XDrawString(dpy, dlg, gc, 10, y, line1, strlen(line1));

            /* input prompt */
            y += line_height + 5;
            XDrawString(dpy, dlg, gc, 10, y, "New name (leave empty to keep):", 27);

            /* input box */
            y += 5 + fontinfo->ascent;
            XDrawRectangle(dpy, dlg, gc, 10, y - fontinfo->ascent, dialog_w - 20, line_height + 4);

            /* draw current new name */
            XDrawString(dpy, dlg, gc, 12, y + fontinfo->ascent/2, out_newname, strlen(out_newname));

            /* draw buttons */
            XDrawRectangle(dpy, dlg, gc, bx1, by, button_w, button_h);
            XDrawString(dpy, dlg, gc, bx1 + 20, by + fontinfo->ascent + 6, "Move", 4);

            XDrawRectangle(dpy, dlg, gc, bx2, by, button_w, button_h);
            XDrawString(dpy, dlg, gc, bx2 + 20, by + fontinfo->ascent + 6, "Copy", 4);

            /* cancel button to the right */
            int bx3 = dialog_w - button_w - 10;
            XDrawRectangle(dpy, dlg, gc, bx3, by, button_w, button_h);
            XDrawString(dpy, dlg, gc, bx3 + 18, by + fontinfo->ascent + 6, "Cancel", 6);

            XFlush(dpy);
        } else if (ev.type == ButtonPress && ev.xbutton.window == dlg) {
            int rx = ev.xbutton.x;
            int ry = ev.xbutton.y;
            /* check buttons */
            if (rx >= bx1 && rx <= bx1 + button_w && ry >= by && ry <= by + button_h) {
                result = 1; running = 0; /* Move */
            } else if (rx >= bx2 && rx <= bx2 + button_w && ry >= by && ry <= by + button_h) {
                result = 2; running = 0; /* Copy */
            } else if (rx >= (dialog_w - button_w - 10) && rx <= (dialog_w - 10) && ry >= by && ry <= by + button_h) {
                result = 0; running = 0; /* Cancel */
            } else {
                /* click in input area -> set keyboard focus implicitly */
            }
        } else if (ev.type == KeyPress && ev.xkey.window == dlg) {
            KeySym ks = XLookupKeysym(&ev.xkey, 0);
            if (ks == XK_Return) {
                /* default action: Move */
                result = 1; running = 0;
            } else if (ks == XK_Escape) {
                result = 0; running = 0;
            } else if (ks == XK_BackSpace) {
                int len = strlen(out_newname);
                if (len > 0) out_newname[len-1] = '\0';
            } else {
                /* append printable chars */
                char buf[32];
                int n = XLookupString(&ev.xkey, buf, sizeof(buf), NULL, NULL);
                for (int i = 0; i < n; i++) {
                    char c = buf[i];
                    if (c >= 32 && c < 127 && (int)strlen(out_newname) + 1 < out_newname_len) {
                        int l = strlen(out_newname);
                        out_newname[l] = c; out_newname[l+1] = '\0';
                    }
                }
            }
            /* force redraw of dialog by sending expose */
            XClearWindow(dpy, dlg);
            /* Redraw dialog after clear */
            XExposeEvent ex;
            memset(&ex,0,sizeof(ex));
            ex.type = Expose;
            ex.display = dpy;
            ex.window = dlg;
            XSendEvent(dpy, dlg, True, ExposureMask, (XEvent*)&ex);
            XFlush(dpy);
            XFlush(dpy);
        }
    }

    XDestroyWindow(dpy, dlg);
    XFlush(dpy);
    /* TODO: fix rename logic */
    return result;
}

/* Perform transfer between panels; if newname is empty, keep original name */
static void perform_transfer(Panel *src, Panel *dst, int src_idx, int do_move, const char *newname)
{
    char src_path[1024], dst_path[1024];
    snprintf(src_path, sizeof(src_path), "%s/%s", strcmp(src->cwd, "/") == 0 ? "" : src->cwd, src->entries[src_idx].name);

    const char *final_name = (newname && strlen(newname) > 0) ? newname : src->entries[src_idx].name;

    snprintf(dst_path, sizeof(dst_path), "%s/%s", strcmp(dst->cwd, "/") == 0 ? "" : dst->cwd, final_name);

    if (src->entries[src_idx].is_dir) {
        /* For simplicity do not handle recursive copy/move of directories */
        fprintf(stderr, "Directory transfer not supported in this simple implementation\n");
        return;
    }

    if (do_move) {
        if (rename(src_path, dst_path) == 0) {
            read_dir(src, src->cwd);
            read_dir(dst, dst->cwd);
            draw_interface();
        } else {
            /* fallback: copy + unlink */
            if (copy_file(src_path, dst_path) == 0) {
                if (unlink(src_path) == 0) {
                    read_dir(src, src->cwd);
                    read_dir(dst, dst->cwd);
                    draw_interface();
                } else perror("unlink");
            } else perror("copy_file");
        }
    } else {
        if (copy_file(src_path, dst_path) == 0) {
            read_dir(dst, dst->cwd);
            draw_interface();
        } else perror("copy_file");
    }
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
            "  Delete or D       - Delete selected file (with confirmation)\n\n"
            "Directory Operations:\n"
            "  Backspace         - Go to parent directory\n"
            "  T                 - Open terminal in current directory\n"
            "  H                 - Go to home directory in active panel\n"
            "  /                 - Go to root directory in active panel\n"
            "  C                 - Copy file to other panel (shows dialog)\n"
            "  M                 - Move file to other panel (shows dialog)\n\n"
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
            strncpy(panel->cwd, newpath, sizeof(panel->cwd)-1);
            panel->cwd[sizeof(panel->cwd)-1] = '\0';
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
        strncpy(active_panel->cwd, home, sizeof(active_panel->cwd)-1);
        active_panel->cwd[sizeof(active_panel->cwd)-1] = '\0';
        read_dir(active_panel, active_panel->cwd);
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
        int visible_lines = (win_h - line_height - MARGIN) / line_height;
        int idx = (y - MARGIN - line_height) / line_height +
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

        int visible_lines = (win_h - line_height * 2 - MARGIN) / line_height;
        int idx = (y - MARGIN - line_height * 2) / line_height + active_panel->scroll_offset;

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
    int visible_lines = (win_h - line_height * 2 - MARGIN) / line_height;

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
    int visible_lines = (win_h - line_height * 2 - MARGIN) / line_height;

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

/* initiate transfer from active panel to the other */
static void initiate_transfer(void)
{
    Panel *src = active_panel;
    Panel *dst = (active_panel == &left_panel) ? &right_panel : &left_panel;
    if (src->selected_idx < 0 || src->selected_idx >= src->nentries) return;
    if (src->entries[src->selected_idx].is_dir) {
        /* for simplicity do not support directory transfer here */
        fprintf(stderr, "Directory transfer not supported.\n");
        return;
    }

    char newname[512] = {0};
    int choice = transfer_dialog(win, src->entries[src->selected_idx].name, newname, sizeof(newname), 1);
    if (choice == 0) return; /* cancel */

    perform_transfer(src, dst, src->selected_idx, (choice == 1), newname);
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
            int visible_lines = (win_h - line_height * 2 - MARGIN) / line_height;
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
            int visible_lines = (win_h - line_height * 2 - MARGIN) / line_height;
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

		/* Домой (переход в домашний каталог) */
		case XK_h:
		case XK_H:
			if (!show_processes) {
				go_home();
			}
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

        /* Transfer operations: show dialog */
        case XK_c:
        case XK_C:
            if (!show_processes) initiate_transfer();
            break;

        case XK_m:
        case XK_M:
            if (!show_processes) initiate_transfer();
            break;

        /* Директории */
        case XK_t:
        case XK_T:
            if (!show_processes) {
                open_terminal();
            }
            break;

        case XK_h:
        case XK_F1:
        case XK_question:
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
        XDrawLine(dpy, win, gc, 0, MARGIN + line_height, win_w, MARGIN + line_height);

        int visible_lines = (win_h - line_height - MARGIN) / line_height;
        int i;
        for (i = 0; i < visible_lines; i++) {
            int entry_idx = i + (active_panel == &left_panel ? left_panel.scroll_offset : right_panel.scroll_offset);
            if (entry_idx >= nprocesses) break;

            int y = MARGIN + (i + 1) * line_height + fontinfo->ascent;
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
                               0, MARGIN + (i + 1) * line_height,
                               win_w, line_height);
                XSetForeground(dpy, gc, BlackPixel(dpy, 0));
            }

            XDrawString(dpy, win, gc, MARGIN, y, display, strlen(display));
        }
    } else {
        /* Draw dual panel file view */
        int visible_lines = (win_h - line_height - MARGIN) / line_height - 1;

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
        XDrawString(dpy, win, gc, MARGIN, MARGIN + line_height + fontinfo->ascent,
                   "Permissions Type    Name", 25);
        XDrawString(dpy, win, gc, separator_x + MARGIN, MARGIN + line_height + fontinfo->ascent,
                   "Permissions Type    Name", 25);

        /* Draw separator line between headers and content */
        XDrawLine(dpy, win, gc, 0, MARGIN + line_height * 2, win_w, MARGIN + line_height * 2);

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

            int y = MARGIN + (i + 2) * line_height + fontinfo->ascent;
            char display[400];
            snprintf(display, sizeof(display), "%-11s %s %s",
                    left_panel.entries[entry_idx].perms,
                    left_panel.entries[entry_idx].type_label,
                    left_panel.entries[entry_idx].name);

            if (entry_idx == left_panel.selected_idx && active_panel == &left_panel) {
                XSetForeground(dpy, gc, 0xC0C0C0);
                XFillRectangle(dpy, win, gc,
                               0, MARGIN + (i + 2) * line_height,
                               panel_width, line_height);
                XSetForeground(dpy, gc, BlackPixel(dpy, 0));
            }

            XDrawString(dpy, win, gc, MARGIN, y, display, strlen(display));
        }

        /* Draw right panel content */
        for (i = 0; i < visible_lines; i++) {
            int entry_idx = i + right_panel.scroll_offset;
            if (entry_idx >= right_panel.nentries) break;

            int y = MARGIN + (i + 2) * line_height + fontinfo->ascent;
            char display[400];
            snprintf(display, sizeof(display), "%-11s %s %s",
                    right_panel.entries[entry_idx].perms,
                    right_panel.entries[entry_idx].type_label,
                    right_panel.entries[entry_idx].name);

            if (entry_idx == right_panel.selected_idx && active_panel == &right_panel) {
                XSetForeground(dpy, gc, 0xC0C0C0);
                XFillRectangle(dpy, win, gc,
                               separator_x + PANEL_SEPARATOR_WIDTH / 2,
                               MARGIN + (i + 2) * line_height,
                               panel_width, line_height);
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
    XDrawString(dpy, win, gc, win_w - 140, MARGIN + fontinfo->ascent,
               mode_button, strlen(mode_button));
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
    /* only set minimum size to allow free resizing */
    hints.flags = PMinSize;
    hints.min_width = 600;
    hints.min_height = 400;
    XSetNormalHints(dpy, win, &hints);

    XSelectInput(dpy, win, ExposureMask | ButtonPressMask |
                 StructureNotifyMask | KeyPressMask);
    XStoreName(dpy, win, "Minix FM v8.0 - Dual Panel (Press F1 for help)");
    XMapWindow(dpy, win);

    /* Try to load a larger font */
    fontinfo = XLoadQueryFont(dpy, "9x15");
    if (!fontinfo) fontinfo = XLoadQueryFont(dpy, "10x20");
    if (!fontinfo) fontinfo = XLoadQueryFont(dpy, "fixed");
    if (!fontinfo) fontinfo = XLoadQueryFont(dpy, "6x13");

    gc = XCreateGC(dpy, win, 0, NULL);
    if (fontinfo) {
        XSetFont(dpy, gc, fontinfo->fid);
        line_height = fontinfo->ascent + fontinfo->descent + 4;
    } else {
        line_height = 18; /* fallback */
    }

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

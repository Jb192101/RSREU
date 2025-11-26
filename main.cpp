/*
 * minix_xfm_v9_final.c
 * Финальная версия
 * Исправлено:
 *  - Перерисовка модальных окон (справка, копирование)
 *  - Белые окна при клике
 *  - Добавлен тёмный / светлый фон (клавиша T)
 *  - Добавлено окно изменения прав доступа (клавиша P)
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

// Цветовые схемы
int dark_mode = 0;
unsigned long bg_color, fg_color, sel_bg_color;
unsigned long dark_bg = 0x202020, dark_fg = 0xFFFFFF, dark_sel = 0x404040;
unsigned long light_bg = 0xFFFFFF, light_fg = 0x000000, light_sel = 0xC0C0C0;

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
static int line_height = 16;

static Panel left_panel, right_panel;
static Panel *active_panel;
static struct timespec last_click_time = {0};
static int last_click_idx = -1;
static int last_click_panel = 0;

static Process processes[MAX_PROCESSES];
static int nprocesses = 0;

static int win_w = 1024;
static int win_h = 700;
static int separator_x;

static void draw_interface(void);
static void read_dir(Panel *panel, const char *path);
static void show_help_dialog(void);
static void update_process_list(void);
static void apply_theme(void);

static void mode_to_str(mode_t mode, char *out) {
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

static void apply_theme() {
    if (dark_mode) {
        bg_color = dark_bg;
        fg_color = dark_fg;
        sel_bg_color = dark_sel;
    } else {
        bg_color = light_bg;
        fg_color = light_fg;
        sel_bg_color = light_sel;
    }
    XSetForeground(dpy, gc, fg_color);
}

static void switch_theme() {
    dark_mode = !dark_mode;
    apply_theme();
    draw_interface();
}

static int is_text_file(const char *path) {
    struct stat st;
    if (stat(path, &st) != 0) return 0;
    if (!S_ISREG(st.st_mode)) return 0;

    FILE *f = fopen(path, "rb");
    if (!f) return 0;

    unsigned char buf[256];
    size_t n = fread(buf, 1, sizeof(buf), f);
    fclose(f);

    if (!n) return 1;
    for (size_t i = 0; i < n; i++) {
        unsigned char c = buf[i];
        if (c == '\n' || c == '\t' || c == '\r') continue;
        if (c < 32 || c > 126) return 0;
    }
    return 1;
}

static void read_dir(Panel *panel, const char *path) {
    DIR *d = opendir(path);
    if (!d) return;

    panel->nentries = 0;
    if (strcmp(path, "/")) {
        strcpy(panel->entries[0].name, "..");
        panel->entries[0].is_dir = 1;
        strcpy(panel->entries[0].type_label, "[DIR]");
        panel->entries[0].mode = 0755 | S_IFDIR;
        strcpy(panel->entries[0].perms, "drwxr-xr-x");
        panel->nentries = 1;
    }

    struct dirent *de;
    char full[1024];
    while ((de = readdir(d)) && panel->nentries < MAX_ENTRIES) {
        if (!strcmp(de->d_name, ".")) continue;
        snprintf(full, sizeof(full), "%s/%s", path, de->d_name);
        struct stat st;
        if (!stat(full, &st)) {
            Entry *e = &panel->entries[panel->nentries];
            strncpy(e->name, de->d_name, 255);
            e->is_dir = S_ISDIR(st.st_mode);
            e->mode = st.st_mode;
            mode_to_str(st.st_mode, e->perms);
            strcpy(e->type_label, e->is_dir ? "[DIR]" : is_text_file(full) ? "[TXT]" : "[BIN]");
            panel->nentries++;
        }
    }
    closedir(d);
}

static void draw_interface(void) {
    XClearWindow(dpy, win);

    separator_x = win_w / 2;
    int panel_width = separator_x - PANEL_SEPARATOR_WIDTH / 2;

    /* background fill */
    XSetForeground(dpy, gc, bg_color);
    XFillRectangle(dpy, win, gc, 0, 0, win_w, win_h);
    XSetForeground(dpy, gc, fg_color);
    XSetBackground(dpy, gc, bg_color);

    /* Title */
    char title[256];
    snprintf(title, sizeof(title), "Minix FM v9 - Dual Panel - %s",
             active_panel == &left_panel ? "LEFT" : "RIGHT");
    XDrawString(dpy, win, gc, MARGIN, MARGIN + fontinfo->ascent, title, strlen(title));

    /* Draw vertical separator */
    XSetForeground(dpy, gc, sel_bg_color);
    XFillRectangle(dpy, win, gc, separator_x - PANEL_SEPARATOR_WIDTH/2, 0, PANEL_SEPARATOR_WIDTH, win_h);
    XSetForeground(dpy, gc, fg_color);

    /* Left panel */
    int y = MARGIN + line_height * 2;
    int i;
    for (i = left_panel.scroll_offset; i < left_panel.nentries; ++i) {
        if (y + line_height > win_h) break;
        Entry *e = &left_panel.entries[i];
        if (i == left_panel.selected_idx && active_panel == &left_panel) {
            XSetForeground(dpy, gc, sel_bg_color);
            XFillRectangle(dpy, win, gc, MARGIN, y - line_height + 4, panel_width - 2*MARGIN, line_height);
            XSetForeground(dpy, gc, fg_color);
        }
        char buf[512];
        snprintf(buf, sizeof(buf), "%-11s %s", e->perms, e->name);
        XDrawString(dpy, win, gc, MARGIN, y, buf, strlen(buf));
        y += line_height;
    }

    /* Right panel */
    y = MARGIN + line_height * 2;
    for (i = right_panel.scroll_offset; i < right_panel.nentries; ++i) {
        if (y + line_height > win_h) break;
        Entry *e = &right_panel.entries[i];
        int x = separator_x + PANEL_SEPARATOR_WIDTH/2 + MARGIN;
        if (i == right_panel.selected_idx && active_panel == &right_panel) {
            XSetForeground(dpy, gc, sel_bg_color);
            XFillRectangle(dpy, win, gc, separator_x + PANEL_SEPARATOR_WIDTH/2 + MARGIN, y - line_height + 4,
                           panel_width - 2*MARGIN, line_height);
            XSetForeground(dpy, gc, fg_color);
        }
        char buf[512];
        snprintf(buf, sizeof(buf), "%-11s %s", e->perms, e->name);
        XDrawString(dpy, win, gc, x, y, buf, strlen(buf));
        y += line_height;
    }

    /* mode button / footer */
    char footer[128];
    snprintf(footer, sizeof(footer), "[T] Theme  [P] Chmod  [C/M/F] Transfer  [F1] Help");
    XDrawString(dpy, win, gc, MARGIN, win_h - MARGIN, footer, strlen(footer));

    XFlush(dpy);
}

/* simple process list updater (keeps old behavior) */
static void update_process_list(void) {
    int i;
    for (i = 0; i < nprocesses; ++i) {
        if (kill(processes[i].pid, 0) != 0) {
            int j;
            for (j = i; j < nprocesses - 1; ++j) processes[j] = processes[j+1];
            nprocesses--; i--;
        }
    }
}

/* copy file utility */
static int copy_file(const char *src, const char *dst) {
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
    close(ifd); close(ofd);
    return (r == 0) ? 0 : -1;
}

/* helper: build path safely */
static void build_path(char *out, size_t outlen, const char *cwd, const char *name) {
    if (!cwd || !name) { out[0] = '\0'; return; }
    if (strcmp(cwd, "/") == 0) snprintf(out, outlen, "/%s", name);
    else snprintf(out, outlen, "%s/%s", cwd, name);
}

/* Transfer dialog: modal; supports rename; robust expose handling */
static int transfer_dialog(Window parent, const char *src_name, char *out_newname, int out_newname_len) {
    /* returns: 0 = cancel, 1 = move, 2 = copy */
    int dialog_w = 500, dialog_h = 160;
    int sx = (win_w - dialog_w)/2, sy = (win_h - dialog_h)/2;
    Window dlg = XCreateSimpleWindow(dpy, parent, sx, sy, dialog_w, dialog_h, 2, fg_color, bg_color);
    XSelectInput(dpy, dlg, ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask);
    XMapWindow(dpy, dlg);
    XFlush(dpy);

    /* initial name empty => keep original */
    out_newname[0] = '\0';

    int button_w = 100, button_h = 28;
    int bx_move = dialog_w/2 - button_w - 10;
    int bx_copy = dialog_w/2 + 10;
    int bx_cancel = dialog_w - button_w - 10;
    int by = dialog_h - button_h - 15;

    int result = 0;
    int running = 1;

    /* draw function for dialog */
    auto void_draw = 0; /* placeholder - C doesn't allow nested functions with types easily */
    /* we'll implement redrawing inline when handling Expose */

    while (running) {
        XEvent ev;
        XNextEvent(dpy, &ev);
        if (ev.type == Expose && ev.xexpose.window == dlg) {
            /* clear and draw background */
            XSetForeground(dpy, gc, bg_color);
            XFillRectangle(dpy, dlg, gc, 0, 0, dialog_w, dialog_h);
            XSetForeground(dpy, gc, fg_color);

            /* title */
            char tbuf[512];
            snprintf(tbuf, sizeof(tbuf), "Transfer: %s", src_name);
            XDrawString(dpy, dlg, gc, 10, 20 + fontinfo->ascent, tbuf, strlen(tbuf));

            /* prompt */
            XDrawString(dpy, dlg, gc, 10, 20 + line_height + fontinfo->ascent, "New name (leave empty to keep):", 31);

            /* input box */
            int iy = 20 + line_height + fontinfo->ascent + 8;
            XSetForeground(dpy, gc, fg_color);
            XDrawRectangle(dpy, dlg, gc, 10, iy, dialog_w - 20, line_height + 4);
            if (out_newname[0] == '\0') {
                char ph[512];
                snprintf(ph, sizeof(ph), "(keep: %s)", src_name);
                XDrawString(dpy, dlg, gc, 12, iy + fontinfo->ascent, ph, strlen(ph));
            } else {
                XDrawString(dpy, dlg, gc, 12, iy + fontinfo->ascent, out_newname, strlen(out_newname));
            }

            /* buttons */
            XDrawRectangle(dpy, dlg, gc, bx_move, by, button_w, button_h);
            XDrawString(dpy, dlg, gc, bx_move + 20, by + fontinfo->ascent + 6, "Move", 4);

            XDrawRectangle(dpy, dlg, gc, bx_copy, by, button_w, button_h);
            XDrawString(dpy, dlg, gc, bx_copy + 20, by + fontinfo->ascent + 6, "Copy", 4);

            XDrawRectangle(dpy, dlg, gc, bx_cancel, by, button_w, button_h);
            XDrawString(dpy, dlg, gc, bx_cancel + 14, by + fontinfo->ascent + 6, "Cancel", 6);

            XFlush(dpy);
        } else if (ev.type == ButtonPress && ev.xbutton.window == dlg) {
            int rx = ev.xbutton.x, ry = ev.xbutton.y;
            if (rx >= bx_move && rx <= bx_move + button_w && ry >= by && ry <= by + button_h) {
                result = 1; running = 0;
            } else if (rx >= bx_copy && rx <= bx_copy + button_w && ry >= by && ry <= by + button_h) {
                result = 2; running = 0;
            } else if (rx >= bx_cancel && rx <= bx_cancel + button_w && ry >= by && ry <= by + button_h) {
                result = 0; running = 0;
            } else {
                /* click in input area => give focus (we already accept KeyPress) */
                /* nothing to do - keypress handler will append chars */
            }
        } else if (ev.type == KeyPress && ev.xkey.window == dlg) {
            KeySym ks = XLookupKeysym(&ev.xkey, 0);
            if (ks == XK_Return) { result = 1; running = 0; }
            else if (ks == XK_Escape) { result = 0; running = 0; }
            else if (ks == XK_BackSpace) {
                int L = strlen(out_newname);
                if (L > 0) out_newname[L-1] = '\0';
                /* force redraw */
                XClearWindow(dpy, dlg);
                XFlush(dpy);
            } else {
                char buf[32];
                int n = XLookupString(&ev.xkey, buf, sizeof(buf), NULL, NULL);
                for (int i = 0; i < n; ++i) {
                    char c = buf[i];
                    if (c >= 32 && c < 127 && (int)strlen(out_newname) + 1 < out_newname_len) {
                        int L = strlen(out_newname);
                        out_newname[L] = c; out_newname[L+1] = '\0';
                    }
                }
                XClearWindow(dpy, dlg);
                XFlush(dpy);
            }
        } else if (ev.type == ConfigureNotify && ev.xconfigure.window == dlg) {
            /* handle resize if needed */
        }
    }

    XDestroyWindow(dpy, dlg);
    XFlush(dpy);
    return result;
}

/* perform transfer between panels */
static void perform_transfer(Panel *src, Panel *dst, int src_idx, int do_move, const char *newname) {
    char src_path[1024], dst_path[1024];
    const char *orig_name = src->entries[src_idx].name;
    const char *final = (newname && strlen(newname) > 0) ? newname : orig_name;
    build_path(src_path, sizeof(src_path), src->cwd, orig_name);
    build_path(dst_path, sizeof(dst_path), dst->cwd, final);

    if (src->entries[src_idx].is_dir) {
        fprintf(stderr, "Directory transfer not supported in this simplified implementation\n");
        return;
    }

    if (do_move) {
        if (rename(src_path, dst_path) == 0) {
            read_dir(src, src->cwd);
            read_dir(dst, dst->cwd);
            draw_interface();
            return;
        } else {
            /* fallback copy + unlink (handle EXDEV) */
            if (copy_file(src_path, dst_path) == 0) {
                if (unlink(src_path) == 0) {
                    read_dir(src, src->cwd);
                    read_dir(dst, dst->cwd);
                    draw_interface();
                    return;
                } else {
                    perror("unlink");
                    /* cleanup target */
                    unlink(dst_path);
                    return;
                }
            } else {
                perror("copy_file");
                return;
            }
        }
    } else {
        if (copy_file(src_path, dst_path) == 0) {
            read_dir(dst, dst->cwd);
            draw_interface();
            return;
        } else {
            perror("copy_file");
            return;
        }
    }
}

/* Help dialog with robust expose/redraw handling */
static void show_help_dialog(void) {
    int dialog_w = 600, dialog_h = 400;
    int sx = (win_w - dialog_w)/2, sy = (win_h - dialog_h)/2;
    Window dlg = XCreateSimpleWindow(dpy, win, sx, sy, dialog_w, dialog_h, 2, fg_color, bg_color);
    XSelectInput(dpy, dlg, ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask);
    XMapWindow(dpy, dlg);
    XFlush(dpy);

    int running = 1;
    while (running) {
        XEvent ev;
        XNextEvent(dpy, &ev);
        if (ev.type == Expose && ev.xexpose.window == dlg) {
            /* draw background */
            XSetForeground(dpy, gc, bg_color);
            XFillRectangle(dpy, dlg, gc, 0, 0, dialog_w, dialog_h);
            XSetForeground(dpy, gc, fg_color);

            int y = 10 + fontinfo->ascent;
            XDrawString(dpy, dlg, gc, 10, y, "Minix FM - Help", 14);
            y += line_height;
            const char *lines[] = {
                "Tab / Left/Right - switch panels",
                "Up/Down - navigate",
                "Enter - open",
                "C/M/F - Copy/Move (shows dialog with rename)",
                "P - change permissions of selected file",
                "T - toggle theme (light/dark)",
                "F1 or ? - this help",
                "Esc - close this help"
            };
            for (int i = 0; i < (int)(sizeof(lines)/sizeof(lines[0])); ++i) {
                y += line_height;
                XDrawString(dpy, dlg, gc, 10, y, lines[i], strlen(lines[i]));
            }
            XFlush(dpy);
        } else if (ev.type == KeyPress && ev.xkey.window == dlg) {
            KeySym ks = XLookupKeysym(&ev.xkey, 0);
            if (ks == XK_Escape || ks == XK_q || ks == XK_Q) running = 0;
        } else if (ev.type == ButtonPress && ev.xbutton.window == dlg) {
            running = 0;
        }
    }

    XDestroyWindow(dpy, dlg);
    XFlush(dpy);
}

/* chmod dialog: change permissions */
static void chmod_dialog(const char *filepath, mode_t old_mode) {
    int dialog_w = 400, dialog_h = 220;
    int sx = (win_w - dialog_w)/2, sy = (win_h - dialog_h)/2;
    Window dlg = XCreateSimpleWindow(dpy, win, sx, sy, dialog_w, dialog_h, 2, fg_color, bg_color);
    XSelectInput(dpy, dlg, ExposureMask | ButtonPressMask | KeyPressMask);
    XMapWindow(dpy, dlg);
    XFlush(dpy);

    /* r/w/x for u/g/o (bits) */
    int flags[9] = {
        (old_mode & S_IRUSR) ? 1 : 0,
        (old_mode & S_IWUSR) ? 1 : 0,
        (old_mode & S_IXUSR) ? 1 : 0,
        (old_mode & S_IRGRP) ? 1 : 0,
        (old_mode & S_IWGRP) ? 1 : 0,
        (old_mode & S_IXGRP) ? 1 : 0,
        (old_mode & S_IROTH) ? 1 : 0,
        (old_mode & S_IWOTH) ? 1 : 0,
        (old_mode & S_IXOTH) ? 1 : 0,
    };

    const char *labels[9] = { "r", "w", "x", "r", "w", "x", "r", "w", "x" };
    const char *groups[3] = { "User:", "Group:", "Other:" };

    int label_x = 10;
    int box_x_start = 80;
    int box_y_start = 40;
    int box_size = 20;

    int running = 1;
    int result = 0; // 0 = cancel, 1 = apply

    while (running) {
        XEvent ev;
        XNextEvent(dpy, &ev);
        if (ev.type == Expose && ev.xexpose.window == dlg) {
            XSetForeground(dpy, gc, bg_color);
            XFillRectangle(dpy, dlg, gc, 0, 0, dialog_w, dialog_h);
            XSetForeground(dpy, gc, fg_color);

            XDrawString(dpy, dlg, gc, 10, 20 + fontinfo->ascent, "Change permissions", 19);

            /* Draw permission boxes */
            for (int g = 0; g < 3; g++) {
                int y = box_y_start + g * (box_size + 10);
                XDrawString(dpy, dlg, gc, label_x, y + fontinfo->ascent, groups[g], strlen(groups[g]));
                for (int p = 0; p < 3; p++) {
                    int idx = g * 3 + p;
                    int x = box_x_start + p * (box_size * 2);
                    XDrawRectangle(dpy, dlg, gc, x, y, box_size, box_size);
                    if (flags[idx]) {
                        XDrawString(dpy, dlg, gc, x + 7, y + fontinfo->ascent, labels[idx], strlen(labels[idx]));
                    }
                }
            }

            /* Buttons */
            int by = dialog_h - 40;
            XDrawRectangle(dpy, dlg, gc, 50, by, 100, 30);
            XDrawString(dpy, dlg, gc, 78, by + fontinfo->ascent + 8, "Apply", 5);

            XDrawRectangle(dpy, dlg, gc, 200, by, 100, 30);
            XDrawString(dpy, dlg, gc, 218, by + fontinfo->ascent + 8, "Cancel", 6);

            XFlush(dpy);
        } else if (ev.type == ButtonPress && ev.xbutton.window == dlg) {
            int x = ev.xbutton.x;
            int y = ev.xbutton.y;

            /* Handle clicks on permission boxes */
            for (int g = 0; g < 3; g++) {
                int yy = box_y_start + g * (box_size + 10);
                for (int p = 0; p < 3; p++) {
                    int idx = g * 3 + p;
                    int xx = box_x_start + p * (box_size * 2);
                    if (x >= xx && x <= xx + box_size && y >= yy && y <= yy + box_size) {
                        flags[idx] = !flags[idx];
                        XClearWindow(dpy, dlg);
                        XFlush(dpy);
                    }
                }
            }

            /* Apply */
            if (x >= 50 && x <= 150 && y >= (dialog_h - 40) && y <= (dialog_h - 10)) {
                result = 1;
                running = 0;
            }

            /* Cancel */
            if (x >= 200 && x <= 300 && y >= (dialog_h - 40) && y <= (dialog_h - 10)) {
                result = 0;
                running = 0;
            }
        } else if (ev.type == KeyPress && ev.xkey.window == dlg) {
            KeySym ks = XLookupKeysym(&ev.xkey, 0);
            if (ks == XK_Escape) { result = 0; running = 0; }
        }
    }

    if (result == 1) {
        mode_t new_mode = 0;
        if (flags[0]) new_mode |= S_IRUSR;
        if (flags[1]) new_mode |= S_IWUSR;
        if (flags[2]) new_mode |= S_IXUSR;
        if (flags[3]) new_mode |= S_IRGRP;
        if (flags[4]) new_mode |= S_IWGRP;
        if (flags[5]) new_mode |= S_IXGRP;
        if (flags[6]) new_mode |= S_IROTH;
        if (flags[7]) new_mode |= S_IWOTH;
        if (flags[8]) new_mode |= S_IXOTH;

        if (chmod(filepath, new_mode) != 0) {
            perror("chmod failed");
        }
    }

    XDestroyWindow(dpy, dlg);
    XFlush(dpy);
}

/* --- Key handling --- */
static void handle_key(XKeyEvent *key) {
    KeySym ks = XLookupKeysym(key, 0);
    int ctrl = key->state & ControlMask;

    switch (ks) {
        case XK_Tab: /* Switch panels */
        case XK_Left:
        case XK_Right:
            active_panel = (active_panel == &left_panel) ? &right_panel : &left_panel;
            draw_interface();
            break;

        case XK_Up:
            if (active_panel->selected_idx > 0) active_panel->selected_idx--;
            draw_interface();
            break;

        case XK_Down:
            if (active_panel->selected_idx < active_panel->nentries - 1) active_panel->selected_idx++;
            draw_interface();
            break;

        case XK_Return:
            /* Open selected entry */
            if (active_panel->selected_idx >= 0 &&
                !active_panel->entries[active_panel->selected_idx].is_dir) {
                char filepath[1024];
                build_path(filepath, sizeof(filepath), active_panel->cwd,
                           active_panel->entries[active_panel->selected_idx].name);
                execlp("xterm", "xterm", "-e", "vi", filepath, NULL);
            }
            break;

        /* Show help */
        case XK_F1:
        case XK_question:
            show_help_dialog();
            draw_interface();
            break;

        /* Copy/Move */
        case XK_c:
        case XK_m: {
            if (active_panel->selected_idx < 0) break;
            Panel *src = active_panel;
            Panel *dst = (active_panel == &left_panel) ? &right_panel : &left_panel;
            char newname[256];
            int choice = transfer_dialog(win, src->entries[src->selected_idx].name,
                                         newname, sizeof(newname));
            if (choice != 0) perform_transfer(src, dst, src->selected_idx, (choice == 1), newname);
            break;
        }

        /* Change chmod */
        case XK_p:
        case XK_P:
            if (active_panel->selected_idx >= 0) {
                char filepath[1024];
                build_path(filepath, sizeof(filepath), active_panel->cwd,
                           active_panel->entries[active_panel->selected_idx].name);
                chmod_dialog(filepath, active_panel->entries[active_panel->selected_idx].mode);
                read_dir(active_panel, active_panel->cwd);
                draw_interface();
            }
            break;

        /* Change theme */
        case XK_t:
        case XK_T:
            switch_theme();
            break;

        /* Quit */
        case XK_q:
        case XK_Q:
            if (ctrl) exit(0);
            break;
    }
}

/* ---------------------------------------------
 * MAIN & завершение
 * ---------------------------------------------
 */

int main(int argc, char **argv) {
    printf("Minix File Manager v9 - Starting...\n");
    printf("Press F1 or ? for help\n");

    /* Initialize directory panels */
    nprocesses = 0;
    char start_cwd[1024];
    if (!getcwd(start_cwd, sizeof(start_cwd))) strcpy(start_cwd, "/");

    init_panel(&left_panel, start_cwd);
    init_panel(&right_panel, start_cwd);
    active_panel = &left_panel;

    /* X init */
    dpy = XOpenDisplay(NULL);
    if (!dpy) {
        fprintf(stderr, "Error: can't open X display.\n");
        return 1;
    }

    int screen = DefaultScreen(dpy);
    win = XCreateSimpleWindow(
        dpy, RootWindow(dpy, screen),
        0, 0, win_w, win_h, 1,
        BlackPixel(dpy, screen), WhitePixel(dpy, screen)
    );

    XSizeHints hints;
    hints.flags = PMinSize;
    hints.min_width = 600;
    hints.min_height = 400;
    XSetNormalHints(dpy, win, &hints);

    XSelectInput(dpy, win,
        ExposureMask |
        ButtonPressMask |
        KeyPressMask |
        StructureNotifyMask
    );

    XStoreName(dpy, win, "Minix FM v9 - Dual Panel");
    XMapWindow(dpy, win);

    /* Load font */
    fontinfo = XLoadQueryFont(dpy, "9x15");
    if (!fontinfo) fontinfo = XLoadQueryFont(dpy, "10x20");
    if (!fontinfo) fontinfo = XLoadQueryFont(dpy, "fixed");
    if (!fontinfo) fontinfo = XLoadQueryFont(dpy, "6x13");

    gc = XCreateGC(dpy, win, 0, NULL);
    XSetFont(dpy, gc, fontinfo->fid);
    line_height = fontinfo->ascent + fontinfo->descent + 4;

    apply_theme();
    draw_interface();

    XEvent ev;
    while (1) {
        XNextEvent(dpy, &ev);

        if (ev.type == Expose)
            draw_interface();

        else if (ev.type == ButtonPress) {
            if (ev.xbutton.button == Button4) {
                /* scroll up */
                if (active_panel->scroll_offset > 0)
                    active_panel->scroll_offset--;
                draw_interface();
            } else if (ev.xbutton.button == Button5) {
                /* scroll down */
                if (active_panel->scroll_offset < active_panel->nentries - 1)
                    active_panel->scroll_offset++;
                draw_interface();
            } else {
                /* Single click selection */
                int panel_width = win_w / 2;
                Panel *clicked_panel = (ev.xbutton.x < panel_width) ? &left_panel : &right_panel;
                if (clicked_panel != active_panel) {
                    active_panel = clicked_panel;
                    draw_interface();
                    continue;
                }
                int idx = (ev.xbutton.y - (MARGIN + line_height * 2)) / line_height
                          + active_panel->scroll_offset;
                if (idx >= 0 && idx < active_panel->nentries) {
                    active_panel->selected_idx = idx;
                    draw_interface();
                }
            }
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

/* ---------------------------------------------
 * END
 * ---------------------------------------------
 */

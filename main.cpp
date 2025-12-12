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
#define MAX_ENTRIES 2000
#define MAX_PATH 1024

/* Theme colors (simple 24-bit values) */
static unsigned long LIGHT_BG = 0xFFFFFF;
static unsigned long LIGHT_FG = 0x000000;
static unsigned long LIGHT_SEL = 0xC0C0C0;
static unsigned long DARK_BG  = 0x202020;
static unsigned long DARK_FG  = 0xFFFFFF;
static unsigned long DARK_SEL  = 0x404040;

/* Data structures */
typedef struct Entry {
    char name[256];
    int is_dir;
    char perms[12];
    mode_t mode;
    char type_label[8];
} Entry;

typedef struct Panel {
	int sorted;                        
	Entry original_entries[MAX_ENTRIES];
	int original_nentries;
    Entry entries[MAX_ENTRIES];
    int nentries;
    char cwd[MAX_PATH];
    int selected_idx;
    int scroll_offset;
    int has_focus;
} Panel;

typedef struct ProcessInfo {
    pid_t pid;
    char command[256];
    time_t start_time;
} ProcessInfo;

/* Global state */
static Display *dpy = NULL;
static Window win = 0;
static GC gc = 0;
static XFontStruct *fontinfo = NULL;
static int line_height = 16;
static int win_w = 1024;
static int win_h = 700;
static int separator_x = 0;

static Panel left_panel;
static Panel right_panel;
static Panel *active_panel = NULL;

static ProcessInfo processes[MAX_PROCESSES];
static int nprocesses = 0;
static int show_processes = 0;

/* Theme state */
static int dark_mode = 0;
static unsigned long bg_color, fg_color, sel_bg_color;

/* Function prototypes */
static void apply_theme(void);
static void draw_interface(void);
static void read_dir(Panel *panel, const char *path);
static void init_panel(Panel *panel, const char *initial_path);
static void update_process_list(void);
static void add_process(pid_t pid, const char *cmd);
static int copy_file(const char *src, const char *dst);
static void build_path(char *out, size_t outlen, const char *cwd, const char *name);
static void scroll_panel(Panel *panel, int direction);


/* Dialog prototypes */
static int transfer_dialog(Window parent, const char *src_name, char *out_newname, int out_newname_len);
static void show_help_dialog(void);
static void chmod_dialog(const char *filepath, mode_t old_mode);

/* Input / actions */
static void perform_transfer(Panel *src, Panel *dst, int src_idx, int do_move, const char *newname);
static void initiate_transfer_from_active(void);
static void handle_key(XKeyEvent *key);
static void handle_click(int x, int y, int button);

/* Utils */
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
    out[10]= '\0';
}

/* is regular text file heuristic */
static int is_text_file(const char *path) {
    struct stat st;
    if (stat(path, &st) != 0) return 0;
    if (!S_ISREG(st.st_mode)) return 0;
    FILE *f = fopen(path, "rb");
    if (!f) return 0;
    unsigned char buf[256];
    size_t n = fread(buf, 1, sizeof(buf), f);
    fclose(f);
    if (n == 0) return 1;
    for (size_t i = 0; i < n; ++i) {
        unsigned char c = buf[i];
        if (c == 9 || c == 10 || c == 13) continue;
        if (c < 32 || c > 126) return 0;
    }
    return 1;
}

static void scroll_panel(Panel *panel, int direction) {
    if (!panel) return;
    panel->scroll_offset += direction;
    if (panel->scroll_offset < 0) panel->scroll_offset = 0;
    if (panel->scroll_offset >= panel->nentries)
        panel->scroll_offset = panel->nentries - 1;
    draw_interface();
}


/* Apply theme colors to globals and GC */
static void apply_theme(void) {
    if (dark_mode) {
        bg_color = DARK_BG;
        fg_color = DARK_FG;
        sel_bg_color = DARK_SEL;
    } else {
        bg_color = LIGHT_BG;
        fg_color = LIGHT_FG;
        sel_bg_color = LIGHT_SEL;
    }
    if (dpy && gc) {
        XSetForeground(dpy, gc, fg_color);
        XSetBackground(dpy, gc, bg_color);
    }
    /* If main window exists, set its background to our theme and clear */
    if (dpy && win) {
        XSetWindowBackground(dpy, win, bg_color);
        XSetWindowBorder(dpy, win, fg_color);
        XClearWindow(dpy, win);
        XFlush(dpy);
    }
}

/* Initialize single panel */
static void init_panel(Panel *panel, const char *initial_path) {
    if (!panel) return;
    memset(panel, 0, sizeof(Panel));
    panel->selected_idx = -1;
    panel->scroll_offset = 0;
    panel->has_focus = 0;
    if (initial_path && initial_path[0]) strncpy(panel->cwd, initial_path, sizeof(panel->cwd)-1);
    else if (getcwd(panel->cwd, sizeof(panel->cwd)) == NULL) strcpy(panel->cwd, "/");
    read_dir(panel, panel->cwd);
}

/* Read directory into panel entries */
static void read_dir(Panel *panel, const char *path) {
    if (!panel || !path) return;
    DIR *d = opendir(path);
    if (!d) {
        perror("opendir");
        panel->nentries = 0;
        return;
    }
    struct dirent *de;
    int idx = 0;
    if (strcmp(path, "/") != 0) {
        strncpy(panel->entries[idx].name, "..", sizeof(panel->entries[idx].name)-1);
        panel->entries[idx].is_dir = 1;
        strncpy(panel->entries[idx].type_label, "[DIR]", sizeof(panel->entries[idx].type_label)-1);
        panel->entries[idx].mode = S_IFDIR | 0755;
        strncpy(panel->entries[idx].perms, "drwxr-xr-x", sizeof(panel->entries[idx].perms)-1);
        idx = 1;
    }
    char full[MAX_PATH];
    while ((de = readdir(d)) != NULL && idx < MAX_ENTRIES) {
        if (strcmp(de->d_name, ".") == 0) continue;
        snprintf(full, sizeof(full), "%s/%s", path, de->d_name);
        struct stat st;
        if (stat(full, &st) == 0) {
            Entry *e = &panel->entries[idx];
            strncpy(e->name, de->d_name, sizeof(e->name)-1);
            e->name[sizeof(e->name)-1] = '\0';
            e->is_dir = S_ISDIR(st.st_mode);
            e->mode = st.st_mode;
            mode_to_str(st.st_mode, e->perms);
            if (e->is_dir) strncpy(e->type_label, "[DIR]", sizeof(e->type_label)-1);
            else {
                if (S_ISREG(st.st_mode)) {
                    if (is_text_file(full)) strncpy(e->type_label, "[TXT]", sizeof(e->type_label)-1);
                    else strncpy(e->type_label, "[BIN]", sizeof(e->type_label)-1);
                } else strncpy(e->type_label, "[?]", sizeof(e->type_label)-1);
            }
            idx++;
        }
    }
    closedir(d);
    panel->nentries = idx;
    panel->selected_idx = (panel->nentries > 0) ? 0 : -1;
	
	memcpy(panel->original_entries, panel->entries,
		   sizeof(Entry) * panel->nentries);
	panel->original_nentries = panel->nentries;
	
	panel->sorted = 0;
}

/* Update process list by culling dead pids */
static void update_process_list(void) {
    int i;
    for (i = 0; i < nprocesses; ++i) {
        if (kill(processes[i].pid, 0) != 0) {
            int j;
            for (j = i; j < nprocesses - 1; ++j) processes[j] = processes[j+1];
            nprocesses--;
            i--;
        }
    }
}

/* Add background process info to list */
static void add_process(pid_t pid, const char *cmd) {
    if (nprocesses >= MAX_PROCESSES) {
        /* drop oldest */
        int i;
        for (i = 0; i < nprocesses - 1; ++i) processes[i] = processes[i+1];
        nprocesses--;
    }
    processes[nprocesses].pid = pid;
    strncpy(processes[nprocesses].command, cmd, sizeof(processes[nprocesses].command)-1);
    processes[nprocesses].start_time = time(NULL);
    nprocesses++;
}

/* Copy file utility */
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
    close(ifd);
    close(ofd);
    return (r == 0) ? 0 : -1;
}

/* Build a path string safely */
static void build_path(char *out, size_t outlen, const char *cwd, const char *name) {
    if (!out || !cwd || !name) return;
    if (strcmp(cwd, "/") == 0) snprintf(out, outlen, "/%s", name);
    else snprintf(out, outlen, "%s/%s", cwd, name);
}

/* Transfer dialog: modal window for Move/Copy/Cancel with rename input */
/* Returns: 0 = Cancel, 1 = Move, 2 = Copy. out_newname filled (empty => keep original) */
static int transfer_dialog(Window parent, const char *src_name, char *out_newname, int out_newname_len) {
    if (!dpy) return 0;
    int dialog_w = 520;
    int dialog_h = 160;
    int sx = (win_w - dialog_w) / 2;
    int sy = (win_h - dialog_h) / 2;
    Window dlg = XCreateSimpleWindow(dpy, parent, sx, sy, dialog_w, dialog_h, 2, fg_color, bg_color);

    /* ensure background/border use theme */
    XSetWindowBackground(dpy, dlg, bg_color);
    XSetWindowBorder(dpy, dlg, fg_color);

    /* Select for expose, key, button events */
    XSelectInput(dpy, dlg, ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask);
    XMapWindow(dpy, dlg);
    XClearWindow(dpy, dlg);
    XFlush(dpy);

    /* initialize output */
    if (out_newname && out_newname_len > 0) out_newname[0] = '\0';

    int button_w = 100, button_h = 28;
    int bx_move = dialog_w/2 - button_w - 10;
    int bx_copy = dialog_w/2 + 10;
    int bx_cancel = dialog_w - button_w - 10;
    int by = dialog_h - button_h - 15;

    int running = 1;
    int result = 0;

    /* Input cursor state */
    int cursor_pos = 0;

    while (running) {
        XEvent ev;
        XNextEvent(dpy, &ev);

        if (ev.type == Expose && ev.xexpose.window == dlg) {
            /* draw background */
            XSetForeground(dpy, gc, bg_color);
            XFillRectangle(dpy, dlg, gc, 0, 0, dialog_w, dialog_h);
            XSetForeground(dpy, gc, fg_color);

            /* Title */
            char title[512];
            snprintf(title, sizeof(title), "Transfer: %s", src_name);
            XDrawString(dpy, dlg, gc, 10, 20 + fontinfo->ascent, title, strlen(title));

            /* Prompt */
            XDrawString(dpy, dlg, gc, 10, 20 + line_height + fontinfo->ascent, "New name (leave empty to keep):", 31);

            /* Input box */
            int iy = 20 + line_height + fontinfo->ascent + 8;
            XDrawRectangle(dpy, dlg, gc, 10, iy, dialog_w - 20, line_height + 4);

            /* Show input or placeholder */
            if (out_newname && out_newname[0] != '\0') {
                XDrawString(dpy, dlg, gc, 12, iy + fontinfo->ascent, out_newname, strlen(out_newname));
            } else {
                char ph[512];
                snprintf(ph, sizeof(ph), "(keep: %s)", src_name);
                XDrawString(dpy, dlg, gc, 12, iy + fontinfo->ascent, ph, strlen(ph));
            }

            /* Draw buttons */
            XSetForeground(dpy, gc, fg_color);
            XDrawRectangle(dpy, dlg, gc, bx_move, by, button_w, button_h);
            XDrawString(dpy, dlg, gc, bx_move + 20, by + fontinfo->ascent + 6, "Move", 4);

            XDrawRectangle(dpy, dlg, gc, bx_copy, by, button_w, button_h);
            XDrawString(dpy, dlg, gc, bx_copy + 20, by + fontinfo->ascent + 6, "Copy", 4);

            XDrawRectangle(dpy, dlg, gc, bx_cancel, by, button_w, button_h);
            XDrawString(dpy, dlg, gc, bx_cancel + 14, by + fontinfo->ascent + 6, "Cancel", 6);

            XFlush(dpy);
        }
        else if (ev.type == ButtonPress && ev.xbutton.window == dlg) {
            int rx = ev.xbutton.x;
            int ry = ev.xbutton.y;
            if (rx >= bx_move && rx <= bx_move + button_w && ry >= by && ry <= by + button_h) {
                result = 1; running = 0;
            } else if (rx >= bx_copy && rx <= bx_copy + button_w && ry >= by && ry <= by + button_h) {
                result = 2; running = 0;
            } else if (rx >= bx_cancel && rx <= bx_cancel + button_w && ry >= by && ry <= by + button_h) {
                result = 0; running = 0;
            }
        }
        else if (ev.type == KeyPress && ev.xkey.window == dlg) {
            KeySym ks = XLookupKeysym(&ev.xkey, 0);
            if (ks == XK_Return) {
                result = 1; running = 0;
            } else if (ks == XK_Escape) {
                result = 0; running = 0;
            } else if (ks == XK_BackSpace) {
                if (out_newname && out_newname[0] != '\0') {
                    int L = strlen(out_newname);
                    if (L > 0) out_newname[L-1] = '\0';
                }
                XClearWindow(dpy, dlg);
                XFlush(dpy);
            } else {
                char buf[32];
                int n = XLookupString(&ev.xkey, buf, sizeof(buf), NULL, NULL);
                for (int i = 0; i < n; ++i) {
                    char c = buf[i];
                    if (c >= 32 && c < 127 && out_newname && (int)strlen(out_newname) + 1 < out_newname_len) {
                        int L = strlen(out_newname);
                        out_newname[L] = c;
                        out_newname[L+1] = '\0';
                    }
                }
                XClearWindow(dpy, dlg);
                XFlush(dpy);
            }
        }
        else if (ev.type == ConfigureNotify && ev.xconfigure.window == dlg) {
            /* ignore resize notifications */
        }
    }

    XDestroyWindow(dpy, dlg);
    XFlush(dpy);
    return result;
}

/* Perform transfer: tries rename, on failure falls back to copy + unlink */
static void perform_transfer(Panel *src, Panel *dst, int src_idx, int do_move, const char *newname) {
    if (!src || !dst || src_idx < 0 || src_idx >= src->nentries) return;
    if (src->entries[src_idx].is_dir) {
        /* no recursive directory handling in this implementation */
        fprintf(stderr, "Directory transfer not supported.\n");
        return;
    }
    char src_path[MAX_PATH], dst_path[MAX_PATH];
    const char *orig = src->entries[src_idx].name;
    const char *final_name = (newname && strlen(newname) > 0) ? newname : orig;
    build_path(src_path, sizeof(src_path), src->cwd, orig);
    build_path(dst_path, sizeof(dst_path), dst->cwd, final_name);

    if (do_move) {
        if (rename(src_path, dst_path) == 0) {
            read_dir(src, src->cwd);
            read_dir(dst, dst->cwd);
            draw_interface();
            return;
        } else {
            /* fallback copy/unlink */
            if (copy_file(src_path, dst_path) == 0) {
                if (unlink(src_path) == 0) {
                    read_dir(src, src->cwd);
                    read_dir(dst, dst->cwd);
                    draw_interface();
                    return;
                } else {
                    perror("unlink after copy failed");
                    /* attempt to remove target to avoid duplicates */
                    unlink(dst_path);
                    return;
                }
            } else {
                perror("copy_file failed during move fallback");
                return;
            }
        }
    } else {
        if (copy_file(src_path, dst_path) == 0) {
            read_dir(dst, dst->cwd);
            draw_interface();
            return;
        } else {
            perror("copy_file failed");
            return;
        }
    }
}

/* Helper to initiate transfer from active panel -> other panel */
static void initiate_transfer_from_active(void) {
    if (!active_panel) return;
    int idx = active_panel->selected_idx;
    if (idx < 0 || idx >= active_panel->nentries) return;
    if (active_panel->entries[idx].is_dir) {
        fprintf(stderr, "Directory transfer not supported.\n");
        return;
    }
    Panel *src = active_panel;
    Panel *dst = (active_panel == &left_panel) ? &right_panel : &left_panel;
    char newname[512] = {0};
    int choice = transfer_dialog(win, src->entries[idx].name, newname, sizeof(newname));
    if (choice == 0) return;
    perform_transfer(src, dst, idx, (choice == 1), newname);
}

/* Help dialog: simple text window with proper expose handling */
static void show_help_dialog(void) {
    if (!dpy) return;
    int dialog_w = 560, dialog_h = 340;
    int sx = (win_w - dialog_w) / 2;
    int sy = (win_h - dialog_h) / 2;
    Window dlg = XCreateSimpleWindow(dpy, win, sx, sy, dialog_w, dialog_h, 2, fg_color, bg_color);
    XSetWindowBackground(dpy, dlg, bg_color);
    XSetWindowBorder(dpy, dlg, fg_color);
    XSelectInput(dpy, dlg, ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask);
    XMapWindow(dpy, dlg);
    XClearWindow(dpy, dlg);
    XFlush(dpy);

    int running = 1;
    while (running) {
        XEvent ev;
        XNextEvent(dpy, &ev);
        if (ev.type == Expose && ev.xexpose.window == dlg) {
            XSetForeground(dpy, gc, bg_color);
            XFillRectangle(dpy, dlg, gc, 0, 0, dialog_w, dialog_h);
            XSetForeground(dpy, gc, fg_color);

            int y = 10 + fontinfo->ascent;
            XDrawString(dpy, dlg, gc, 10, y, "Minix FM - Help", 14);
            y += line_height;
            const char *lines[] = {
                "Tab / Left/Right  - switch panels",
                "Up/Down           - navigate",
                "Enter             - open file (editor)",
                "C / M / F         - transfer: opens dialog (Move/Copy/Cancel) with rename field",
                "P                 - change permissions (chmod dialog)",
                "T                 - toggle theme (light/dark)",
                "F1 or ?           - show this help",
                "Esc or click      - close this window"
            };
            int count = (int)(sizeof(lines)/sizeof(lines[0]));
            for (int i = 0; i < count; ++i) {
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

/* Chmod dialog: toggle rwx for user/group/other */
static void chmod_dialog(const char *filepath, mode_t old_mode) {
    if (!dpy || !filepath) return;
    int dialog_w = 420, dialog_h = 220;
    int sx = (win_w - dialog_w) / 2;
    int sy = (win_h - dialog_h) / 2;
    Window dlg = XCreateSimpleWindow(dpy, win, sx, sy, dialog_w, dialog_h, 2, fg_color, bg_color);
    XSetWindowBackground(dpy, dlg, bg_color);
    XSetWindowBorder(dpy, dlg, fg_color);
    XSelectInput(dpy, dlg, ExposureMask | ButtonPressMask | KeyPressMask);
    XMapWindow(dpy, dlg);
    XClearWindow(dpy, dlg);
    XFlush(dpy);

    /* flags: u r w x, g r w x, o r w x */
    int flags[9];
    flags[0] = (old_mode & S_IRUSR) ? 1 : 0;
    flags[1] = (old_mode & S_IWUSR) ? 1 : 0;
    flags[2] = (old_mode & S_IXUSR) ? 1 : 0;
    flags[3] = (old_mode & S_IRGRP) ? 1 : 0;
    flags[4] = (old_mode & S_IWGRP) ? 1 : 0;
    flags[5] = (old_mode & S_IXGRP) ? 1 : 0;
    flags[6] = (old_mode & S_IROTH) ? 1 : 0;
    flags[7] = (old_mode & S_IWOTH) ? 1 : 0;
    flags[8] = (old_mode & S_IXOTH) ? 1 : 0;

    const char *group_labels[3] = { "User:", "Group:", "Other:" };
    const char *perm_labels[3] = { "r", "w", "x" };

    int box_x = 100;
    int box_y = 40;
    int box_w = 20;
    int box_h = 20;

    int running = 1;
    int apply = 0;

    while (running) {
        XEvent ev;
        XNextEvent(dpy, &ev);
        if (ev.type == Expose && ev.xexpose.window == dlg) {
            XSetForeground(dpy, gc, bg_color);
            XFillRectangle(dpy, dlg, gc, 0, 0, dialog_w, dialog_h);
            XSetForeground(dpy, gc, fg_color);

            XDrawString(dpy, dlg, gc, 10, 20 + fontinfo->ascent, "Change permissions", 18);

            for (int g = 0; g < 3; ++g) {
                int y = box_y + g * 40;
                XDrawString(dpy, dlg, gc, 10, y + fontinfo->ascent, group_labels[g], strlen(group_labels[g]));
                for (int p = 0; p < 3; ++p) {
                    int idx = g*3 + p;
                    int x = box_x + p * 40;
                    XDrawRectangle(dpy, dlg, gc, x, y, box_w, box_h);
                    if (flags[idx]) {
                        XDrawString(dpy, dlg, gc, x+6, y + fontinfo->ascent, perm_labels[p], 1);
                    }
                }
            }

            /* buttons */
            int by = dialog_h - 50;
            XDrawRectangle(dpy, dlg, gc, 60, by, 100, 30);
            XDrawString(dpy, dlg, gc, 90, by + fontinfo->ascent + 6, "Apply", 5);
            XDrawRectangle(dpy, dlg, gc, 220, by, 100, 30);
            XDrawString(dpy, dlg, gc, 250, by + fontinfo->ascent + 6, "Cancel", 6);

            XFlush(dpy);
        } else if (ev.type == ButtonPress && ev.xbutton.window == dlg) {
            int mx = ev.xbutton.x, my = ev.xbutton.y;
            /* toggle boxes */
            for (int g = 0; g < 3; ++g) {
                int y = box_y + g * 40;
                for (int p = 0; p < 3; ++p) {
                    int x = box_x + p * 40;
                    if (mx >= x && mx <= x + box_w && my >= y && my <= y + box_h) {
                        int idx = g*3 + p;
                        flags[idx] = !flags[idx];
                        XClearWindow(dpy, dlg);
                        XFlush(dpy);
                    }
                }
            }
            /* Apply */
            if (mx >= 60 && mx <= 160 && my >= dialog_h - 50 && my <= dialog_h - 20) {
                apply = 1; running = 0;
            }
            /* Cancel */
            if (mx >= 220 && mx <= 320 && my >= dialog_h - 50 && my <= dialog_h - 20) {
                apply = 0; running = 0;
            }
        } else if (ev.type == KeyPress && ev.xkey.window == dlg) {
            KeySym ks = XLookupKeysym(&ev.xkey, 0);
            if (ks == XK_Escape) { apply = 0; running = 0; }
        }
    }

    if (apply) {
        mode_t newmode = 0;
        if (flags[0]) newmode |= S_IRUSR;
        if (flags[1]) newmode |= S_IWUSR;
        if (flags[2]) newmode |= S_IXUSR;
        if (flags[3]) newmode |= S_IRGRP;
        if (flags[4]) newmode |= S_IWGRP;
        if (flags[5]) newmode |= S_IXGRP;
        if (flags[6]) newmode |= S_IROTH;
        if (flags[7]) newmode |= S_IWOTH;
        if (flags[8]) newmode |= S_IXOTH;
        if (chmod(filepath, newmode) != 0) {
            perror("chmod failed");
        }
    }

    XDestroyWindow(dpy, dlg);
    XFlush(dpy);
}

/* Handle mouse clicks in main window */
static void handle_click(int x, int y, int button) {
    int panel_mid = win_w / 2;
    Panel *clicked = (x < panel_mid) ? &left_panel : &right_panel;
    if (clicked != active_panel) {
        active_panel = clicked;
        draw_interface();
        return;
    }

    /* compute index under click */
    int top_y = MARGIN + line_height * 2;
    if (y < top_y) return;
    int idx = (y - top_y) / line_height + active_panel->scroll_offset;
    if (idx >= 0 && idx < active_panel->nentries) {
        active_panel->selected_idx = idx;
        draw_interface();
    }
}

static int entry_cmp(const void *a, const void *b) {
    const Entry *ea = (const Entry*)a;
    const Entry *eb = (const Entry*)b;
    return strcasecmp(ea->name, eb->name);
}

static void toggle_sort_panel(Panel *p) {
    if (!p) return;

    if (p->sorted == 0) {
        qsort(p->entries, p->nentries, sizeof(Entry), entry_cmp);
        p->sorted = 1;
    } else {
        memcpy(p->entries, p->original_entries,
               sizeof(Entry) * p->original_nentries);
        p->nentries = p->original_nentries;
        p->sorted = 0;
    }

    p->selected_idx = (p->nentries > 0) ? 0 : -1;
    p->scroll_offset = 0;
}


/* Handle keyboard events */
static void handle_key(XKeyEvent *key) {
    KeySym ks = XLookupKeysym(key, 0);
    int ctrl = key->state & ControlMask;

    switch (ks) {
        case XK_Tab:
        case XK_Left:
        case XK_Right:
            active_panel = (active_panel == &left_panel) ? &right_panel : &left_panel;
            draw_interface();
            break;
        case XK_Up:
            if (active_panel->selected_idx > 0) active_panel->selected_idx--;
            if (active_panel->selected_idx < active_panel->scroll_offset) active_panel->scroll_offset = active_panel->selected_idx;
            draw_interface();
            break;
        case XK_Down:
            if (active_panel->selected_idx < active_panel->nentries - 1) active_panel->selected_idx++;
            /* ensure visible */
            {
                int visible = (win_h - (MARGIN + line_height * 3)) / line_height;
                if (active_panel->selected_idx >= active_panel->scroll_offset + visible)
                    active_panel->scroll_offset = active_panel->selected_idx - visible + 1;
            }
            draw_interface();
            break;
        case XK_Return:
            /* open file or enter directory */
            if (active_panel->selected_idx >= 0) {
                Entry *e = &active_panel->entries[active_panel->selected_idx];
                if (e->is_dir) {
                    if (strcmp(e->name, "..") == 0) {
                        /* parent */
                        char *p = strrchr(active_panel->cwd, '/');
                        if (!p || p == active_panel->cwd) strcpy(active_panel->cwd, "/");
                        else *p = '\0';
                    } else {
                        char newcwd[MAX_PATH];
                        if (strcmp(active_panel->cwd, "/") == 0) snprintf(newcwd, sizeof(newcwd), "/%s", e->name);
                        else snprintf(newcwd, sizeof(newcwd), "%s/%s", active_panel->cwd, e->name);
                        strncpy(active_panel->cwd, newcwd, sizeof(active_panel->cwd)-1);
                    }
                    read_dir(active_panel, active_panel->cwd);
                    draw_interface();
                } else {
                    /* open with xterm vi */
                    char filepath[MAX_PATH];
                    build_path(filepath, sizeof(filepath), active_panel->cwd, e->name);
                    pid_t pid = fork();
                    if (pid == 0) {
                        execlp("xterm", "xterm", "-e", "vi", filepath, (char*)NULL);
                        _exit(1);
                    } else if (pid > 0) {
                        add_process(pid, "xterm (vi)");
                    }
                }
            }
            break;
        case XK_F1:
        case XK_question:
            show_help_dialog();
            draw_interface();
            break;
		case XK_s:
		case XK_S:
			toggle_sort_panel(active_panel);
			draw_interface();
			break;
        case XK_c: case XK_C:
        case XK_m: case XK_M:
        case XK_f: case XK_F:
            /* initiate transfer */
            initiate_transfer_from_active();
            break;
        case XK_p: case XK_P:
            if (active_panel->selected_idx >= 0) {
                Entry *e = &active_panel->entries[active_panel->selected_idx];
                char filepath[MAX_PATH];
                build_path(filepath, sizeof(filepath), active_panel->cwd, e->name);
                chmod_dialog(filepath, e->mode);
                read_dir(active_panel, active_panel->cwd);
                draw_interface();
            }
            break;
        case XK_t: case XK_T:
            dark_mode = !dark_mode;
            apply_theme();
            draw_interface();
            break;
        case XK_q: case XK_Q:
            if (ctrl) exit(0);
            break;
        case XK_r: case XK_R:
            read_dir(active_panel, active_panel->cwd);
            draw_interface();
            break;
        case XK_Delete:
        case XK_d: case XK_D:
            if (active_panel->selected_idx >= 0) {
                Entry *e = &active_panel->entries[active_panel->selected_idx];
                char filepath[MAX_PATH];
                build_path(filepath, sizeof(filepath), active_panel->cwd, e->name);
                if (e->is_dir) {
                    if (rmdir(filepath) == 0) {
                        read_dir(active_panel, active_panel->cwd);
                        draw_interface();
                    } else perror("rmdir");
                } else {
                    if (unlink(filepath) == 0) {
                        read_dir(active_panel, active_panel->cwd);
                        draw_interface();
                    } else perror("unlink");
                }
            }
            break;
        default:
            break;
    }
}

static void draw_interface(void) {
    int panel_mid = win_w / 2;
    int left_w = panel_mid - PANEL_SEPARATOR_WIDTH / 2;
    int right_x = panel_mid + PANEL_SEPARATOR_WIDTH / 2;
    int right_w = win_w - right_x;
    int i, y;

    /* Очистка окна */
    XClearWindow(dpy, win);

    /* Заголовок окна */
    XStoreName(dpy, win,
        (active_panel == &left_panel)
            ? "Minix FM (C Version) - LEFT panel active | F1: Help"
            : "Minix FM (C Version) - RIGHT panel active | F1: Help"
    );

    /* Отрисовка заголовка панелей */
    XSetForeground(dpy, gc, fg_color);
    XDrawString(dpy, win, gc, MARGIN, MARGIN + fontinfo->ascent,
        left_panel.cwd, strlen(left_panel.cwd));

    XDrawString(dpy, win, gc, right_x + MARGIN, MARGIN + fontinfo->ascent,
        right_panel.cwd, strlen(right_panel.cwd));

    /* Заголовок столбцов */
    XDrawString(dpy, win, gc, MARGIN,
        MARGIN + line_height + fontinfo->ascent,
        "Permissions  Type  Name", 23);

    XDrawString(dpy, win, gc, right_x + MARGIN,
        MARGIN + line_height + fontinfo->ascent,
        "Permissions  Type  Name", 23);

    /* Линия-разделитель */
    XDrawLine(dpy, win, gc, 0,
        MARGIN + line_height * 2,
        win_w, MARGIN + line_height * 2);

    /* Вертикальный разделитель - используем цвет выделения / контраста */
    XSetForeground(dpy, gc, sel_bg_color);
    XFillRectangle(dpy, win, gc,
        panel_mid - PANEL_SEPARATOR_WIDTH / 2, 0,
        PANEL_SEPARATOR_WIDTH, win_h);

    /* Вернуть цвет текста */
    XSetForeground(dpy, gc, fg_color);

    /* Сколько строк помещается */
    int visible = (win_h - (MARGIN + line_height * 3)) / line_height;

    /* ----------- ОТРИСОВКА ЛЕВОЙ ПАНЕЛИ ----------- */
    for (i = 0; i < visible; i++) {
        int idx = i + left_panel.scroll_offset;
        y = MARGIN + (i + 3) * line_height + fontinfo->ascent;

        if (idx < left_panel.nentries) {
            Entry *e = &left_panel.entries[idx];
            char line[512];
            snprintf(line, sizeof(line), "%-10s %-5s %s",
                e->perms, e->type_label, e->name);

            if (&left_panel == active_panel && idx == left_panel.selected_idx) {
                XSetForeground(dpy, gc, sel_bg_color);
                XFillRectangle(dpy, win, gc, 0,
                               y - line_height, left_w, line_height);
                XSetForeground(dpy, gc, fg_color);
            }

            XDrawString(dpy, win, gc, MARGIN, y, line, strlen(line));
        }
    }

    /* ----------- ОТРИСОВКА ПРАВОЙ ПАНЕЛИ ----------- */
    for (i = 0; i < visible; i++) {
        int idx = i + right_panel.scroll_offset;
        y = MARGIN + (i + 3) * line_height + fontinfo->ascent;

        if (idx < right_panel.nentries) {
            Entry *e = &right_panel.entries[idx];
            char line[512];
            snprintf(line, sizeof(line), "%-10s %-5s %s",
                e->perms, e->type_label, e->name);

            if (&right_panel == active_panel && idx == right_panel.selected_idx) {
                XSetForeground(dpy, gc, sel_bg_color);
                XFillRectangle(dpy, win, gc, right_x,
                               y - line_height, right_w, line_height);
                XSetForeground(dpy, gc, fg_color);
            }

            XDrawString(dpy, win, gc, right_x + MARGIN, y, line, strlen(line));
        }
    }

    /* ----------- Смена темы ----------- */
    const char *theme_text = dark_mode ? "[T] Light mode" : "[T] Dark mode";
    XDrawString(dpy, win, gc,
                win_w - (strlen(theme_text) * 8) - 10,
                MARGIN + fontinfo->ascent,
                theme_text, strlen(theme_text));
}

/* Print help to stdout (for CLI usage) */
static void print_cli_help_and_exit(const char *prog) {
    printf("%s - simple dual-panel Xlib file manager (C)\n", prog);
    printf("Usage: %s [options]\n", prog);
    printf("  -h, --help      Print this help and exit (useful if X dialogs are problematic)\n");
    printf("Keys:\n");
    printf("  Tab / Left/Right  - switch panels\n");
    printf("  Up/Down           - navigate\n");
    printf("  Enter             - open file (editor)\n");
    printf("  C / M / F         - transfer (Move/Copy/Cancel) with rename field\n");
    printf("  P                 - change permissions (chmod dialog)\n");
    printf("  T                 - toggle theme (light/dark)\n");
    printf("  F1 or ?           - show graphical help\n");
    printf("  Ctrl+Q            - quit\n");
    exit(0);
}

/* MAIN PROGRAM */
int main(int argc, char **argv) {
    if (argc > 1) {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            print_cli_help_and_exit(argv[0]);
        }
    }

    char start_cwd[MAX_PATH];
    if (!getcwd(start_cwd, sizeof(start_cwd))) strcpy(start_cwd, "/");

    init_panel(&left_panel, start_cwd);
    init_panel(&right_panel, start_cwd);
    active_panel = &left_panel;

    dpy = XOpenDisplay(NULL);
    if (!dpy) {
        fprintf(stderr, "ERROR: cannot open X server\n");
        return 1;
    }

    int screen = DefaultScreen(dpy);
    /* Create window using temporary default colors; we'll apply theme after GC created */
    win = XCreateSimpleWindow(dpy, RootWindow(dpy, screen),
                              0, 0, win_w, win_h, 1,
                              BlackPixel(dpy, screen), WhitePixel(dpy, screen));

    XSizeHints hints;
    hints.flags = PMinSize;
    hints.min_width = 600;
    hints.min_height = 400;
    XSetNormalHints(dpy, win, &hints);

    XSelectInput(dpy, win, ExposureMask | ButtonPressMask | KeyPressMask | StructureNotifyMask);
    XMapWindow(dpy, win);

    /* Load larger font */
    fontinfo = XLoadQueryFont(dpy, "10x20");
    if (!fontinfo) fontinfo = XLoadQueryFont(dpy, "9x15");
    if (!fontinfo) fontinfo = XLoadQueryFont(dpy, "fixed");

    gc = XCreateGC(dpy, win, 0, NULL);
    if (fontinfo) {
        XSetFont(dpy, gc, fontinfo->fid);
        line_height = fontinfo->ascent + fontinfo->descent + 4;
    }

    /* Apply initial theme and set window background/border to avoid white flashes */
    apply_theme();
    XSetWindowBackground(dpy, win, bg_color);
    XSetWindowBorder(dpy, win, fg_color);
    XClearWindow(dpy, win);
    XFlush(dpy);

    draw_interface();

    XEvent ev;
    while (1) {
        XNextEvent(dpy, &ev);
        if (ev.type == Expose)
            draw_interface();
        else if (ev.type == ButtonPress) {
            if (ev.xbutton.button == Button4) scroll_panel(active_panel, -1);
            else if (ev.xbutton.button == Button5) scroll_panel(active_panel, 1);
            else handle_click(ev.xbutton.x, ev.xbutton.y, ev.xbutton.button);
        }
        else if (ev.type == KeyPress)
            handle_key(&ev.xkey);
        else if (ev.type == ConfigureNotify) {
            win_w = ev.xconfigure.width;
            win_h = ev.xconfigure.height;
            draw_interface();
        }
    }

    XCloseDisplay(dpy);
    return 0;
}

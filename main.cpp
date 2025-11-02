#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define WINDOW_W 700
#define WINDOW_H 400
#define LINE_HEIGHT 16
#define MARGIN 5

typedef struct Entry {
    char name[256];
    int is_dir;
    char perms[11];
    mode_t mode;
} Entry;

static Display *dpy;
static Window win;
static GC gc;
static XFontStruct *fontinfo;

static Entry entries[1000];
static int nentries = 0;
static char cwd[1024];
static int selected_idx = -1;   // NEW: index of selected entry

/* Convert st_mode to string like -rwxr-xr-- */
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

/* Read directory contents */
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

    if (strcmp(path, "/") != 0) {
        strcpy(entries[nentries].name, "..");
        entries[nentries].is_dir = 1;
        strcpy(entries[nentries].perms, "drwx------");
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
            nentries++;
        }
    }
    closedir(d);
}

/* Draw the file list with highlight */
static void draw_list(void)
{
    XClearWindow(dpy, win);
    for (int i = 0; i < nentries; i++) {
        int y = MARGIN + i * LINE_HEIGHT + fontinfo->ascent;
        char display[400];
        sprintf(display, "%-12s %s%s",
                entries[i].perms,
                entries[i].is_dir ? "[DIR] " : "",
                entries[i].name);

        if (i == selected_idx) {
            XSetForeground(dpy, gc, 0xC0C0C0);
            XFillRectangle(dpy, win, gc,
                           0, MARGIN + i * LINE_HEIGHT,
                           WINDOW_W, LINE_HEIGHT);
            XSetForeground(dpy, gc, BlackPixel(dpy, 0));
        }

        XDrawString(dpy, win, gc, MARGIN, y, display, strlen(display));
    }
}

/* Check read permission */
static int has_read_access(const char *path)
{
    return access(path, R_OK) == 0;
}

/* Open file or directory */
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

        int pid = fork();
        if (pid == 0) {
            execlp("xterm", "xterm", "-e", "vi", filepath, NULL);
            _exit(1);
        }
    }
}

/* Handle mouse click with selection */
static void handle_click(int y)
{
    int idx = (y - MARGIN) / LINE_HEIGHT;
    if (idx >= 0 && idx < nentries) {
        selected_idx = idx;  
        draw_list();
        open_entry(idx);
    }
}

int main(int argc, char **argv)
{
    if (getcwd(cwd, sizeof(cwd)) == NULL)
        strcpy(cwd, "/");

    dpy = XOpenDisplay(NULL);
    if (!dpy) {
        fprintf(stderr, "Can't open display\n");
        return 1;
    }

    win = XCreateSimpleWindow(dpy, RootWindow(dpy, 0),
                              0, 0, WINDOW_W, WINDOW_H, 1,
                              BlackPixel(dpy, 0), WhitePixel(dpy, 0));
    XSelectInput(dpy, win, ExposureMask | ButtonPressMask);
    XStoreName(dpy, win, "Minix FM v3.0 (highlight + permissions)");
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
        else if (ev.type == ButtonPress)
            handle_click(ev.xbutton.y);
    }

    return 0;
}

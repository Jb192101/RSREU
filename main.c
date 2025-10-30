#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define WIN_W 640
#define WIN_H 480
#define LINE_H 18
#define LEFT_MARGIN 8
#define TOP_MARGIN 8

struct entry {
    char name[256];
    int is_dir;
};

static struct entry entries[1024];
static int entry_count = 0;
static int scroll = 0;
static char cwd[PATH_MAX];

void read_directory(const char *path) {
    DIR *d = opendir(path);
    if (!d) {
        perror("opendir");
        return;
    }
    entry_count = 0;
    struct dirent *de;
    while ((de = readdir(d)) != NULL && entry_count < 1024) {
        if (strcmp(de->d_name, ".") == 0)
            continue;
        struct stat st;
        char full[PATH_MAX];
        snprintf(full, sizeof(full), "%s/%s", path, de->d_name);
        if (stat(full, &st) == 0 && S_ISDIR(st.st_mode))
            entries[entry_count].is_dir = 1;
        else
            entries[entry_count].is_dir = 0;
        strncpy(entries[entry_count].name, de->d_name, sizeof(entries[entry_count].name)-1);
        entry_count++;
    }
    closedir(d);
    scroll = 0;
}

void draw(Display *dpy, Window win, GC gc) {
    XClearWindow(dpy, win);
    int y = TOP_MARGIN + LINE_H;
    int visible = (WIN_H - 2*TOP_MARGIN) / LINE_H;
    for (int i = scroll; i < entry_count && i < scroll + visible; i++) {
        if (entries[i].is_dir) {
            XDrawRectangle(dpy, win, gc, LEFT_MARGIN, y - LINE_H + 4, 10, 10);
        }
        XDrawString(dpy, win, gc, LEFT_MARGIN + 20, y, entries[i].name, strlen(entries[i].name));
        y += LINE_H;
    }
    char status[512];
    snprintf(status, sizeof(status), "CWD: %s (%d items)", cwd, entry_count);
    XDrawString(dpy, win, gc, LEFT_MARGIN, WIN_H - 5, status, strlen(status));
}

int entry_at_y(int y) {
    int rel = y - TOP_MARGIN;
    if (rel < 0)
        return -1;
    int idx = scroll + rel / LINE_H;
    if (idx >= 0 && idx < entry_count)
        return idx;
    return -1;
}

int main() {
    if (!getcwd(cwd, sizeof(cwd))) {
        perror("getcwd");
        return 1;
    }

    Display *dpy = XOpenDisplay(NULL);
    if (!dpy) {
        fprintf(stderr, "Cannot open X display\n");
        return 1;
    }

    int screen = DefaultScreen(dpy);
    Window win = XCreateSimpleWindow(dpy, RootWindow(dpy, screen),
                                     50, 50, WIN_W, WIN_H, 1,
                                     BlackPixel(dpy, screen),
                                     WhitePixel(dpy, screen));
    XSelectInput(dpy, win, ExposureMask | KeyPressMask | ButtonPressMask);
    XMapWindow(dpy, win);

    GC gc = XCreateGC(dpy, win, 0, NULL);
    XSetForeground(dpy, gc, BlackPixel(dpy, screen));

    read_directory(cwd);

    XEvent ev;
    int running = 1;
    while (running) {
        XNextEvent(dpy, &ev);
        if (ev.type == Expose) {
            draw(dpy, win, gc);
        } else if (ev.type == KeyPress) {
            KeySym ks = XLookupKeysym(&ev.xkey, 0);
            if (ks == XK_q || ks == XK_Escape)
                running = 0;
            else if (ks == XK_Up) {
                if (scroll > 0) scroll--;
                draw(dpy, win, gc);
            } else if (ks == XK_Down) {
                int visible = (WIN_H - 2*TOP_MARGIN) / LINE_H;
                if (scroll + visible < entry_count) scroll++;
                draw(dpy, win, gc);
            }
        } else if (ev.type == ButtonPress) {
            if (ev.xbutton.button == Button1) {
                int idx = entry_at_y(ev.xbutton.y);
                if (idx >= 0 && entries[idx].is_dir) {
                    if (strcmp(entries[idx].name, "..") == 0) {
                        char *p = strrchr(cwd, '/');
                        if (p && p != cwd)
                            *p = '\0';
                        else
                            strcpy(cwd, "/");
                    } else {
                        if (strcmp(cwd, "/") == 0)
                            snprintf(cwd, sizeof(cwd), "/%s", entries[idx].name);
                        else
                            snprintf(cwd + strlen(cwd), sizeof(cwd) - strlen(cwd), "/%s", entries[idx].name);
                    }
                    read_directory(cwd);
                    draw(dpy, win, gc);
                }
            } else if (ev.xbutton.button == Button4) { // scroll up
                if (scroll > 0) scroll--;
                draw(dpy, win, gc);
            } else if (ev.xbutton.button == Button5) { // scroll down
                int visible = (WIN_H - 2*TOP_MARGIN) / LINE_H;
                if (scroll + visible < entry_count) scroll++;
                draw(dpy, win, gc);
            }
        }
    }

    XFreeGC(dpy, gc);
    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);
    return 0;
}

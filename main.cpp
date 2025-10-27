#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

const int WIN_W = 640;
const int WIN_H = 480;
const int LINE_H = 18;
const int LEFT_MARGIN = 8;
const int TOP_MARGIN = 8;
const int VISIBLE_LINES = (WIN_H - 2*TOP_MARGIN) / LINE_H;

struct Entry {
    std::string name;
    bool is_dir;
};

std::vector<Entry> entries;
int scroll = 0;
std::string cwd = ".";

void read_directory(const std::string &path) {
    entries.clear();
    DIR *d = opendir(path.c_str());
    if (!d) {
        perror("opendir");
        return;
    }
    struct dirent *de;
    while ((de = readdir(d)) != nullptr) {
        std::string n = de->d_name;
        if (n == ".") continue;
        struct stat st;
        std::string full = path + "/" + n;
        if (stat(full.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
            entries.push_back({n, true});
        } else {
            entries.push_back({n, false});
        }
    }
    closedir(d);
    std::sort(entries.begin(), entries.end(), [](const Entry &a, const Entry &b){
        if (a.is_dir != b.is_dir) return a.is_dir > b.is_dir;
        return strcasecmp(a.name.c_str(), b.name.c_str()) < 0;
    });
    scroll = 0;
}

void draw(Display *dpy, Window win, GC gc) {
    XClearWindow(dpy, win);
    int x = LEFT_MARGIN;
    int y = TOP_MARGIN + LINE_H - 4;
    int last = std::min((int)entries.size(), scroll + VISIBLE_LINES);
    for (int i = scroll; i < last; ++i) {
        const Entry &e = entries[i];
        if (e.is_dir) {
            XDrawRectangle(dpy, win, gc, x-6, y-12, 10, 12);
        }
        XDrawString(dpy, win, gc, x+12, y, e.name.c_str(), e.name.size());
        y += LINE_H;
    }
    std::string status = "CWD: " + cwd + "  (" + std::to_string(entries.size()) + " items)";
    XDrawString(dpy, win, gc, LEFT_MARGIN, WIN_H - 6, status.c_str(), status.size());
}

int entry_at_y(int y) {
    int rel = y - TOP_MARGIN;
    if (rel < 0) return -1;
    int idx = rel / LINE_H + scroll;
    if (idx >= 0 && idx < (int)entries.size()) return idx;
    return -1;
}

int main() {
    // init X
    Display *dpy = XOpenDisplay(nullptr);
    if (!dpy) {
        std::cerr << "Cannot open display\n";
        return 1;
    }
    int scr = DefaultScreen(dpy);
    Window root = RootWindow(dpy, scr);
    Window win = XCreateSimpleWindow(dpy, root, 10, 10, WIN_W, WIN_H, 1,
                                     BlackPixel(dpy, scr), WhitePixel(dpy, scr));
    XSelectInput(dpy, win, ExposureMask | ButtonPressMask | KeyPressMask | StructureNotifyMask);
    XMapWindow(dpy, win);
    GC gc = XCreateGC(dpy, win, 0, nullptr);

    XSetForeground(dpy, gc, BlackPixel(dpy, scr));

    char buf[PATH_MAX];
    if (getcwd(buf, sizeof(buf))) cwd = buf;
    read_directory(cwd);

    XEvent ev;
    bool running = true;
    while (running) {
        XNextEvent(dpy, &ev);
        switch (ev.type) {
            case Expose:
                if (ev.xexpose.count == 0) draw(dpy, win, gc);
                break;
            case ConfigureNotify:
                break;
            case ButtonPress: {
                int b = ev.xbutton.button;
                if (b == Button1) {
                    int idx = entry_at_y(ev.xbutton.y);
                    if (idx >= 0) {
                        Entry &e = entries[idx];
                        if (e.is_dir) {
                            if (e.name == "..") {
                                size_t pos = cwd.find_last_of('/');
                                if (pos == std::string::npos) cwd = ".";
                                else if (pos == 0) cwd = "/";
                                else cwd = cwd.substr(0, pos);
                            } else if (cwd == "/") cwd = "/" + e.name;
                            else cwd = cwd + "/" + e.name;
                            read_directory(cwd);
                            draw(dpy, win, gc);
                        } else {
                            std::cout << cwd << "/" << e.name << std::endl;
                        }
                    }
                } else if (b == Button4) {
                    scroll = std::max(0, scroll - 1);
                    draw(dpy, win, gc);
                } else if (b == Button5) {
                    if ((int)entries.size() > VISIBLE_LINES)
                        scroll = std::min((int)entries.size() - VISIBLE_LINES, scroll + 1);
                    draw(dpy, win, gc);
                }
                break;
            }
            case KeyPress: {
                KeySym ks = XLookupKeysym(&ev.xkey, 0);
                if (ks == XK_q || ks == XK_Escape) running = false;
                else if (ks == XK_Up) {
                    scroll = std::max(0, scroll - 1);
                    draw(dpy, win, gc);
                } else if (ks == XK_Down) {
                    if ((int)entries.size() > VISIBLE_LINES)
                        scroll = std::min((int)entries.size() - VISIBLE_LINES, scroll + 1);
                    draw(dpy, win, gc);
                } else if (ks == XK_Return) {
                    if (!entries.empty()) {
                        Entry &e = entries[scroll];
                        if (e.is_dir) {
                            if (cwd == "/") cwd = "/" + e.name;
                            else cwd = cwd + "/" + e.name;
                            read_directory(cwd);
                            draw(dpy, win, gc);
                        }
                    }
                }
                break;
            }
            default:
                break;
        }
    }

    XFreeGC(dpy, gc);
    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);
    return 0;
}

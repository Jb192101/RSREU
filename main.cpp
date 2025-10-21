#include <X11/Xlib.h>

int main() {
    Display* display = XOpenDisplay(NULL);
    if (!display) return 1;
    
    int screen = DefaultScreen(display);
    Window window = XCreateSimpleWindow(display, RootWindow(display, screen),
                                       100, 100, 400, 300, 2,
                                       BlackPixel(display, screen),
                                       WhitePixel(display, screen));
    
    XStoreName(display, window, "Minix Window");
    XSelectInput(display, window, ExposureMask | KeyPressMask);
    XMapWindow(display, window);
    
    XEvent event;
    while (1) {
        XNextEvent(display, &event);
        if (event.type == KeyPress) break;
    }
    
    XDestroyWindow(display, window);
    XCloseDisplay(display);
    return 0;
}
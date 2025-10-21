#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <iostream>
#include <cstdlib>
#include <string>

int main() {
    Display* display = XOpenDisplay(NULL);
    if (display == NULL) {
        std::cerr << "Cannot open X display" << std::endl;
        return 1;
    }

    int screen = DefaultScreen(display);
    
    Window window = XCreateSimpleWindow(
        display,
        RootWindow(display, screen),
        100, 100,          
        400, 300,         
        2,                  
        BlackPixel(display, screen),  
        WhitePixel(display, screen)   
    );

    XStoreName(display, window, "X11 Window Example");

    XSelectInput(display, window, 
        ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask);

    XMapWindow(display, window);

    GC gc = XCreateGC(display, window, 0, NULL);
    XSetForeground(display, gc, BlackPixel(display, screen));

    bool running = true;
    while (running) {
        XEvent event;
        XNextEvent(display, &event);

        switch (event.type) {
            case Expose:
                if (event.xexpose.count == 0) {
                    std::string hello = "Hello X11 World!";
                    XDrawString(display, window, gc, 50, 50, 
                               hello.c_str(), hello.length());
                    
                    XDrawRectangle(display, window, gc, 100, 100, 200, 100);
                }
                break;

            case KeyPress:
                std::cout << "Key pressed: " 
                         << XLookupKeysym(&event.xkey, 0) << std::endl;
                running = false; 
                break;

            case ButtonPress:
                std::cout << "Mouse button pressed at: " 
                         << event.xbutton.x << ", " << event.xbutton.y << std::endl;
                break;

            case ConfigureNotify:
                std::cout << "Window resized to: " 
                         << event.xconfigure.width << "x" 
                         << event.xconfigure.height << std::endl;
                break;
        }
    }

    // Очистка ресурсов
    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);

    std::cout << "Window closed" << std::endl;
    return 0;
}
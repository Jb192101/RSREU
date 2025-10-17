#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <iostream>
#include <cstdlib>

int main() {
    // Открываем соединение с X-сервером
    Display* display = XOpenDisplay(NULL);
    if (display == NULL) {
        std::cerr << "Cannot open X display" << std::endl;
        return 1;
    }

    // Получаем экран по умолчанию
    int screen = DefaultScreen(display);
    
    // Создаем окно
    Window window = XCreateSimpleWindow(
        display,
        RootWindow(display, screen),
        100, 100,            // x, y позиция
        400, 300,            // ширина, высота
        2,                   // ширина границы
        BlackPixel(display, screen),  // цвет границы
        WhitePixel(display, screen)   // цвет фона
    );

    // Устанавливаем заголовок окна
    XStoreName(display, window, "X11 Window Example");

    // Выбираем события которые будем обрабатывать
    XSelectInput(display, window, 
        ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask);

    // Показываем окно
    XMapWindow(display, window);

    // Создаем графический контекст
    GC gc = XCreateGC(display, window, 0, NULL);
    XSetForeground(display, gc, BlackPixel(display, screen));

    // Главный цикл обработки событий
    bool running = true;
    while (running) {
        XEvent event;
        XNextEvent(display, &event);

        switch (event.type) {
            case Expose:
                // Отрисовка при получении события Expose
                if (event.xexpose.count == 0) {
                    // Рисуем текст
                    std::string hello = "Hello X11 World!";
                    XDrawString(display, window, gc, 50, 50, 
                               hello.c_str(), hello.length());
                    
                    // Рисуем прямоугольник
                    XDrawRectangle(display, window, gc, 100, 100, 200, 100);
                }
                break;

            case KeyPress:
                std::cout << "Key pressed: " 
                         << XLookupKeysym(&event.xkey, 0) << std::endl;
                running = false;  // Закрываем окно при нажатии любой клавиши
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
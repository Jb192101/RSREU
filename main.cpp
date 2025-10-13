#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>

int main(int argc, char **argv) {
    // Создаем главное окно
    Fl_Window *window = new Fl_Window(400, 300, "Простое FLTK приложение");
    
    // Создаем текстовую метку
    Fl_Box *box = new Fl_Box(100, 100, 200, 100, "Привет, FLTK!");
    
    // Настраиваем шрифт и размер текста
    box->labelfont(FL_BOLD);
    box->labelsize(20);
    box->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
    
    // Завершаем создание окна
    window->end();
    
    // Показываем окно
    window->show();
    
    // Запускаем главный цикл обработки событий
    return Fl::run();
}

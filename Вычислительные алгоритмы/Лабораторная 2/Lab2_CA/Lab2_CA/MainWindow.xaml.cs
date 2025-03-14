using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Markup;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace Lab2_CA
{
    public partial class MainWindow : Window
    {
        /* 
         * Коэффициенты для уравнения ae^(bx) - 5cx^(2d)
         * По умолчанию всё равно единице
         */
        public static float a = 1.0f;
        public static float b = 1.0f;
        public static float c = 1.0f;
        public static float d = 1.0f;

        // Эпсилон (постоянная)
        const float epsilon = 0.0001f;

        // Диапазон
        double l1 = 0;
        double l2 = Math.Pow(10, 5);

        // Ф-ии

        // Функция
        public static double f(double x, double a, double b, double c, double d)
        {
            return a*Math.Pow(x, 5*b) + c*x - 0.2*d;
        }

        // Её первая производная
        private double f_strih(double x)
        {
            return 5 * a * b * Math.Pow(x, 5 * b - 1) + c;
        }

        // Её вторая производная
        private double f_strih_strih(double x)
        {
            return 5 * a * b * (5 * b - 1) * Math.Pow(x, 5 * b - 2);
        }
        // Фи
        private double phi(double x)
        {
            return 0.2*d/c - a*Math.Pow(x, 5*b)/c;
        }

        // Ф-ии закончились

        public MainWindow()
        {
            InitializeComponent();
        }

        // Обновление данных
        private void Button_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                // Загрузка данных
                a = Convert.ToSingle(a_tb.Text);
                b = Convert.ToSingle(b_tb.Text);
                c = Convert.ToSingle(c_tb.Text);
                d = Convert.ToSingle(d_tb.Text);
                //Console.WriteLine("Данные загружены успешно!");

                // Использование методов
                //dihotomiya_label.Text = dihotomy_method();
                dihotomiya_label.Text = dihotomy_method();
                proportional_label.Text = hords_method();
                newton_label.Text = newton_method();
                modif_newton_label.Text = modif_newton_method();
                combine_label.Text = combine_method();
                iterations_label.Text = iterations_method(); 
                //Console.WriteLine("Все методы прошлись успешно!");

            }
            catch(Exception ex)
            {
                MessageBox.Show("Введите цифры! Иные символы не поддерживаются", "ОШИБКА");
            }

        }

        // Отображение графика
        private void show_graph_Click(object sender, RoutedEventArgs e)
        {
            GraphWindow graphWindow = new GraphWindow();
            graphWindow.Show();
        }

        // Методы
        // Метод дихотомии (+)
        private String dihotomy_method()
        {
            int countOfIterations = 0;
            double z, x = 0;
            while(Math.Abs(l2 - l1) >= 2*epsilon)
            {
                z = (l1 + l2) / 2;
                if (f(z, a, b, c, d) * f(l1, a, b, c, d) < 0)
                {
                    l2 = z;
                } else
                {
                    l1 = z;
                }

                x = z;
                countOfIterations++;
            }

            pogresh_dihotomy_label.Text = Convert.ToString(Math.Round((l2 - l1)/Math.Pow(2, countOfIterations), 15));

            return Convert.ToString(Math.Round(x, 15));
        }

        // Метод хорд (+)
        private String hords_method()
        {
            double x = 1, x_prev;
            double X;
            if(f(l1, a, b, c, d) * f_strih_strih(l1) > 0)
            {
                X = l1;
                x_prev = l2;
            } else
            {
                X = l2;
                x_prev = l1;
            }

            while(x - x_prev > epsilon)
            {
                x = x_prev - f(x_prev, a, b, c, d) * (X - x_prev) / (f(X, a, b, c, d) - f(x_prev, a, b, c, d));
                x_prev = x;
            }

            pogresh_hords_label.Text = Convert.ToString(Math.Round(l2 - l1, 15));

            return Convert.ToString(x);
        }

        // Метод Ньютона (+)
        private String newton_method()
        {
            double x;
            if (f(l1, a, b, c, d) * f_strih_strih(l1) > 0)
            {
                x = l1;
            } else if (f(l2, a, b, c, d) * f_strih_strih(l2) > 0)
            {
                x = l2;
            } else
            {
                return "Error of calculating!";
            }

            double h;
            while(true)
            {
                h = f(x, a, b, c, d) / f_strih(x);
                x = x - h;

                if (Math.Abs(h) < epsilon)
                {
                    break;
                }
            }

            pogresh_Newton_label.Text = Convert.ToString(Math.Round(h, 15));

            return Convert.ToString(Math.Round(x, 15));
        }

        // Модифицированный метод Ньютона (+)
        private String modif_newton_method()
        {
            double x = 1000;
            double x_prev = 0;
            if(f(l1, a, b, c, d) * f_strih_strih(l1) > 0)
            {
                x = l1;
            } else
            {
                x = l2;
            }

            double c_der = f_strih(x);
            double inacc = 100000;
            while (x - x_prev > epsilon)
            {
                x_prev = x;
                x = Math.Round(x - f(x, a, b, c, d) / c_der, 10);
                inacc = Math.Abs(Math.Round(f(x, a, b, c, d) / Math.Abs(f_strih(x)), 10));
            }

            pogresh_modif_newton_label.Text = Convert.ToString(Math.Round(Math.Abs(x - x_prev), 10));

            return Convert.ToString(Math.Round(x, 15));
        }

        // Комбинированный метод (+)
        private String combine_method()
        {
            double x, y;
            if (f(l1, a, b, c, d) * f_strih_strih(l1) > 0)
            {
                x = l1;
                y = l2;
            } else
            {
                x = l2;
                y = l1;
            }

            double inacc = 1000000.0f;
            while (inacc > epsilon)
            {
                x = Math.Round(x - f(x, a, b, c, d) / f_strih(x), 10);
                y = Math.Round(y - f(y, a, b, c, d) * (x - y) / (f(x, a, b, c, d) - f(y, a, b, c, d)), 10);
                inacc = Math.Abs((x - y));
            }

            pogresh_combine_label.Text = Convert.ToString(Math.Round(inacc, 15));
            return Convert.ToString(Math.Round(x, 15));
        }

        // Метод итераций
        private String iterations_method()
        {
            int maxIterations = 100;
            double last = 0;
            double x = (l1 + l2) / 2;
            double m = Math.Max(f(l1, a, b, c, d), f(l2, a, b, c, d));
            m = Math.Max(m, f(x, a, b, c, d));
            double acc = 0, x_new = 1;

            for(int i = 0; i < maxIterations; i++)
            {
                if(Math.Abs(f(x, a, b, c, d)) < epsilon)
                {
                    acc = Math.Round(Math.Abs(x - last), 15);
                    break;
                }
                x_new = x - f(x, a, b, c, d) / m;
                m = Math.Max(m, f_strih(x_new));
                last = x;
                x = x_new;
            }

            pogresh_iterations_label.Text = Convert.ToString(acc);

            return Convert.ToString(Math.Round(x, 15));
        }

    }
}
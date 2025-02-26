using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
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
        float a = 1.0f;
        float b = 1.0f;
        float c = 1.0f;
        float d = 1.0f;

        // Эпсилон (постоянная)
        const float epsilon = 0.0001f;

        // Диапазон
        double l1 = -1 * Math.Pow(10, 5);
        double l2 = Math.Pow(10, 5);

        // Ф-ии

        // Функция
        private double f(double x)
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

        // Фи 1 (доделать)
        private double phi1(double x)
        {
            return x;
        }

        // Фи 2 (доделать)
        private double phi2(double x)
        {
            return x;
        }

        // Фи 3 (доделать)
        private double phi3(double x)
        {
            return x;
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
                this.a = Convert.ToSingle(a_tb.Text);
                this.b = Convert.ToSingle(b_tb.Text);
                this.c = Convert.ToSingle(c_tb.Text);
                this.d = Convert.ToSingle(d_tb.Text);
                //Console.WriteLine("Данные загружены успешно!");

                // Использование методов
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

        // Метод дихотомии
        private String dihotomy_method()
        {
            double z, x = 0;
            while(Math.Abs(l2 - l1) >= epsilon)
            {
                z = (l1 + l2) / 2;
                if (f(z) * f(l1) < 0)
                {
                    l2 = z;
                } else
                {
                    l1 = z;
                }

                x = (l2 + l1) / 2;
            }

            // Восстановление значений
            //l1 = -1 * Math.Pow(10, 5);
            //l2 = Math.Pow(10, 5);


            return Convert.ToString(Math.Round(x, 10));
        }

        // Метод хорд
        private String hords_method()
        {
            double q, t, x, y;
            q = f(l1);
            t = f(l2);
            x = l1;

            while (true)
            {
                x = l1 - ((l2 - l1) * q / (t - q));
                y = f(x);
                if (y * q < 0)
                {
                    l2 = x;
                    t = y;
                } else
                {
                    l1 = x;
                    q = y;
                }

                // Условие выхода из цикла
                if (f(x) <= epsilon)
                {
                    break;
                }
            }

            // Восстановление значений
            //l1 = -1 * Math.Pow(10, 5);
            //l2 = Math.Pow(10, 5);

            return Convert.ToString(Math.Round(x, 10));
        }

        // Метод Ньютона
        private String newton_method()
        {
            double x;
            if (f(l1) * f_strih_strih(l1) > 0)
            {
                x = l1;
            } else if (f(l2) * f_strih_strih(l2) > 0)
            {
                x = l2;
            } else
            {
                return "Error of calculating!";
            }

            double h;
            while(true)
            {
                h = f(x) / f_strih(x);
                x = x - h;

                if (Math.Abs(h) < epsilon)
                {
                    break;
                }
            }

            return Convert.ToString(Math.Round(x, 10));
        }

        // Модифицированный метод Ньютона
        private String modif_newton_method()
        {
            return Convert.ToString(1);
        }

        // Комбинированный метод
        private String combine_method()
        {
            return Convert.ToString(1);
        }

        // Метод итераций (доделать)
        private String iterations_method()
        {
            double x, x1, d;
            x = (l1 + l2) / 2;

            while (true)
            {
                if (x > 0)
                {
                    x1 = phi1(x);
                } else if (x > -1)
                {
                    x1 = phi2(x);
                } else
                {
                    x1 = phi3(x);
                }

                d = Math.Abs(x1 - x);
                x = x1;

                if (x1 < epsilon)
                {
                    break;
                }
            }

            return Convert.ToString(x);
        }

    }
}
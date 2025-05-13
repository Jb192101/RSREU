using System;
using System.Drawing;
using System.Windows.Forms;

namespace Lab9_CG
{
    public partial class Form1 : Form
    {
        private PictureBox fractalPictureBox;
        private const float MinX = -2.5f;
        private const float MaxX = 5f;
        private const float MinY = 0;
        private const float MaxY = 10f;
        private const int PointNumber = 100000;

        // Вероятности для разных преобразований
        private float[] probability = { 0.912675f, 0.087325f };

        // Коэффициенты для аффинных преобразований
        private float[,] coefficients = {
            // a         b         c         d        e        f
            { 0.745455f, -0.459091f, 0.406061f, 0.887121f, 1.460279f, 0.691072f },  // Первое преобразование
            { -0.424242f, -0.065152f, -0.175758f, -0.218182f, 3.809567f, 6.741476f } // Второе преобразование
        };

        private Bitmap fractalBitmap;
        private Graphics graphics;

        public Form1()
        {
            InitializeComponent();
            this.ClientSize = new Size(800, 600);

            // Настройка PictureBox
            fractalPictureBox = new PictureBox
            {
                Dock = DockStyle.Fill,
                BackColor = Color.Black
            };
            this.Controls.Add(fractalPictureBox);

            // Создание битмапа для рисования
            fractalBitmap = new Bitmap(fractalPictureBox.Width, fractalPictureBox.Height);
            graphics = Graphics.FromImage(fractalBitmap);
            graphics.Clear(Color.Black);

            // Рисование фрактала
            DrawFractal();

            // Отображение результата
            fractalPictureBox.Image = fractalBitmap;
        }

        private void DrawFractal()
        {
            Random random = new Random();
            float x = 0, y = 0;

            for (int i = 0; i < PointNumber; i++)
            {
                // Выбор преобразования на основе вероятности
                double r = random.NextDouble();
                int transformation = (r <= probability[0]) ? 0 : 1;

                // Применение аффинного преобразования
                float newX = coefficients[transformation, 0] * x
                            + coefficients[transformation, 1] * y
                            + coefficients[transformation, 4];

                float newY = coefficients[transformation, 2] * x
                            + coefficients[transformation, 3] * y
                            + coefficients[transformation, 5];

                x = newX;
                y = newY;

                // Преобразование координат для отображения
                int pixelX = (int)(fractalBitmap.Width / 3 + x * fractalBitmap.Width / (MaxX - MinX));
                int pixelY = (int)(fractalBitmap.Height - y * fractalBitmap.Height / (MaxY - MinY));

                // Установка пикселя с проверкой границ
                if (pixelX >= 0 && pixelX < fractalBitmap.Width && pixelY >= 0 && pixelY < fractalBitmap.Height)
                {
                    // Используем градиент от зеленого к желтому для визуального интереса
                    int colorValue = (int)(i % 255);
                    fractalBitmap.SetPixel(pixelX, pixelY, Color.FromArgb(0, 255 - colorValue / 2, colorValue));
                }
            }
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);
            if (fractalBitmap != null)
            {
                e.Graphics.DrawImage(fractalBitmap, Point.Empty);
            }
        }

        private void Form1_Load(object sender, EventArgs e)
        {
        }
    }
}
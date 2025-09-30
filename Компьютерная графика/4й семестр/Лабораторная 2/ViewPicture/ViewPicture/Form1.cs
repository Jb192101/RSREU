using System.Drawing;
using System.Security.Cryptography.X509Certificates;
using System.Windows.Forms;
using System.Windows.Forms.VisualStyles;

namespace ViewPicture
{

    public partial class Form1 : Form
    {

        private Bitmap _bitmap;
        public Form1()
        {
            InitializeComponent();
            InitializeComboBox();
        }

        private void InitializeComboBox()
        {
            comboBox1.SelectedIndex = 1; // Устанавливаем 100% по умолчанию
            comboBox1.SelectedIndexChanged += comboBox1_SelectedIndexChanged;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            using (OpenFileDialog openFileDialog = new OpenFileDialog())
            {
                openFileDialog.Filter = "Bitmap files (*.bmp)|*.bmp";
                if (openFileDialog.ShowDialog() == DialogResult.OK)
                {
                    _bitmap = new Bitmap(openFileDialog.FileName);
                    pictureBox1.Image = _bitmap;
                    pictureBox1.SizeMode = PictureBoxSizeMode.Zoom; // Устанавливаем режим отображения
                    ScaleImage();
                }
            }
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            ScaleImage();
        }
        private void ScaleImage()
        {
            if (_bitmap != null)
            {
                float scale = GetScaleFactor();
                int newWidth = (int)(_bitmap.Width * scale);
                int newHeight = (int)(_bitmap.Height * scale);

                pictureBox1.Width = newWidth;
                pictureBox1.Height = newHeight;

                // Обновляем изображение в PictureBox с новыми размерами
                pictureBox1.Image = new Bitmap(_bitmap, newWidth, newHeight);

                // Меняем размер формы, чтобы она соответствовала PictureBox
                this.ClientSize = new Size(newWidth + 29, newHeight + 94); // Учитывайте размеры заголовка и границ формы
                button1.Location = new Point(pictureBox1.Image.Width - 70, pictureBox1.Image.Height + 60);
                comboBox1.Location = new Point(pictureBox1.Image.Width -150, pictureBox1.Image.Height +60);
            }
        }

        private float GetScaleFactor()
        {
            switch (comboBox1.SelectedItem.ToString())
            {
                case "50%":
                    return 0.5f;
                case "100%":
                    return 1.0f;
                case "150%":
                    return 1.5f;
                case "200%":
                    return 2.0f;
                default:
                    return 1.0f;
            }
        }

    }
}

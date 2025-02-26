using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using OxyPlot;
using OxyPlot.Series;

namespace Lab2_CA
{
    public partial class GraphWindow : Window
    {
        public GraphWindow()
        {
            InitializeComponent();

            MyModelTask1 = new PlotModel { Title = "График" };
            MyModelTask1.Series.Add(new FunctionSeries(Math.Sin, 0, 10, 0.1, "График" ));

            this.DataContext = this;
        }

        public PlotModel MyModelTask1 { get; set; }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.NetworkInformation;
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
using OxyPlot.Axes;
using OxyPlot.Series;

namespace Lab2_CA
{
    public partial class GraphWindow : Window
    {
        private static double x0 = -2;
        private static double xn = 2;
        private static double epsilon = 0.0001;

        public PlotModel MyModel { get; set; }

        public GraphWindow()
        {
            InitializeComponent();
            MyModel = CreatePlotModel();
            DataContext = this; // Устанавливаем DataContext для привязки
        }

        private PlotModel CreatePlotModel()
        {
            var plotModel = new PlotModel { Title = "График функции" };

            var series = new LineSeries
            {
                MarkerType = MarkerType.None
            };

            var seria_point = new ScatterSeries
            {
                MarkerType = MarkerType.Circle,
                MarkerSize = 5,
                MarkerFill = OxyColors.Red,
            };


            for (double x = -10; x <= 10; x += epsilon)
            {
                double y = MainWindow.f(x, MainWindow.a, MainWindow.b, MainWindow.c, MainWindow.d);
                series.Points.Add(new DataPoint(x, y));

                if(Math.Abs(y) < epsilon)
                {
                    seria_point.Points.Add(new ScatterPoint(x, y));
                }
            }

            plotModel.Axes.Add(new LinearAxis
            {
                Position = AxisPosition.Bottom,
                Minimum = x0,
                Maximum = xn,
                PositionAtZeroCrossing = true,
                AxislineStyle = LineStyle.Solid,
                IsPanEnabled = false,
                IsZoomEnabled = false,
            });
            plotModel.Axes.Add(new LinearAxis
            {
                Position = AxisPosition.Left,
                Minimum = x0,
                Maximum = xn,
                PositionAtZeroCrossing = true,
                AxislineStyle = LineStyle.Solid,
                IsPanEnabled = false,
                IsZoomEnabled = false,
            });

            plotModel.Series.Add(series);
            plotModel.Series.Add(seria_point);

            return plotModel;
        }

    }
}

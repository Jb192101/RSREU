package view;

import javafx.scene.Scene;
import javafx.scene.chart.LineChart;
import javafx.scene.chart.NumberAxis;
import javafx.scene.chart.XYChart;
import javafx.scene.layout.VBox;
import javafx.stage.Stage;
import viewmodel.SpecificContainer;
import viewmodel.ViewModel;

import java.util.ArrayList;

public class GraphWindow extends Stage {
    private ViewModel viewModel;
    private SpecificContainer<Double> sC;

    public GraphWindow(ViewModel _vm) {
        this.viewModel = _vm;
        this.sC = viewModel.start();

        this.setTitle("Лабораторная работа 4 - Барышев");

        LineChart<Number, Number> topChart = createFunctionChart();
        LineChart<Number, Number> bottomChart = createDifferenceChart();

        fillCharts(topChart, bottomChart);

        topChart.setPrefHeight(300);
        bottomChart.setPrefHeight(300);

        VBox root = new VBox(topChart, bottomChart);

        this.setScene(new Scene(root, 800, 600));
    }

    private LineChart<Number, Number> createFunctionChart() {
        NumberAxis xAxis = new NumberAxis(viewModel.getModel().getMinX(), viewModel.getModel().getMaxX(), viewModel.getModel().getN());
        NumberAxis yAxis = new NumberAxis(-1, 1, 0.05);
        LineChart<Number, Number> chart = new LineChart<>(xAxis, yAxis);
        chart.setTitle("Графики исходной и интерполирующей функций");
        chart.setCreateSymbols(false);
        return chart;
    }

    private LineChart<Number, Number> createDifferenceChart() {
        NumberAxis xAxis = new NumberAxis(viewModel.getModel().getMinX(), viewModel.getModel().getMaxX(), viewModel.getModel().getN());
        NumberAxis yAxis = new NumberAxis();
        LineChart<Number, Number> chart = new LineChart<>(xAxis, yAxis);
        chart.setTitle("График погрешностей");
        chart.setCreateSymbols(false);
        return chart;
    }

    private void fillCharts(LineChart<Number, Number> topChart,
                            LineChart<Number, Number> bottomChart) {
        XYChart.Series<Number, Number> sourceSeries = new XYChart.Series<>();
        XYChart.Series<Number, Number> interpolSeries = new XYChart.Series<>();
        XYChart.Series<Number, Number> diffSeries = new XYChart.Series<>();

        sourceSeries.setName("Исходная");
        interpolSeries.setName("Интерполирующая");
        diffSeries.setName("Погрешности");

        ArrayList<Double> sourceFun = this.sC.getFirst();
        ArrayList<Double> interpolFun = this.sC.getSecond();

        int n = sC.getFirst().size();
        for (int i = 0; i < n; i++) {
            double x = (double) i / n;
            double sourceValue = sourceFun.get(i);
            double interpolValue = interpolFun.get(i);

            sourceSeries.getData().add(new XYChart.Data<>(x, sourceValue));
            interpolSeries.getData().add(new XYChart.Data<>(x, interpolValue));
            diffSeries.getData().add(new XYChart.Data<>(x, sourceValue - interpolValue));
        }

        topChart.getData().addAll(sourceSeries, interpolSeries);
        bottomChart.getData().add(diffSeries);
    }
}

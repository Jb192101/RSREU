package view;

import javafx.scene.Scene;
import javafx.scene.chart.LineChart;
import javafx.scene.chart.NumberAxis;
import javafx.scene.chart.XYChart;
import javafx.scene.layout.VBox;
import javafx.stage.Stage;
import model.XYContainer;
import viewmodel.ViewModelTaskFirst;

import java.util.ArrayList;
import java.util.Arrays;

public class WindowTaskFirst extends Stage {
    private final float h = 0.1f;
    ViewModelTaskFirst vmtf;

    public WindowTaskFirst(ViewModelTaskFirst _vmtf) {
        this.vmtf = _vmtf;
    }

    public void start() {
        Stage primaryStage = new Stage();

        XYContainer xy1 = vmtf.taskSolving(h);
        XYContainer xy2 = vmtf.taskSolving(2*h);
        XYContainer xy3 = vmtf.taskSolving(h/2);

        // Данные для графиков (отдельные массивы X и Y)
        ArrayList<Double> xData1 = new ArrayList<>(xy1.getXs());
        ArrayList<Double> yData1 = new ArrayList<>(xy1.getYs());

        ArrayList<Double> xData2 = new ArrayList<>(xy2.getXs());
        ArrayList<Double> yData2 = new ArrayList<>(xy2.getYs());

        ArrayList<Double> xData3 = new ArrayList<>(xy3.getXs());
        ArrayList<Double> yData3 = new ArrayList<>(xy3.getYs());

        // Создаем графики
        LineChart<Number, Number> chart1 = createChartFromXY("График 1", "X", "Y", xData1, yData1);
        LineChart<Number, Number> chart2 = createChartFromXY("График 2", "X", "Y", xData2, yData2);
        LineChart<Number, Number> chart3 = createChartFromXY("График 3", "X", "Y", xData3, yData3);

        // Размещаем графики вертикально
        VBox root = new VBox(10, chart1, chart2, chart3);
        root.setStyle("-fx-padding: 10;");

        // Настраиваем и показываем окно
        Scene scene = new Scene(root, 800, 600);
        primaryStage.setTitle("Графики по отдельным массивам X и Y");
        primaryStage.setScene(scene);
        primaryStage.show();
    }

    private LineChart<Number, Number> createChartFromXY(
            String title, String xAxisLabel, String yAxisLabel,
            ArrayList<Double> xValues, ArrayList<Double> yValues) {

        // Проверка на одинаковый размер массивов
        if (xValues.size() != yValues.size()) {
            throw new IllegalArgumentException("Размеры массивов X и Y должны совпадать");
        }

        // Создаем оси
        NumberAxis xAxis = new NumberAxis();
        xAxis.setLabel(xAxisLabel);
        NumberAxis yAxis = new NumberAxis();
        yAxis.setLabel(yAxisLabel);

        // Создаем график
        LineChart<Number, Number> chart = new LineChart<>(xAxis, yAxis);
        chart.setTitle(title);
        chart.setPrefSize(750, 180);
        chart.setLegendVisible(false);

        // Создаем серию данных
        XYChart.Series<Number, Number> series = new XYChart.Series<>();

        // Добавляем точки из массивов
        for (int i = 0; i < xValues.size(); i++) {
            series.getData().add(new XYChart.Data<>(xValues.get(i), yValues.get(i)));
        }

        // Добавляем серию на график
        chart.getData().add(series);

        return chart;
    }
}

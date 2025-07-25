package view;

import javafx.scene.Scene;
import javafx.scene.chart.LineChart;
import javafx.scene.chart.NumberAxis;
import javafx.scene.chart.XYChart;
import javafx.scene.layout.VBox;
import javafx.stage.Stage;
import model.XYContainer;
import viewmodel.IOpenWindow;
import viewmodel.ViewModelTaskSecond;

import java.util.ArrayList;

public class WindowTaskSecond extends Stage implements IOpenWindow {
    private final double h = 0.1;
    private final double y10 = 0.5;
    private final double y20 = 1.5;

    ViewModelTaskSecond vmts;

    public WindowTaskSecond(ViewModelTaskSecond _vmts) {
        this.vmts = _vmts;
    }

    @Override
    public void openWindow() {

    }

    public void start() {
        Stage primaryStage = new Stage();

        XYContainer xy1 = vmts.taskSolving(h, y10, y20);
        XYContainer xy2 = vmts.taskSolving(2*h, y10, y20);
        XYContainer xy3 = vmts.taskSolving(h/2, y10, y20);

        // Данные для графиков (отдельные массивы X и Y)
        ArrayList<Double> xData1 = new ArrayList<>(xy1.getXs());
        ArrayList<Double> yData1 = new ArrayList<>(xy1.getYs());
        ArrayList<Double> y2Data1 = new ArrayList<>(xy1.getYs2());

        ArrayList<Double> xData2 = new ArrayList<>(xy2.getXs());
        ArrayList<Double> yData2 = new ArrayList<>(xy2.getYs());
        ArrayList<Double> y2Data2 = new ArrayList<>(xy2.getYs2());

        ArrayList<Double> xData3 = new ArrayList<>(xy3.getXs());
        ArrayList<Double> yData3 = new ArrayList<>(xy3.getYs());
        ArrayList<Double> y2Data3 = new ArrayList<>(xy3.getYs2());

        // Создаем графики
        LineChart<Number, Number> chart11 = createChartFromXY("График 1 - y1", "X", "Y", xData1, yData1);
        LineChart<Number, Number> chart12 = createChartFromXY("График 1 - y2", "X", "Y", xData1, y2Data1);

        LineChart<Number, Number> chart21 = createChartFromXY("График 2 - y1", "X", "Y", xData2, yData2);
        LineChart<Number, Number> chart22 = createChartFromXY("График 2 - y2", "X", "Y", xData2, y2Data2);

        LineChart<Number, Number> chart31 = createChartFromXY("График 3 - y1", "X", "Y", xData3, yData3);
        LineChart<Number, Number> chart32 = createChartFromXY("График 3 - y2", "X", "Y", xData3, y2Data3);

        // Размещаем графики вертикально
        VBox root = new VBox(10, chart11, chart12, chart21, chart22, chart31, chart32);
        root.setStyle("-fx-padding: 10;");

        // Настраиваем и показываем окно
        Scene scene = new Scene(root, 800, 900);
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

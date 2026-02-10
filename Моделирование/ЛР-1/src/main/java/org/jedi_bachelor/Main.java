package org.jedi_bachelor;

import com.fasterxml.jackson.databind.ObjectMapper;
import javafx.application.Application;
import javafx.scene.Scene;
import javafx.scene.chart.BarChart;
import javafx.scene.chart.CategoryAxis;
import javafx.scene.chart.NumberAxis;
import javafx.scene.chart.XYChart;
import javafx.scene.control.Label;
import javafx.scene.control.ScrollPane;
import javafx.scene.layout.VBox;
import javafx.stage.Stage;
import org.jedi_bachelor.constants.WorkConstants;
import org.jedi_bachelor.constants.WorkPaths;
import org.jedi_bachelor.generator.UniversalGenerator;

import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

public class Main extends Application {
    private final ObjectMapper objectMapper = new ObjectMapper();
    private final UniversalGenerator<Double> generator = new UniversalGenerator<>(3);

    private double mean;
    private double variance;
    private Map<Integer, Integer> histogram;

    public static void main(String[] args) {
        launch(args);
    }

    @Override
    public void start(Stage stage) throws Exception {
        generateData();

        VBox root = new VBox(10);
        root.setStyle("-fx-padding: 20; -fx-background-color: #f5f5f5;");

        Label titleLabel = new Label("Лабораторная работа №1. Гистограмма частот");
        titleLabel.setStyle("-fx-font-size: 18px; -fx-font-weight: bold;");

        Label infoLabel = new Label(String.format(
                "Параметры: N = %d, k = %d интервалов, генератор: универсальный (k=1)",
                WorkConstants.COUNT_OF_UPS, WorkConstants.COUNT_OF_DIVISIONS
        ));

        BarChart<String, Number> histogramChart = createHistogramChart();

        VBox statsBox = new VBox(5);
        statsBox.setStyle("-fx-padding: 10; -fx-background-color: white; -fx-border-color: #ccc; -fx-border-radius: 5;");

        Label statsTitle = new Label("Статистические характеристики:");
        statsTitle.setStyle("-fx-font-weight: bold;");

        Label meanLabel = new Label(String.format("Математическое ожидание: %.6f", mean));
        Label varianceLabel = new Label(String.format("Дисперсия: %.6f", variance));
        Label stdLabel = new Label(String.format("СКО: %.6f", Math.sqrt(variance)));

        Label theoryTitle = new Label("\nТеоретические значения для U[0,1):");
        theoryTitle.setStyle("-fx-font-weight: bold;");

        Label theoryMean = new Label("Теоретическое матожидание: 0.5");
        Label theoryVariance = new Label(String.format("Теоретическая дисперсия: 1/12 ≈ %.6f", 1.0/12));
        Label theoryStd = new Label(String.format("Теоретическое СКО: √(1/12) ≈ %.6f", Math.sqrt(1.0/12)));

        Label diffTitle = new Label("\nОтклонения от теоретических значений:");
        diffTitle.setStyle("-fx-font-weight: bold;");

        Label meanDiff = new Label(String.format("Отклонение матожидания: %.6f", Math.abs(mean - 0.5)));
        Label varDiff = new Label(String.format("Отклонение дисперсии: %.6f", Math.abs(variance - 1.0/12)));

        statsBox.getChildren().addAll(
                statsTitle, meanLabel, varianceLabel, stdLabel,
                theoryTitle, theoryMean, theoryVariance, theoryStd,
                diffTitle, meanDiff, varDiff
        );

        VBox tableBox = createHistogramTable();
        ScrollPane scrollPane = new ScrollPane(tableBox);
        scrollPane.setFitToWidth(true);
        scrollPane.setPrefHeight(200);

        root.getChildren().addAll(titleLabel, infoLabel, histogramChart, statsBox, scrollPane);

        Scene scene = new Scene(root, 900, 850);
        stage.setTitle("Компьютерное моделирование - Лабораторная работа 1");
        stage.setScene(scene);
        stage.show();
    }

    private void generateData() throws IOException {
        // Парсим JSON в объекты
        Integer[] aArray = objectMapper.readValue(new File(WorkPaths.A_ARRAY_PATH), Integer[].class);
        Integer[] bArray = objectMapper.readValue(new File(WorkPaths.B_ARRAY_PATH), Integer[].class);
        Integer[] cArray = objectMapper.readValue(new File(WorkPaths.C_ARRAY_PATH), Integer[].class);
        Integer[] dArray = objectMapper.readValue(new File(WorkPaths.D_ARRAY_PATH), Integer[].class);
        Integer[] yArray = objectMapper.readValue(new File(WorkPaths.Y_ARRAY_PATH), Integer[].class);

        generator.setA(aArray);
        generator.setB(bArray);
        generator.setC(cArray);
        generator.setD(dArray);

        generator.setY(yArray);

        // Получение выборки
        Double[] results = new Double[WorkConstants.COUNT_OF_UPS];

        for(int j = 0; j < WorkConstants.COUNT_OF_UPS; j++) {
            results[j] = generator.generate();
        }

        // Вычисление математического ожидания (среднего)
        double sum = 0.0;
        for(Double value : results) {
            sum += value;
        }
        mean = sum / WorkConstants.COUNT_OF_UPS;
        System.out.println("Математическое ожидание (среднее): " + mean);

        // Вычисление дисперсии
        variance = 0.0;
        for(Double value : results) {
            variance += Math.pow(value - mean, 2);
        }
        variance /= (WorkConstants.COUNT_OF_UPS - 1); // несмещенная оценка
        System.out.println("Дисперсия: " + variance);
        System.out.println("Среднеквадратическое отклонение: " + Math.sqrt(variance));

        // Второй и третий моменты
        double secondMoment = 0.0;
        double thirdMoment = 0.0;
        for(Double value : results) {
            secondMoment += Math.pow(value, 2);
            thirdMoment += Math.pow(value, 3);
        }
        secondMoment /= WorkConstants.COUNT_OF_UPS;
        thirdMoment /= WorkConstants.COUNT_OF_UPS;

        System.out.println("Второй момент: " + secondMoment);
        System.out.println("Третий момент: " + thirdMoment);

        // Построение гистограммы
        int divisions = WorkConstants.COUNT_OF_DIVISIONS;
        double intervalWidth = 1.0 / divisions;

        histogram = new HashMap<>();
        for(int i = 0; i < divisions; i++) {
            histogram.put(i, 0);
        }

        // Подсчет попаданий в интервалы
        for(Double value : results) {
            int intervalIndex = (int)(value / intervalWidth);
            if(intervalIndex >= divisions) {
                intervalIndex = divisions - 1;
            }
            histogram.put(intervalIndex, histogram.get(intervalIndex) + 1);
        }

        // Вывод гистограммы (пока в виде текста)
        System.out.println("\nГистограмма частот:");
        System.out.println("Интервал\tЧастота\t\tОтносительная частота");

        int totalCount = 0;
        for(Map.Entry<Integer, Integer> entry : histogram.entrySet()) {
            double lowerBound = entry.getKey() * intervalWidth;
            double upperBound = (entry.getKey() + 1) * intervalWidth;
            int frequency = entry.getValue();
            double relativeFrequency = (double)frequency / WorkConstants.COUNT_OF_UPS;

            System.out.printf("[%.4f, %.4f)\t%d\t\t%.4f%n",
                    lowerBound, upperBound, frequency, relativeFrequency);
            totalCount += frequency;
        }

        System.out.println("Общее количество значений: " + totalCount);

        // Построение статистической функции распределения
        System.out.println("\nСтатистическая функция распределения:");
        System.out.println("x\t\tF*(x)");

        int cumulativeCount = 0;
        for(Map.Entry<Integer, Integer> entry : histogram.entrySet()) {
            cumulativeCount += entry.getValue();
            double x = (entry.getKey() + 1) * intervalWidth;
            double empiricalCDF = (double) cumulativeCount / WorkConstants.COUNT_OF_UPS;

            System.out.printf("%.4f\t\t%.4f%n", x, empiricalCDF);
        }

        System.out.println("\nАнализ равномерности:");
        double expectedFrequency = (double)WorkConstants.COUNT_OF_UPS / divisions;
        System.out.println("Ожидаемая частота в каждом интервале: " + expectedFrequency);

        double maxDeviation = 0;
        for(int frequency : histogram.values()) {
            double deviation = Math.abs(frequency - expectedFrequency);
            if(deviation > maxDeviation) {
                maxDeviation = deviation;
            }
        }
        System.out.println("Максимальное отклонение от ожидаемой частоты: " + maxDeviation);
        System.out.println("Относительное отклонение: " + (maxDeviation / expectedFrequency * 100) + "%");
    }

    private BarChart<String, Number> createHistogramChart() {
        final CategoryAxis xAxis = new CategoryAxis();
        final NumberAxis yAxis = new NumberAxis();
        xAxis.setLabel("Интервалы значений");
        yAxis.setLabel("Частота");

        // Создание гистограммы
        final BarChart<String, Number> barChart = new BarChart<>(xAxis, yAxis);
        barChart.setTitle("Гистограмма частот равномерного распределения");
        barChart.setLegendVisible(true);
        barChart.setBarGap(0);
        barChart.setCategoryGap(1);
        barChart.setPrefHeight(400);

        // Данные гистограммы
        XYChart.Series<String, Number> series = new XYChart.Series<>();
        series.setName("Эмпирическая частота");

        int divisions = WorkConstants.COUNT_OF_DIVISIONS;
        double intervalWidth = 1.0 / divisions;

        for(int i = 0; i < divisions; i++) {
            double lowerBound = i * intervalWidth;
            double upperBound = (i + 1) * intervalWidth;
            String intervalLabel = String.format("%.3f-%.3f", lowerBound, upperBound);

            series.getData().add(new XYChart.Data<>(intervalLabel, histogram.get(i)));
        }

        barChart.getData().add(series);

        return barChart;
    }

    private VBox createHistogramTable() {
        VBox table = new VBox(5);
        table.setStyle("-fx-padding: 10; -fx-background-color: white; -fx-border-color: #ccc;");

        Label tableTitle = new Label("Таблица частот по интервалам:");
        tableTitle.setStyle("-fx-font-weight: bold;");

        table.getChildren().add(tableTitle);

        // Заголовок таблицы
        VBox header = new VBox();
        header.setStyle("-fx-background-color: #e0e0e0; -fx-padding: 5;");
        Label headerText = new Label(String.format("%-25s %-15s %-20s %-20s",
                "Интервал", "Частота", "Отн. частота", "Отклонение"));
        header.getChildren().add(headerText);
        table.getChildren().add(header);

        int divisions = WorkConstants.COUNT_OF_DIVISIONS;
        double intervalWidth = 1.0 / divisions;
        double expectedFrequency = (double)WorkConstants.COUNT_OF_UPS / divisions;

        // Данные таблицы
        for(int i = 0; i < divisions; i++) {
            double lowerBound = i * intervalWidth;
            double upperBound = (i + 1) * intervalWidth;
            int frequency = histogram.get(i);
            double relativeFreq = (double)frequency / WorkConstants.COUNT_OF_UPS;
            double deviation = Math.abs(frequency - expectedFrequency);

            Label row = new Label(String.format("%-25s %-15d %-20.4f %-20.2f",
                    String.format("[%.4f, %.4f)", lowerBound, upperBound),
                    frequency, relativeFreq, deviation));

            row.setStyle("-fx-padding: 3;");
            if (i % 2 == 0) {
                row.setStyle("-fx-padding: 3; -fx-background-color: #f9f9f9;");
            }

            table.getChildren().add(row);
        }

        Label totalRow = new Label(String.format("%-25s %-15d %-20.4f",
                "Всего", WorkConstants.COUNT_OF_UPS, 1.0));
        totalRow.setStyle("-fx-padding: 3; -fx-font-weight: bold; -fx-background-color: #d0e0ff;");
        table.getChildren().add(totalRow);

        return table;
    }
}
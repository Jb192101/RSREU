package org.jedi_bachelor;

import com.fasterxml.jackson.databind.ObjectMapper;
import org.jedi_bachelor.constants.WorkConstants;
import org.jedi_bachelor.constants.WorkPaths;
import org.jedi_bachelor.generator.UniversalGenerator;

import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

public class Main {
    private static final ObjectMapper objectMapper = new ObjectMapper();

    public static void main(String[] args) throws IOException {
        // Конфигурация генератора
        UniversalGenerator<Double> generator = new UniversalGenerator<>(3);

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
        double mean = sum / WorkConstants.COUNT_OF_UPS;
        System.out.println("Математическое ожидание (среднее): " + mean);

        // Вычисление дисперсии
        double varianceSum = 0.0;
        for(Double value : results) {
            varianceSum += Math.pow(value - mean, 2);
        }
        double variance = varianceSum / (WorkConstants.COUNT_OF_UPS - 1); // несмещенная оценка
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

        Map<Integer, Integer> histogram = new HashMap<>();
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
}
package org.jedi_bachelor.orders.model;

import org.jedi_bachelor.orders.math.FuzzyLogic;
import org.jedi_bachelor.orders.math.MArray;

public class PairedComparionsMatrix {
    private Float[][] mainMatrix;

    private final String nameOfMatrix;

    private final String[] titles;

    private MArray mfc;

    public PairedComparionsMatrix(int n, String[] titles, String nameOfMatrix) {
        this.mainMatrix = new Float[n][n];
        this.titles = titles;
        this.nameOfMatrix = nameOfMatrix;

        this.mfc = new MArray(n);

        // Конфигурация главной матрицы
        // на главной диагонали единицы, в остальных местах нули
        for(int i = 0; i < n; i++) {
            for(int j = 0; j < n; j++) {
                if(i != j)
                    this.mainMatrix[i][j] = 0.0f;
                else
                    this.mainMatrix[i][j] = 1.0f;
            }
        }

        // Конфигурация MFc(x)
        // 1. Составление матрицы значений
        Float[][] values = configureMfc(n);

        // 2. Нахождение средних значений по столбцам и их запись в M(x)
        configureAverageValues(n, values);
    }

    private Float[][] configureMfc(int n) {
        Float[][] values = new Float[n][n];
        for(int i = 0; i < n; i++) {
            for(int j = 0; j < n; j++) {
                values[i][j] = FuzzyLogic.m(i, j, this.mainMatrix);
            }
        }

        return values;
    }

    private void configureAverageValues(int n, Float[][] values) {
        float average;
        for(int d = 0; d < n; d++) {
            average = 0.0f;
            for(int u = 0; u < n; u++) {
                average += values[d][u];
            }

            average /= n;

            this.mfc.setValue(d, average);
        }
    }

    // val - от 1 до 10
    public void setValue(int i, int j, float val) {
        if(i != j && this.mainMatrix[i][j] == 0.0f) {
            this.mainMatrix[i][j] = val;
            this.mainMatrix[j][i] = 1/val;
        }
    }

    @Override
    public String toString() {
        StringBuilder result = new StringBuilder();

        result.append("Матрица: ").append(nameOfMatrix).append("\n\n");

        int maxCellWidth = 8;
        for (int i = 0; i < titles.length; i++) {
            for (int j = 0; j < titles.length; j++) {
                if (mainMatrix[i][j] != null) {
                    String cellValue = String.format("%.3f", mainMatrix[i][j]);
                    maxCellWidth = Math.max(maxCellWidth, cellValue.length());
                }
            }
        }

        result.append(String.format("%-" + (maxCellWidth + 2) + "s", ""));
        for (String title : titles) {
            result.append(String.format("%-" + (maxCellWidth + 2) + "s", title));
        }
        result.append("\n");

        result.append("-".repeat((maxCellWidth + 2) * (titles.length + 1))).append("\n");

        for (int i = 0; i < titles.length; i++) {
            result.append(String.format("%-" + (maxCellWidth + 2) + "s", titles[i]));

            for (int j = 0; j < titles.length; j++) {
                String cellValue = mainMatrix[i][j] != null ?
                        String.format("%.3f", mainMatrix[i][j]) : "null";
                result.append(String.format("%-" + (maxCellWidth + 2) + "s", cellValue));
            }
            result.append("\n");
        }

        result.append("\nMFC значения:\n");
        for (int i = 0; i < titles.length; i++) {
            float mfcValue = mfc.getValue(i);
            result.append(String.format("%s: %.4f\n", titles[i], mfcValue));
        }

        return result.toString();
    }

    public Float[][] getMainMatrix() {
        return mainMatrix;
    }

    public String getNameOfMatrix() {
        return nameOfMatrix;
    }

    public String[] getTitles() {
        return titles;
    }

    public MArray getMfc() {
        return mfc;
    }
}

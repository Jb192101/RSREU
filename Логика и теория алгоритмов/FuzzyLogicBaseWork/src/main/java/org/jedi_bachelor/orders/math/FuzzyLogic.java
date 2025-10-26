package org.jedi_bachelor.orders.math;

public class FuzzyLogic {
    // Ф-ия принадлежности
    public static float m(int i, int j, Float[][] matrix) {
        float sum = 0;
        for(int k = 0; k < matrix.length; k++) {
            sum += matrix[i][k];
        }

        return matrix[i][j] / sum;
    }

    // Вычисление высоты матрицы
    public static float getHeightOfMatrix(Float[][] matrix) {
        float maxValue = -1.0f;

        for(int i = 0; i < matrix.length; i++) {
            for(int j = 0; j < matrix[0].length; j++) {
                maxValue = Math.max(matrix[i][j], maxValue);
            }
        }

        return maxValue;
    }

    // Логическое "И"
    public static MArray logicalAnd(MArray m1, MArray m2) {
        MArray result = new MArray(m1.getSize());

        for(int i = 0; i < m1.getSize(); i++) {
            result.setValue(i, Math.min(m1.getValue(i), m2.getValue(i)));
        }

        return result;
    }

    // Логическое "НЕ"
    public static MArray logicalNot(MArray m1) {
        MArray result = new MArray(m1.getSize());

        for(int i = 0; i < m1.getSize(); i++) {
            result.setValue(i, 1 - m1.getValue(i));
        }

        return result;
    }

    // Логическое "ИЛИ"
    public static MArray logicalOr(MArray m1, MArray m2) {
        MArray result = new MArray(m1.getSize());

        for(int i = 0; i < m1.getSize(); i++) {
            result.setValue(i, Math.max(m1.getValue(i), m2.getValue(i)));
        }

        return result;
    }

    // Логическое сложение по модулю 2
    public static MArray symmetricalDifference(MArray m1, MArray m2) {
        MArray result = new MArray(m1.getSize());

        float val;
        for(int i = 0; i < m1.getSize(); i++) {
            val = Math.max(
                    Math.min(m1.getValue(i), 1 - m2.getValue(i)),
                    Math.min(1 - m1.getValue(i), m2.getValue(i))
            );
            result.setValue(i, val);
        }

        return result;
    }
}

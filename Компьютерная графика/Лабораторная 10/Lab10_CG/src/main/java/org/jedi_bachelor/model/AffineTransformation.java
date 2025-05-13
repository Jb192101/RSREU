package org.jedi_bachelor.model;

public class AffineTransformation {
    public static Line rotateLine(Line line, double angleDegrees, double pivotX, double pivotY) {
        return line;
    }

    private double[][] multiplicationMatrixs(double[][] m1, double[][] m2) {
        int l11 = m1.length;
        int l12 = m1[0].length;
        int l21 = m2.length;
        int l22 = m2[0].length;

        if(l12 != l21)
            return new double[1][1];

        double[][] result = new double[l11][l22];

        for(int i = 0; i < l11; i++) {
            for(int j = 0; j < l22; j++) {
                for(int k = 0; k < l12; k++) {
                    result[i][j] += m1[i][k] * m2[k][j];
                }
            }
        }

        return result;
    }
}

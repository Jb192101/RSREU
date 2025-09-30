package org.jedi_bachelor.model;

public class AffineTransformation {
    public static Line rotateLine(Line line, double angleDegrees, double a, double b) {
        // Шаг 1
        double[][] resultMatrix = {{Math.cos(angleDegrees), Math.sin(angleDegrees), 0},
                {-Math.sin(angleDegrees), Math.cos(angleDegrees), 0},
                {-a*Math.cos(angleDegrees) + b*Math.sin(angleDegrees) + a, -a*Math.sin(angleDegrees) - b*Math.cos(angleDegrees) + b, 1}};

        // Шаг 2
        double[][] result1 = multiplicationMatrixes(new double[][] {{line.getStartX(), line.getStartY(), 1}}, resultMatrix);
        double[][] result2 = multiplicationMatrixes(new double[][] {{line.getEndX(), line.getEndY(), 1}}, resultMatrix);

        return new Line(result1[0][0], result1[0][1], result2[0][0],result2[0][1]);
    }

    private static double[][] multiplicationMatrixes(double[][] m1, double[][] m2) {
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

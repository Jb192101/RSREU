package org.jedi_bachelor.generator;

/*
* Класс генератора универсального датчика
*
* @author Барышев Г.А.
 */

import lombok.Data;
import lombok.RequiredArgsConstructor;

@Data
@RequiredArgsConstructor
public class UniversalGenerator<Double> implements Generator<java.lang.Double> {
    private final Integer kValue;

    // Вспомогательные массивы
    private Integer[] a;
    private Integer[] b;
    private Integer[] c;
    private Integer[] d;
    private Integer[] y;

    @Override
    public java.lang.Double generate() {
        double s = 0.0;
        double[] x = new double[kValue];
        int[] yN = new int[kValue];

        for(int i = 0; i < kValue; i++) {
            yN[i] = Math.abs(a[i] * (y[i] % b[i]) - c[i] * (y[i] / b[i]));
            y[i] = yN[i];
            x[i] = (double) yN[i] / d[i];
            s += x[i];
        }

        return Math.abs(s - Math.floor(s));
    }
}

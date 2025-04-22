package model;

import java.util.ArrayList;

public class ModelTaskThird implements IFunctionTaskThird {
    private double a;
    private double b;

    public ModelTaskThird(double _a, double _b) {
        this.a = _a;
        this.b = _b;
    }

    public ModelTaskThird() {

    }

    @Override
    public double f31(double _x, double _y2) {
        return Math.atan(_x*_x + _y2*_y2);
    }

    @Override
    public double f32(double _x, double _y1) {
        return Math.sin(_x + _y1);
    }

    @Override
    public XYContainer algoRungeKutt(double _h, double _y10, double _y20) {
        ArrayList<Double> xValues = new ArrayList<>();
        xValues.add(this.a);
        ArrayList<Double> y1Values = new ArrayList<>();
        y1Values.add(_y10);
        ArrayList<Double> y2Values = new ArrayList<>();
        y2Values.add(_y20);

        double x = this.a;
        double y1 = _y10;
        double y2 = _y20;

        double k1, m1, k2, m2, k3, m3, k4, m4;
        while(x < this.b) {
            k1 = (_h * f31(x, y2));
            m1 = (_h * f32(x, y1));

            k2 = (_h * f31(x + _h / 2, y2 + m1 / 2));
            m2 = (_h * f32(x + _h / 2, y1 + k1 / 2));

            k3 = (_h * f31(x + _h / 2, y2 + m2 / 2));
            m3 = (_h * f32(x + _h / 2, y1 + k2 / 2));

            k4 = (_h * f31(x + _h, y2 + m3));
            m4 = (_h * f32(x + _h, y1 + k3));

            y1 += (k1 + 2 * k2 + 2 * k3 + k4) / 6;
            y2 += (m1 + 2 * m2 + 2 * m3 + m4) / 6;
            x += _h;

            xValues.add(x);
            y1Values.add(y1);
            y2Values.add(y2);
        }

        return new XYContainer(xValues, y1Values, y2Values);
    }
}

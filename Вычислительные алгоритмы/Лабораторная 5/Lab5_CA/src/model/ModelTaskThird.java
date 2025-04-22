package model;

import java.util.ArrayList;

public class ModelTaskThird implements IFunctionTaskThird {
    private double a1;
    private double a2;

    public ModelTaskThird(double _a, double _b) {
        this.a1 = _a;
        this.a2 = _b;
    }

    @Override
    public double f31(double _y2) {
        return _y2;
    }

    @Override
    public double f32(double _y1, double _y2) {
        return -a1 * _y2 - a2 * _y1;
    }

    @Override
    public XYContainer algoRungeKutt(double _x0, double _xn, double _y10, double _y20, double _h) {
        ArrayList<Double> xs = new ArrayList<>();
        xs.add(_x0);
        ArrayList<Double> ys1 = new ArrayList<>();
        xs.add(_y10);
        ArrayList<Double> ys2 = new ArrayList<>();
        xs.add(_y20);

        int N = (int) ((_xn - _x0) / _h);

        double x, y1, y2;
        double k11, k12, k21, k22, k31, k32, k41, k42;
        double y1New, y2New, xNew;
        for(int i = 0; i < N; i++) {
            x = xs.getLast();
            y1 = ys1.getLast();
            y2 = ys2.getLast();

            k11 = _h * f31(y2);
            k12 = _h * f32(y1, y2);

            k21 = _h * f31(y2 + k12 / 2);
            k22 = _h * f32(y1 + k11 / 2, y2 + k12 / 2);

            k31 = _h * f31(y2 + k22 / 2);
            k32 = _h * f32(y1 + k21 / 2, y2 + k22 / 2);

            k41 = _h * f31(y2 + k32);
            k42 = _h * f32(y1 + k31, y2 + k32);

            y1New = y1 + (k11 + 2 * k21 + 2 * k31 + k41) / 6;
            y2New = y2 + (k12 + 2 * k22 + 2 * k32 + k42) / 6;
            xNew = x + _h;

            xs.add(xNew);
            ys1.add(y1New);
            ys2.add(y2New);
        }


        return new XYContainer(xs, ys1, ys2);
    }
}

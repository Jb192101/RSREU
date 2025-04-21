package model;

import java.util.ArrayList;

public class ModelTaskFirst implements IFunctionTaskFirst {
    private double x0;
    private double xn;
    private double y0;

    public ModelTaskFirst(float _x0, float _xn, float _y0) {
        this.x0 = _x0;
        this.xn = _xn;
        this.y0 = _y0;
    }

    @Override
    public double f(double _x, double _y) {
        return _y*Math.log(_y)/_x;
    }

    @Override
    public double k1(double _x, double _y, double _h) {
        return _h*f(_x, _y);
    }

    @Override
    public double k2(double _x, double _y, double _h) {
        return _h*f(_x + _h/2, (float) (_y + k1(_x, _y, _h) / 2));
    }

    @Override
    public double k3(double _x, double _y, double _h) {
        return _h*f(_x + _h/2, (float) (_y + k2(_x, _y, _h) / 2));
    }

    @Override
    public double k4(double _x, double _y, double _h) {
        return _h*f(_x + _h/2, (float) (_y + k3(_x, _y, _h)));
    }

    @Override
    public XYContainer methodRungeKutt(float _h) {
        float h = _h;

        ArrayList<Double> xs = new ArrayList<>();
        double x = x0;
        while(x < xn + h/3) {
            xs.add(x);
            x += h;
        }
        ArrayList<Double> ys = new ArrayList<>();
        ys.add(y0);

        double y;
        for (int i = 0; i < xs.size()-1; i++) {
            y = ys.get(i) + (k1(xs.get(i), ys.get(i), h) + 2 * k2(xs.get(i), ys.get(i), h) +
                    2 * k3(xs.get(i), ys.get(i), h) + k4(xs.get(i), ys.get(i), h) / 6);

            ys.add(y);
        }

        return new XYContainer(xs, ys);
    }
}

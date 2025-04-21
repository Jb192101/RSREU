package model;

import java.util.ArrayList;

public class Model implements IFunction {
    private float a;
    private float b;
    private float x0;
    private float xn;
    private float h;
    private float y0;

    public Model(float _a, float _b, float _x0, float _xn, float _h, float _y0) {
        this.a = _a;
        this.b = _b;
        this.x0 = _x0;
        this.xn = _xn;
        this.h = _h;
        this.y0 = _y0;
    }

    @Override
    public float f(float _x) {
        return 0;
    }

    @Override
    public double k1(float _x, float _y) {
        return 0;
    }

    @Override
    public double k2(float _x, float _y) {
        return 0;
    }

    @Override
    public double k3(float _x, float _y) {
        return 0;
    }

    @Override
    public double k4(float _x, float _y) {
        return 0;
    }

    @Override
    public XYContainer methodRungeKutt() {
        ArrayList<Float> xs = new ArrayList<>();
        float x = x0;
        while(x < xn + h/3) {
            xs.add(x);
            x += h;
        }
        ArrayList<Float> ys = new ArrayList<>();
        ys.add(y0);

        float y;
        for (int i = 0; i < xs.size()-1; i++) {
            y = (float) (ys.get(i) + (k1(xs.get(i), ys.get(i)) + 2 * k2(xs.get(i), ys.get(i)) +
                    2 * k3(xs.get(i), ys.get(i)) + k4(xs.get(i), ys.get(i)) / 6));

            ys.add(y);
        }

        return new XYContainer(xs, ys);
    }
}

package model;

import java.util.ArrayList;

public class Model implements IFunction {
    private float a;
    private float b;
    private float x0;
    private float xn;
    private float y0;

    public Model(float _a, float _b, float _x0, float _xn, float _y0) {
        this.a = _a;
        this.b = _b;
        this.x0 = _x0;
        this.xn = _xn;
        this.y0 = _y0;
    }

    @Override
    public float f(float _x, float _y) {
        return (float) (_y*Math.log(_y)/_x);
    }

    @Override
    public double k1(float _x, float _y, float _h) {
        return _h*f(_x, _y);
    }

    @Override
    public double k2(float _x, float _y, float _h) {
        return _h*f(_x + _h/2, (float) (_y + k1(_x, _y, _h) / 2));
    }

    @Override
    public double k3(float _x, float _y, float _h) {
        return _h*f(_x + _h/2, (float) (_y + k2(_x, _y, _h) / 2));;
    }

    @Override
    public double k4(float _x, float _y, float _h) {
        return _h*f(_x + _h/2, (float) (_y + k3(_x, _y, _h)));
    }

    @Override
    public XYContainer methodRungeKutt(float _h) {
        float h = _h;

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
            y = (float) (ys.get(i) + (k1(xs.get(i), ys.get(i), h) + 2 * k2(xs.get(i), ys.get(i), h) +
                    2 * k3(xs.get(i), ys.get(i), h) + k4(xs.get(i), ys.get(i), h) / 6));

            ys.add(y);
        }

        return new XYContainer(xs, ys);
    }
}

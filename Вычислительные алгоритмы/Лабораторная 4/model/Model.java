package model;

import java.util.ArrayList;
import java.util.List;

public class Model implements Function, Interpolation {
    private int minX;
    private int maxX;
    private int n;

    public Model(int _m1, int _m2, int _n) {
        if(_m1 <= _m2 && _n > 0) {
            this.minX = _m1;
            this.maxX = _m2;
            this.n = _n;
        }
    }

    public int getMinX() {
        return this.minX;
    }

    public int getMaxX() {
        return this.maxX;
    }

    public int getN() {
        return this.n;
    }

    @Override
    public double f(double x) {
        return Math.sin(Math.PI*Math.cos(Math.PI*x));
    }

    @Override
    public List<Double> makePointsX() {
        ArrayList<Double> res = new ArrayList<>();
        double h = (double) (this.maxX - this.minX)/this.n;
        double x = this.minX;
        while(x < this.maxX + h / 3) {
            res.add(x);
            x += h;
        }

        return res;
    }

    @Override
    public List<Double> makePointsY(List<Double> _xs) {
        return _xs.stream().map(e -> this.f(e)).toList();
    }

    @Override
    public double dividedDifferences(List<Double> _xs, List<Double> _ys) {
        int n = _xs.size();
        double res = 0;
        double tmp;
        double prod;
        for(int k = 0; k < n; k++) {
            tmp = _ys.get(k);
            prod = 1;
            for(int j = 0; j < n; j++) {
                if(j != k) {
                    prod *= _xs.get(k) - _xs.get(j);
                }
            }
            tmp /= prod;
            res += tmp;

        }
        return res;
    }

    @Override
    public double backwardsInterpolation(double _current, List<Double> _xs, List<Double> _ys) {
        int n = _xs.size();
        double res = 0;

        List<Double> x, y;
        for(int i = n-1; i >= 0; i--) {
            x = _xs.subList(i, n-1);
            y = _ys.subList(i, n-1);
            double temp = this.dividedDifferences(x, y);
            for(int j = i + 1; j < n; j++) {
                temp *= _current - _xs.get(j);
            }
            res += temp;
        }

        return res;
    }
}

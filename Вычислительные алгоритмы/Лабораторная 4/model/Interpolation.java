package model;

import java.util.List;

public interface Interpolation {
    List<Double> makePointsX();
    List<Double> makePointsY(List<Double> _xs);
    double dividedDifferences(List<Double> _xs, List<Double> _ys);
    double backwardsInterpolation(double _current, List<Double> _xs, List<Double> _ys);
}

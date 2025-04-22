package model;

public interface IFunctionTaskThird {
    double f31(double _y2);
    double f32(double _y1, double _y2);

    XYContainer algoRungeKutt(double _x0, double _xn, double _y10, double _y20, double _h);
}

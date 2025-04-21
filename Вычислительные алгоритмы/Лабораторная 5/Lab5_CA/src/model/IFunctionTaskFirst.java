package model;

public interface IFunctionTaskFirst {
    double f(double _x, double _y);

    // Для Рунге-Кутта
    double k1(double _x, double _y, double _h);
    double k2(double _x, double _y, double _h);
    double k3(double _x, double _y, double _h);
    double k4(double _x, double _y, double _h);

    XYContainer methodRungeKutt(float _h);
}

package model;

public interface IFunction {
    float f(float _x);

    // Для Рунге-Кутта
    double k1(float _x, float _y);
    double k2(float _x, float _y);
    double k3(float _x, float _y);
    double k4(float _x, float _y);

    XYContainer methodRungeKutt();
}

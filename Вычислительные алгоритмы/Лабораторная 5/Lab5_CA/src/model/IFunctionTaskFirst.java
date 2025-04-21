package model;

public interface IFunctionTaskFirst {
    float f(float _x, float _y);

    // Для Рунге-Кутта
    double k1(float _x, float _y, float _h);
    double k2(float _x, float _y, float _h);
    double k3(float _x, float _y, float _h);
    double k4(float _x, float _y, float _h);

    XYContainer methodRungeKutt(float _h);

    // Для второго задания
    double f2_1(float _x, float _y2);
    double f2_2(float _x, float _y1);
}

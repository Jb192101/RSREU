package model;

import java.util.ArrayList;

public class XYContainer {
    private ArrayList<Double> xs;
    private ArrayList<Double> ys;
    private ArrayList<Double> ys2;

    public XYContainer(ArrayList<Double> _x, ArrayList<Double> _y) {
        this.xs = _x;
        this.ys = _y;
    }

    public XYContainer(ArrayList<Double> _x, ArrayList<Double> _y, ArrayList<Double> _y2) {
        this.xs = _x;
        this.ys = _y;
        this.ys2 = _y2;
    }

    public ArrayList<Double> getXs() {
        return this.xs;
    }

    public ArrayList<Double> getYs() {
        return this.ys;
    }

    public ArrayList<Double> getYs2() {
        return this.ys2;
    }
}

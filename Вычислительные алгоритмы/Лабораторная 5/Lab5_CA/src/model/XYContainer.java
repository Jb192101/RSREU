package model;

import java.util.ArrayList;

public class XYContainer {
    private ArrayList<Float> xs;
    private ArrayList<Float> ys;
    private ArrayList<Float> ys2;

    public XYContainer(ArrayList<Float> _x, ArrayList<Float> _y) {
        this.xs = _x;
        this.ys = _y;
    }

    public XYContainer(ArrayList<Float> _x, ArrayList<Float> _y, ArrayList<Float> _y2) {
        this.xs = _x;
        this.ys = _y;
        this.ys2 = _y2;
    }

    public ArrayList<Float> getXs() {
        return this.xs;
    }

    public ArrayList<Float> getYs() {
        return this.ys;
    }

    public ArrayList<Float> getYs2() {
        return this.ys2;
    }
}

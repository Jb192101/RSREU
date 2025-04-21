package model;

import java.util.ArrayList;

public class XYContainer {
    private ArrayList<Float> xs;
    private ArrayList<Float> ys;

    public XYContainer(ArrayList<Float> _x, ArrayList<Float> _y) {
        this.xs = _x;
        this.ys = _y;
    }

    public ArrayList<Float> getXs() {
        return this.xs;
    }

    public ArrayList<Float> getYs() {
        return this.ys;
    }
}

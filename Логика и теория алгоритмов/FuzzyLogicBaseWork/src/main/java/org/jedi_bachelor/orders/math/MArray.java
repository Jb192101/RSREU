package org.jedi_bachelor.orders.math;

public class MArray {
    private Float[] array;

    public MArray(int n) {
        this.array = new Float[n];
    }

    public void setValue(int i, float value) {
        this.array[i] = value;
    }

    public Integer getSize() {
        return this.array.length;
    }

    public Float getValue(int i) {
        return this.array[i];
    }

    @Override
    public String toString() {
        String result = "Значения принадлежности: {";

        for(int i = 0; i < this.array.length; i++) {
            result += "(" + this.array[i] + ", x" + (i+1) + ")";
        }
        result += "}";

        return result;
    }
}

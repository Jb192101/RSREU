package viewmodel;

import java.util.ArrayList;

public class SpecificContainer<T> {
    private ArrayList<T> ys1;
    private ArrayList<T> ys2;

    public SpecificContainer(ArrayList<T> _list1, ArrayList<T> _list2) {
        if(_list1.size() == _list2.size()) {
            this.ys1 = _list1;
            this.ys2 = _list2;
        }
    }

    public ArrayList<T> getFirst() {
        return this.ys1;
    }

    public ArrayList<T> getSecond() {
        return this.ys2;
    }
}

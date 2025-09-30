import javafx.geometry.Point2D;

import java.util.ArrayList;

public class Model {
    public Model() {

    }

    public ArrayList<Point2D> drawLine(int _x1, int _y1, int _x2, int _y2) {
        ArrayList<Point2D> arrayList = new ArrayList<>();

        int d = _x2 - _x1;
        int dx = 1;
        int dy = Math.abs((_y2 - _y1) / (_x2 - _x1));

        int x = _x1, y = _y1;

        int i = 0;
        do {
            arrayList.add(new Point2D(x, y));
            x += dx;
            y += dy;
            i++;
        } while(i < d);

        return arrayList;
    }
}

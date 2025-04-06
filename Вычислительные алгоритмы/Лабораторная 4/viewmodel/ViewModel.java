package viewmodel;

import model.Model;

import java.util.ArrayList;
import java.util.List;

public class ViewModel {
    private Model model;

    public ViewModel(Model _m) {
        this.model = _m;
    }

    public SpecificContainer<Double> start() {
        List<Double> args = model.makePointsX();
        List<Double> ys = model.makePointsY(args);

        List<Double> arg = new ArrayList<>();

        double x0 = args.getFirst() - 0.05;
        double hx = 0.0005;
        double xn = args.getLast() + 0.05;

        double x = x0;
        while(x < xn + hx / 3) {
            arg.add(x);
            x += hx;
        }

        ArrayList<Double> y1 = (ArrayList<Double>) arg.stream().map(e -> model.f(e)).toList();
        ArrayList<Double> y2 = (ArrayList<Double>) arg.stream().map(e -> model.backwardsInterpolation(e, args, ys)).toList();

        return new SpecificContainer<>(y1, y2);
    }
}

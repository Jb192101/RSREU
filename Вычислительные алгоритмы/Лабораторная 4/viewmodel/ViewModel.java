package viewmodel;

import model.Model;

import java.util.ArrayList;
import java.util.List;

public class ViewModel {
    private Model model;

    public ViewModel(Model _m) {
        this.model = _m;
    }

    public Model getModel() {
        return model;
    }

    public SpecificContainer<Double> start() {
        List<Double> args = model.makePointsX();
        List<Double> ys = model.makePointsY(args);

        List<Double> arg = new ArrayList<>();

        double x0 = args.getFirst();
        double hx = 0.005;
        double xn = args.getLast();

        double x = x0;
        while(x < xn + hx / 3) {
            arg.add(x);
            x += hx;
        }

        List<Double> y1 = arg.stream().map(e -> model.f(e)).toList();
        List<Double> y2 = arg.stream().map(e -> model.backwardsInterpolation(e, args, ys)).toList();

        return new SpecificContainer<>(new ArrayList<>(y1), new ArrayList<>(y2));
    }
}

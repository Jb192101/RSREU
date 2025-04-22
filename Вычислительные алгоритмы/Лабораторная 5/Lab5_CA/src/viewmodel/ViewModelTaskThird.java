package viewmodel;

import model.ModelTaskThird;
import model.XYContainer;
import view.WindowTaskThird;

public class ViewModelTaskThird implements IOpenWindow {
    private ModelTaskThird mtt;

    public ViewModelTaskThird(ModelTaskThird _mtt) {
        this.mtt = _mtt;
    }

    @Override
    public void openWindow() {
        WindowTaskThird windowTaskThird = new WindowTaskThird(this);
        windowTaskThird.start();
    }

    public XYContainer taskSolving(double _h, double _y10, double _y20) {
        return mtt.algoRungeKutt(-1*Math.PI/2, 0, _y10, _y20, _h);
    }
}

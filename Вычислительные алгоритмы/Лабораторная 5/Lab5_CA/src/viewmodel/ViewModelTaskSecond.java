package viewmodel;

import model.ModelTaskSecond;
import model.XYContainer;
import view.WindowTaskSecond;

public class ViewModelTaskSecond implements IOpenWindow {
    private ModelTaskSecond mts;

    public ViewModelTaskSecond(ModelTaskSecond _mts) {
        this.mts = _mts;
    }

    @Override
    public void openWindow() {
        WindowTaskSecond windowTaskSecond = new WindowTaskSecond(this);
        windowTaskSecond.start();
    }

    public XYContainer taskSolving(double _h, double _y10, double _y20) {
        return mts.algoRungeKutt(_h, _y10, _y20);
    }
}

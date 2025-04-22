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
    }

    public XYContainer taskSolving(double _h, double _y10, double _y20) {
        return mtt.algoRungeKutt(_h, _y10, _y20);
    }
}

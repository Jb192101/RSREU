package viewmodel;

import model.ModelTaskFirst;
import model.XYContainer;
import view.WindowTaskFirst;

public class ViewModelTaskFirst implements IOpenWindow {
    private ModelTaskFirst mtf;

    public ViewModelTaskFirst(ModelTaskFirst _mtf) {
        this.mtf = _mtf;
    }

    @Override
    public void openWindow() {
        WindowTaskFirst windowTaskFirst = new WindowTaskFirst(this);
        windowTaskFirst.start();
    }

    public XYContainer taskSolving(float _h) {
        return mtf.methodRungeKutt(_h);
    }
}

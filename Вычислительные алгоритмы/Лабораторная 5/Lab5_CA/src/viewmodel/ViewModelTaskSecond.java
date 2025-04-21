package viewmodel;

import model.ModelTaskSecond;
import view.WindowTaskSecond;

public class ViewModelTaskSecond implements IOpenWindow {
    private ModelTaskSecond mts;

    public ViewModelTaskSecond(ModelTaskSecond _mts) {
        this.mts = _mts;
    }

    @Override
    public void openWindow() {
        WindowTaskSecond windowTaskFirst = new WindowTaskSecond(this);
    }
}

package viewmodel;

import model.ModelTaskThird;
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
}

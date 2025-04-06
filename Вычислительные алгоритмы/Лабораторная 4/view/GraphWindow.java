package view;

import javafx.stage.Stage;
import viewmodel.ViewModel;

public class GraphWindow extends Stage {
    private ViewModel viewModel;

    public GraphWindow(ViewModel _vm) {
        this.viewModel = _vm;
    }
}

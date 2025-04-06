package view;

import javafx.stage.Stage;
import viewmodel.SpecificContainer;
import viewmodel.ViewModel;

public class GraphWindow extends Stage {
    private ViewModel viewModel;

    public GraphWindow(ViewModel _vm) {
        this.viewModel = _vm;
        SpecificContainer sC1 = viewModel.start();
    }
}

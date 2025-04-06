import javafx.application.Application;
import javafx.stage.Stage;

import model.Model;
import view.GraphWindow;
import viewmodel.ViewModel;

public class Main extends Application {
    public static void main(String[] args) {
        launch();
    }

    @Override
    public void start(Stage stage) {
        int xStart = 0;
        int xEnd = 1;
        int n = 5;
        GraphWindow graphWindow = new GraphWindow(new ViewModel(new Model(xStart, xEnd, n)));
        graphWindow.show();
    }
}
import javafx.application.Application;
import javafx.stage.Stage;

import model.Model;
import view.GraphWindow;
import viewmodel.ViewModel;

public class Main extends Application {
    @Override
    public void start(Stage primaryStage) {
        final int minX = 0;
        final int maxX = 1;
        final int n = 5;
        GraphWindow graphWindow = new GraphWindow(new ViewModel(new Model(minX, maxX, n)));
        graphWindow.show();
    }

    public static void main(String[] args) {
        launch(args);
    }
}
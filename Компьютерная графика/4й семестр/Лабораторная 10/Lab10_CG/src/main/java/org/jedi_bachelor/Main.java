package org.jedi_bachelor;

import javafx.application.Application;
import javafx.scene.Scene;
import javafx.stage.Stage;
import org.jedi_bachelor.view.MainView;
import org.jedi_bachelor.viewmodel.LineViewModel;

public class Main extends Application {
    @Override
    public void start(Stage stage) {
        LineViewModel viewModel = new LineViewModel();
        MainView mainView = new MainView(viewModel);

        Scene scene = new Scene(mainView.getView(), 800, 600);
        scene.getStylesheets().add(getClass().getResource("/styles.css").toExternalForm());

        stage.setTitle("Аффинный поворот");
        stage.setScene(scene);
        stage.show();
    }

    public static void main(String[] args) {
        launch();
    }
}
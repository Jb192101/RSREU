package view;

import javafx.scene.Scene;
import javafx.scene.layout.BorderPane;
import javafx.stage.Stage;
import model.ImageModel;
import viewmodel.ImageEditorViewModel;

public class MainView {
    private final Stage primaryStage;
    private final ImageModel model;
    private final ImageEditorViewModel viewModel;

    public MainView(Stage primaryStage) {
        this.primaryStage = primaryStage;
        this.model = new ImageModel();
        this.viewModel = new ImageEditorViewModel(model);

        setupStage();
    }

    private void setupStage() {
        primaryStage.setTitle("Лабораторная работа 7 - КГ - Барышев");

        ImageEditorView editorView = new ImageEditorView(viewModel);
        BorderPane root = new BorderPane(editorView.getView());

        Scene scene = new Scene(root, 1000, 700);
        scene.getStylesheets().add(getClass().getResource("/styles.css").toExternalForm());

        primaryStage.setScene(scene);
    }

    public void show() {
        primaryStage.show();
    }
}
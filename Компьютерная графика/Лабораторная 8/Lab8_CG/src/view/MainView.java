package view;

import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.image.ImageView;
import javafx.scene.layout.*;
import javafx.stage.Stage;

public class MainView {
    private Stage stage;
    private ImageView originalImageView;
    private ImageView modifiedImageView;
    private Slider hueSlider;
    private Slider saturationSlider;
    private Slider lightnessSlider;
    private Button openButton;
    private Button saveButton;

    public MainView(Stage stage) {
        this.stage = stage;
        initializeUI();
    }

    private void initializeUI() {
        BorderPane root = new BorderPane();
        root.setPadding(new Insets(10));

        HBox imageBox = new HBox(10);
        originalImageView = new ImageView();
        originalImageView.setFitWidth(400);
        originalImageView.setFitHeight(400);
        originalImageView.setPreserveRatio(true);

        modifiedImageView = new ImageView();
        modifiedImageView.setFitWidth(400);
        modifiedImageView.setFitHeight(400);
        modifiedImageView.setPreserveRatio(true);

        imageBox.getChildren().addAll(originalImageView, modifiedImageView);
        imageBox.setAlignment(Pos.CENTER);
        root.setCenter(imageBox);

        VBox controlsBox = new VBox(10);
        controlsBox.setPadding(new Insets(10));

        VBox hueBox = new VBox(5);
        hueBox.getChildren().add(new Label("Hue Adjustment (-180 to 180)"));
        hueSlider = new Slider(-180, 180, 0);
        hueSlider.setShowTickLabels(true);
        hueSlider.setShowTickMarks(true);
        hueSlider.setMajorTickUnit(90);
        hueSlider.setBlockIncrement(10);
        hueBox.getChildren().add(hueSlider);

        VBox saturationBox = new VBox(5);
        saturationBox.getChildren().add(new Label("Saturation Adjustment (-1 to 1)"));
        saturationSlider = new Slider(-1, 1, 0);
        saturationSlider.setShowTickLabels(true);
        saturationSlider.setShowTickMarks(true);
        saturationSlider.setMajorTickUnit(0.5);
        saturationSlider.setBlockIncrement(0.1);
        saturationBox.getChildren().add(saturationSlider);

        VBox lightnessBox = new VBox(5);
        lightnessBox.getChildren().add(new Label("Lightness Adjustment (-1 to 1)"));
        lightnessSlider = new Slider(-1, 1, 0);
        lightnessSlider.setShowTickLabels(true);
        lightnessSlider.setShowTickMarks(true);
        lightnessSlider.setMajorTickUnit(0.5);
        lightnessSlider.setBlockIncrement(0.1);
        lightnessBox.getChildren().add(lightnessSlider);

        HBox buttonBox = new HBox(10);
        openButton = new Button("Открыть изображение");
        saveButton = new Button("Сохранить изображение");
        buttonBox.getChildren().addAll(openButton, saveButton);
        buttonBox.setAlignment(Pos.CENTER);

        controlsBox.getChildren().addAll(hueBox, saturationBox, lightnessBox, buttonBox);
        root.setBottom(controlsBox);

        Scene scene = new Scene(root, 850, 700);
        stage.setTitle("Лабораторная работа 8 - Барышев");
        stage.setScene(scene);
    }

    public ImageView getOriginalImageView() {
        return originalImageView;
    }

    public ImageView getModifiedImageView() {
        return modifiedImageView;
    }

    public Slider getHueSlider() {
        return hueSlider;
    }

    public Slider getSaturationSlider() {
        return saturationSlider;
    }

    public Slider getLightnessSlider() {
        return lightnessSlider;
    }

    public Button getOpenButton() {
        return openButton;
    }

    public Button getSaveButton() {
        return saveButton;
    }

    public void show() {
        stage.show();
    }
}
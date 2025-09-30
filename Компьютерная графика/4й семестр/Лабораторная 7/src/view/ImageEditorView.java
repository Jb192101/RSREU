package view;

import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.control.*;
import javafx.scene.image.ImageView;
import javafx.scene.layout.*;
import javafx.stage.FileChooser;
import viewmodel.ImageEditorViewModel;

import java.io.File;

public class ImageEditorView {
    private final ImageEditorViewModel viewModel;
    private final BorderPane view;
    private final FileChooser fileChooser = new FileChooser();

    public ImageEditorView(ImageEditorViewModel viewModel) {
        this.viewModel = viewModel;
        this.view = new BorderPane();

        setupFileChooser();
        setupUI();
    }

    private void setupFileChooser() {
        fileChooser.getExtensionFilters().addAll(
                new FileChooser.ExtensionFilter("Image Files", "*.jpg", "*.jpeg", "*.png")
        );
    }

    private void setupUI() {
        // Top menu
        HBox menuBar = createMenuBar();

        // Image view
        ImageView imageView = new ImageView();
        imageView.setPreserveRatio(true);
        imageView.fitWidthProperty().bind(view.widthProperty().multiply(0.7));
        imageView.fitHeightProperty().bind(view.heightProperty().multiply(0.8));
        imageView.imageProperty().bind(viewModel.processedImageProperty());

        // Controls
        VBox controls = createControls();

        // Layout
        ScrollPane scrollPane = new ScrollPane(imageView);
        scrollPane.setFitToWidth(true);
        scrollPane.setFitToHeight(true);

        HBox mainContent = new HBox(20, scrollPane, controls);
        mainContent.setAlignment(Pos.CENTER);

        view.setTop(menuBar);
        view.setCenter(mainContent);
    }

    private HBox createMenuBar() {
        Button openButton = new Button("Open");
        openButton.setOnAction(e -> {
            File file = fileChooser.showOpenDialog(null);
            if (file != null) {
                viewModel.loadImage(file);
            }
        });

        Button saveButton = new Button("Save");
        saveButton.disableProperty().bind(viewModel.imageLoadedProperty().not());
        saveButton.setOnAction(e -> {
            File file = fileChooser.showSaveDialog(null);
            if (file != null) {
                viewModel.saveImage(file);
            }
        });

        Button resetButton = new Button("Reset");
        resetButton.disableProperty().bind(viewModel.imageLoadedProperty().not());
        resetButton.setOnAction(e -> viewModel.resetFilters());

        HBox menuBar = new HBox(10, openButton, saveButton, resetButton);
        menuBar.setPadding(new Insets(10));
        menuBar.setAlignment(Pos.CENTER_LEFT);

        return menuBar;
    }

    private VBox createControls() {
        // Brightness control
        Label brightnessLabel = new Label("Яркость:");
        Slider brightnessSlider = new Slider(-1, 1, 0);
        brightnessSlider.setMajorTickUnit(0.5);
        brightnessSlider.setShowTickLabels(true);
        brightnessSlider.valueProperty().bindBidirectional(viewModel.brightnessProperty());

        // Contrast control
        Label contrastLabel = new Label("Контраст:");
        Slider contrastSlider = new Slider(-100, 100, 0);
        contrastSlider.setMajorTickUnit(50);
        contrastSlider.setShowTickLabels(true);
        contrastSlider.valueProperty().bindBidirectional(viewModel.contrastProperty());

        // Saturation control
        Label saturationLabel = new Label("Насыщенность:");
        Slider saturationSlider = new Slider(0, 2, 1);
        saturationSlider.setMajorTickUnit(0.5);
        saturationSlider.setShowTickLabels(true);
        saturationSlider.valueProperty().bindBidirectional(viewModel.saturationProperty());

        // Sharpness control
        Label sharpnessLabel = new Label("Чёткость:");
        Slider sharpnessSlider = new Slider(0, 1, 0);
        sharpnessSlider.setMajorTickUnit(0.25);
        sharpnessSlider.setShowTickLabels(true);
        sharpnessSlider.valueProperty().bindBidirectional(viewModel.sharpnessProperty());

        VBox controls = new VBox(10,
                brightnessLabel, brightnessSlider,
                contrastLabel, contrastSlider,
                saturationLabel, saturationSlider,
                sharpnessLabel, sharpnessSlider);

        controls.setPadding(new Insets(20));
        controls.setAlignment(Pos.CENTER_LEFT);
        controls.setMinWidth(250);

        return controls;
    }

    public BorderPane getView() {
        return view;
    }
}

package controller;

import javafx.embed.swing.SwingFXUtils;
import javafx.scene.image.Image;
import javafx.stage.FileChooser;
import javafx.stage.Window;
import model.ImageModel;
import view.MainView;

import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;

public class MainController {
    private ImageModel model;
    private MainView view;

    public MainController(ImageModel model, MainView view) {
        this.model = model;
        this.view = view;

        setupEventHandlers();
    }

    private void setupEventHandlers() {
        view.getOpenButton().setOnAction(event -> {
            FileChooser fileChooser = new FileChooser();
            fileChooser.setTitle("Открыть картиночку");
            fileChooser.getExtensionFilters().addAll(
                    new FileChooser.ExtensionFilter("Image Files", "*.png", "*.jpg", "*.jpeg", "*.bmp", "*.gif")
            );

            Window window = view.getOriginalImageView().getScene().getWindow();
            File file = fileChooser.showOpenDialog(window);

            if (file != null) {
                Image image = new Image(file.toURI().toString());
                model.setOriginalImage(image);
                view.getOriginalImageView().setImage(image);
                view.getModifiedImageView().setImage(image);

                view.getHueSlider().setValue(0);
                view.getSaturationSlider().setValue(0);
                view.getLightnessSlider().setValue(0);
            }
        });

        view.getSaveButton().setOnAction(event -> {
            if (model.getModifiedImage() == null) return;

            FileChooser fileChooser = new FileChooser();
            fileChooser.setTitle("Save Image");
            fileChooser.getExtensionFilters().addAll(
                    new FileChooser.ExtensionFilter("PNG", "*.png"),
                    new FileChooser.ExtensionFilter("JPEG", "*.jpg", "*.jpeg"),
                    new FileChooser.ExtensionFilter("BMP", "*.bmp")
            );

            Window window = view.getModifiedImageView().getScene().getWindow();
            File file = fileChooser.showSaveDialog(window);

            if (file != null) {
                try {
                    BufferedImage bufferedImage = SwingFXUtils.fromFXImage(model.getModifiedImage(), null);
                    String extension = fileChooser.getSelectedExtensionFilter().getExtensions().get(0).substring(2);
                    ImageIO.write(bufferedImage, extension, file);
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        });

        // HSL sliders
        view.getHueSlider().valueProperty().addListener((obs, oldVal, newVal) -> {
            model.setHueAdjustment(newVal.doubleValue());
            model.applyHSLAdjustments();
            view.getModifiedImageView().setImage(model.getModifiedImage());
        });

        view.getSaturationSlider().valueProperty().addListener((obs, oldVal, newVal) -> {
            model.setSaturationAdjustment(newVal.doubleValue());
            model.applyHSLAdjustments();
            view.getModifiedImageView().setImage(model.getModifiedImage());
        });

        view.getLightnessSlider().valueProperty().addListener((obs, oldVal, newVal) -> {
            model.setLightnessAdjustment(newVal.doubleValue());
            model.applyHSLAdjustments();
            view.getModifiedImageView().setImage(model.getModifiedImage());
        });
    }
}
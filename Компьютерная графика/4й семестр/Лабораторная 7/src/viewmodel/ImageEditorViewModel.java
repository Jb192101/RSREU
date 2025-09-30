package viewmodel;

import javafx.beans.property.*;
import javafx.scene.image.Image;
import model.ImageModel;
import model.ImageProcessor;
import utils.FileUtils;

import java.io.File;

public class ImageEditorViewModel {
    private final ImageModel model;
    private final DoubleProperty brightness = new SimpleDoubleProperty(0);
    private final DoubleProperty contrast = new SimpleDoubleProperty(0);
    private final DoubleProperty saturation = new SimpleDoubleProperty(1);
    private final DoubleProperty sharpness = new SimpleDoubleProperty(0);
    private final BooleanProperty imageLoaded = new SimpleBooleanProperty(false);

    public ImageEditorViewModel(ImageModel model) {
        this.model = model;

        brightness.addListener((obs, oldVal, newVal) -> applyFilters());
        contrast.addListener((obs, oldVal, newVal) -> applyFilters());
        saturation.addListener((obs, oldVal, newVal) -> applyFilters());
        sharpness.addListener((obs, oldVal, newVal) -> applyFilters());
    }

    public void loadImage(File file) {
        Image image = FileUtils.loadImage(file);
        if (image != null) {
            model.setOriginalImage(image);
            imageLoaded.set(true);
            resetFilters();
        }
    }

    public void saveImage(File file) {
        if (model.getProcessedImage() != null) {
            FileUtils.saveImage(model.getProcessedImage(), file);
        }
    }

    public void resetFilters() {
        brightness.set(0);
        contrast.set(0);
        saturation.set(1);
        sharpness.set(0);
        model.resetToOriginal();
    }

    private void applyFilters() {
        if (model.getOriginalImage() == null) return;

        Image processed = model.getOriginalImage();

        if (brightness.get() != 0) {
            processed = ImageProcessor.adjustBrightness(processed, brightness.get());
        }

        if (contrast.get() != 0) {
            processed = ImageProcessor.adjustContrast(processed, contrast.get());
        }

        if (saturation.get() != 1) {
            processed = ImageProcessor.adjustSaturation(processed, saturation.get());
        }

        if (sharpness.get() != 0) {
            processed = ImageProcessor.adjustSharpness(processed, sharpness.get());
        }

        model.setProcessedImage(processed);
    }

    // Property getters
    public DoubleProperty brightnessProperty() { return brightness; }
    public DoubleProperty contrastProperty() { return contrast; }
    public DoubleProperty saturationProperty() { return saturation; }
    public DoubleProperty sharpnessProperty() { return sharpness; }
    public BooleanProperty imageLoadedProperty() { return imageLoaded; }
    public ObjectProperty<Image> processedImageProperty() { return model.processedImageProperty(); }
}

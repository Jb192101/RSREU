package model;

import javafx.beans.property.ObjectProperty;
import javafx.beans.property.SimpleObjectProperty;
import javafx.scene.image.Image;

public class ImageModel {
    private final ObjectProperty<Image> originalImage = new SimpleObjectProperty<>();
    private final ObjectProperty<Image> processedImage = new SimpleObjectProperty<>();

    public Image getOriginalImage() {
        return originalImage.get();
    }

    public ObjectProperty<Image> originalImageProperty() {
        return originalImage;
    }

    public void setOriginalImage(Image originalImage) {
        this.originalImage.set(originalImage);
        this.processedImage.set(originalImage);
    }

    public Image getProcessedImage() {
        return processedImage.get();
    }

    public ObjectProperty<Image> processedImageProperty() {
        return processedImage;
    }

    public void setProcessedImage(Image processedImage) {
        this.processedImage.set(processedImage);
    }

    public void resetToOriginal() {
        this.processedImage.set(this.originalImage.get());
    }
}
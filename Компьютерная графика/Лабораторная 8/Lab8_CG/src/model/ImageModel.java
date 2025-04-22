package model;

import javafx.scene.image.Image;
import javafx.scene.image.PixelReader;
import javafx.scene.image.PixelWriter;
import javafx.scene.image.WritableImage;
import javafx.scene.paint.Color;

public class ImageModel {
    private Image originalImage;
    private Image modifiedImage;

    // HSL values for adjustment
    private double hueAdjustment = 0;
    private double saturationAdjustment = 0;
    private double lightnessAdjustment = 0;

    public Image getOriginalImage() {
        return originalImage;
    }

    public void setOriginalImage(Image originalImage) {
        this.originalImage = originalImage;
        this.modifiedImage = originalImage;
    }

    public Image getModifiedImage() {
        return modifiedImage;
    }

    public void setModifiedImage(Image modifiedImage) {
        this.modifiedImage = modifiedImage;
    }

    public double getHueAdjustment() {
        return hueAdjustment;
    }

    public void setHueAdjustment(double hueAdjustment) {
        this.hueAdjustment = hueAdjustment;
    }

    public double getSaturationAdjustment() {
        return saturationAdjustment;
    }

    public void setSaturationAdjustment(double saturationAdjustment) {
        this.saturationAdjustment = saturationAdjustment;
    }

    public double getLightnessAdjustment() {
        return lightnessAdjustment;
    }

    public void setLightnessAdjustment(double lightnessAdjustment) {
        this.lightnessAdjustment = lightnessAdjustment;
    }

    // Convert RGB to HSL for a single pixel
    // INSERT RGB TO HSL CONVERSION CODE HERE

    // Convert HSL to RGB for a single pixel
    // INSERT HSL TO RGB CONVERSION CODE HERE

    // Apply HSL adjustments to the entire image
    public void applyHSLAdjustments() {
        if (originalImage == null) return;

        int width = (int) originalImage.getWidth();
        int height = (int) originalImage.getHeight();

        WritableImage result = new WritableImage(width, height);
        PixelWriter writer = result.getPixelWriter();
        PixelReader reader = originalImage.getPixelReader();

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                Color originalColor = reader.getColor(x, y);

                // Convert RGB to HSL
                // INSERT RGB TO HSL CONVERSION CODE HERE
                // double[] hsl = rgbToHsl(originalColor);

                // Apply adjustments
                // hsl[0] = (hsl[0] + hueAdjustment) % 360;
                // hsl[1] = Math.max(0, Math.min(1, hsl[1] + saturationAdjustment));
                // hsl[2] = Math.max(0, Math.min(1, hsl[2] + lightnessAdjustment));

                // Convert back to RGB
                // Color adjustedColor = hslToRgb(hsl);

                // writer.setColor(x, y, adjustedColor);
            }
        }

        modifiedImage = result;
    }
}

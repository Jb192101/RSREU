package model;

import javafx.scene.image.Image;
import javafx.scene.image.PixelReader;
import javafx.scene.image.PixelWriter;
import javafx.scene.image.WritableImage;
import javafx.scene.paint.Color;

public class ImageModel {
    private Image originalImage;
    private Image modifiedImage;

    private double hueAdjustment = 0;
    private double saturationAdjustment = 0;
    private double lightnessAdjustment = 0;

    public void setOriginalImage(Image originalImage) {
        this.originalImage = originalImage;
        this.modifiedImage = originalImage;
    }

    public Image getModifiedImage() {
        return modifiedImage;
    }

    public void setHueAdjustment(double hueAdjustment) {
        this.hueAdjustment = hueAdjustment;
    }

    public void setSaturationAdjustment(double saturationAdjustment) {
        this.saturationAdjustment = saturationAdjustment;
    }

    public void setLightnessAdjustment(double lightnessAdjustment) {
        this.lightnessAdjustment = lightnessAdjustment;
    }

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

                double[] hsl = rgbToHsl(originalColor);

                hsl[0] = (hsl[0] + hueAdjustment) % 360;
                if (hsl[0] < 0) hsl[0] += 360;

                hsl[1] = Math.max(0, Math.min(1, hsl[1] + saturationAdjustment));
                hsl[2] = Math.max(0, Math.min(1, hsl[2] + lightnessAdjustment));

                Color adjustedColor = hslToRgb(hsl);

                writer.setColor(x, y, adjustedColor);
            }
        }

        modifiedImage = result;
    }

    public static Color hslToRgb(double[] hsl) {
        double h = hsl[0];
        double s = hsl[1];
        double l = hsl[2];

        if (s == 0) {
            return new Color(l, l, l, 1.0);
        }

        double c = (1 - Math.abs(2 * l - 1)) * s;
        double x = c * (1 - Math.abs((h / 60) % 2 - 1));
        double m = l - c / 2;

        double r, g, b;

        if (h >= 0 && h < 60) {
            r = c; g = x; b = 0;
        } else if (h >= 60 && h < 120) {
            r = x; g = c; b = 0;
        } else if (h >= 120 && h < 180) {
            r = 0; g = c; b = x;
        } else if (h >= 180 && h < 240) {
            r = 0; g = x; b = c;
        } else if (h >= 240 && h < 300) {
            r = x; g = 0; b = c;
        } else {
            r = c; g = 0; b = x;
        }

        r += m;
        g += m;
        b += m;

        r = Math.max(0, Math.min(1, r));
        g = Math.max(0, Math.min(1, g));
        b = Math.max(0, Math.min(1, b));

        return new Color(r, g, b, 1.0);
    }

    public static double[] rgbToHsl(Color color) {
        double r = color.getRed();
        double g = color.getGreen();
        double b = color.getBlue();

        double max = Math.max(r, Math.max(g, b));
        double min = Math.min(r, Math.min(g, b));
        double delta = max - min;

        double l = (max + min) / 2;

        if (delta == 0) {
            return new double[]{0, 0, l};
        }

        double s = delta / (1 - Math.abs(2 * l - 1));

        double h;
        if (max == r) {
            h = 60 * (((g - b) / delta) % 6);
        } else if (max == g) {
            h = 60 * (((b - r) / delta) + 2);
        } else { // max == b
            h = 60 * (((r - g) / delta) + 4);
        }

        if (h < 0) {
            h += 360;
        }

        return new double[]{h, s, l};
    }
}

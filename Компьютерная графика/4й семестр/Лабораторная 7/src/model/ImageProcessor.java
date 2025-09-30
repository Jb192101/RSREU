package model;

import javafx.scene.image.Image;
import javafx.scene.image.PixelReader;
import javafx.scene.image.PixelWriter;
import javafx.scene.image.WritableImage;
import javafx.scene.paint.Color;

public class ImageProcessor {
    public static Image adjustBrightness(Image image, double brightness) {
        int width = (int) image.getWidth();
        int height = (int) image.getHeight();
        WritableImage result = new WritableImage(width, height);
        PixelReader reader = image.getPixelReader();
        PixelWriter writer = result.getPixelWriter();

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                Color color = reader.getColor(x, y);
                double r = clamp(color.getRed() + brightness);
                double g = clamp(color.getGreen() + brightness);
                double b = clamp(color.getBlue() + brightness);
                writer.setColor(x, y, new Color(r, g, b, color.getOpacity()));
            }
        }
        return result;
    }

    public static Image adjustContrast(Image image, double contrast) {
        int width = (int) image.getWidth();
        int height = (int) image.getHeight();
        WritableImage result = new WritableImage(width, height);
        PixelReader reader = image.getPixelReader();
        PixelWriter writer = result.getPixelWriter();

        double factor = (259 * (contrast + 255)) / (255 * (259 - contrast));

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                Color color = reader.getColor(x, y);
                double r = clamp(factor * (color.getRed() * 255 - 128) + 128) / 255;
                double g = clamp(factor * (color.getGreen() * 255 - 128) + 128) / 255;
                double b = clamp(factor * (color.getBlue() * 255 - 128) + 128) / 255;
                writer.setColor(x, y, new Color(r, g, b, color.getOpacity()));
            }
        }
        return result;
    }

    public static Image adjustSaturation(Image image, double saturation) {
        int width = (int) image.getWidth();
        int height = (int) image.getHeight();
        WritableImage result = new WritableImage(width, height);
        PixelReader reader = image.getPixelReader();
        PixelWriter writer = result.getPixelWriter();

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                Color color = reader.getColor(x, y);
                double gray = 0.21 * color.getRed() + 0.72 * color.getGreen() + 0.07 * color.getBlue();
                double r = clamp(gray + (color.getRed() - gray) * saturation);
                double g = clamp(gray + (color.getGreen() - gray) * saturation);
                double b = clamp(gray + (color.getBlue() - gray) * saturation);
                writer.setColor(x, y, new Color(r, g, b, color.getOpacity()));
            }
        }
        return result;
    }

    public static Image adjustSharpness(Image image, double sharpness) {
        int width = (int) image.getWidth();
        int height = (int) image.getHeight();
        WritableImage result = new WritableImage(width, height);
        PixelReader reader = image.getPixelReader();
        PixelWriter writer = result.getPixelWriter();

        // Простое ядро повышения резкости
        double[][] kernel = {
                {0, -1, 0},
                {-1, 5, -1},
                {0, -1, 0}
        };

        for (int y = 1; y < height - 1; y++) {
            for (int x = 1; x < width - 1; x++) {
                double r = 0, g = 0, b = 0;

                for (int ky = -1; ky <= 1; ky++) {
                    for (int kx = -1; kx <= 1; kx++) {
                        Color neighbor = reader.getColor(x + kx, y + ky);
                        double weight = kernel[ky + 1][kx + 1] * sharpness;
                        r += neighbor.getRed() * weight;
                        g += neighbor.getGreen() * weight;
                        b += neighbor.getBlue() * weight;
                    }
                }

                Color original = reader.getColor(x, y);
                r = clamp((1 - sharpness) * original.getRed() + r);
                g = clamp((1 - sharpness) * original.getGreen() + g);
                b = clamp((1 - sharpness) * original.getBlue() + b);
                writer.setColor(x, y, new Color(r, g, b, original.getOpacity()));
            }
        }
        return result;
    }

    private static double clamp(double value) {
        return Math.min(1, Math.max(0, value));
    }
}

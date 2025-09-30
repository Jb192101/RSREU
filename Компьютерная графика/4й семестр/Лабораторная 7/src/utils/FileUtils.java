package utils;

import javafx.embed.swing.SwingFXUtils;
import javafx.scene.image.Image;
import javafx.stage.FileChooser;

import javax.imageio.ImageIO;
import java.io.File;
import java.io.IOException;

public class FileUtils {
    public static Image loadImage(File file) {
        try {
            return new Image(file.toURI().toString());
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
    }

    public static void saveImage(Image image, File file) {
        try {
            String extension = getFileExtension(file);
            ImageIO.write(SwingFXUtils.fromFXImage(image, null), extension, file);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private static String getFileExtension(File file) {
        String name = file.getName();
        int lastIndexOf = name.lastIndexOf(".");
        if (lastIndexOf == -1) {
            return "png"; // default extension
        }
        return name.substring(lastIndexOf + 1);
    }
}
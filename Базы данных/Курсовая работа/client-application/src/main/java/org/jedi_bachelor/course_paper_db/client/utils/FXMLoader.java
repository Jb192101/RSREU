package org.jedi_bachelor.course_paper_db.client.ui.utils;

import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import org.jedi_bachelor.course_paper_db.client.ui.MainApplication;

import java.io.IOException;

public class FXMLoader {
    public static Parent load(String fxmlPath) throws IOException {
        FXMLLoader loader = new FXMLLoader(FXMLoader.class.getResource(fxmlPath));
        loader.setControllerFactory(MainApplication.getSpringContext()::getBean);
        return loader.load();
    }

    public static <T> T loadAndGetController(String fxmlPath) throws IOException {
        FXMLLoader loader = new FXMLLoader(FXMLoader.class.getResource(fxmlPath));
        loader.setControllerFactory(MainApplication.getSpringContext()::getBean);
        loader.load();
        return loader.getController();
    }
}
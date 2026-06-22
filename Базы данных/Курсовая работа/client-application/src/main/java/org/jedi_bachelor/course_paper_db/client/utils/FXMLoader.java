package org.jedi_bachelor.course_paper_db.client.utils;

import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import org.jedi_bachelor.course_paper_db.client.ui.MainApplication;
import org.springframework.context.ConfigurableApplicationContext;

import java.io.IOException;

public class FXMLoader {

    public static Parent load(String fxmlPath) throws IOException {
        ConfigurableApplicationContext context = MainApplication.getSpringContext();
        FXMLLoader loader = new FXMLLoader(FXMLoader.class.getResource(fxmlPath));
        loader.setControllerFactory(context::getBean);
        return loader.load();
    }

    public static <T> T loadAndGetController(String fxmlPath) throws IOException {
        ConfigurableApplicationContext context = MainApplication.getSpringContext();
        FXMLLoader loader = new FXMLLoader(FXMLoader.class.getResource(fxmlPath));
        loader.setControllerFactory(context::getBean);
        loader.load();
        return loader.getController();
    }
}
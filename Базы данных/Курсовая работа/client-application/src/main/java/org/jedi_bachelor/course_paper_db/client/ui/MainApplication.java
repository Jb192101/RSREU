package org.jedi_bachelor.course_paper_db.client.ui;

import javafx.application.Application;
import javafx.application.Platform;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.image.Image;
import javafx.fxml.FXMLLoader;
import javafx.stage.Stage;
import lombok.Getter;
import org.jedi_bachelor.course_paper_db.client.ClientMain;
import org.springframework.boot.SpringApplication;
import org.springframework.context.ConfigurableApplicationContext;

import java.io.IOException;

public class MainApplication extends Application {

    @Getter
    private static ConfigurableApplicationContext springContext;
    @Getter
    private static Stage primaryStage;

    @Override
    public void init() {
        springContext = SpringApplication.run(ClientMain.class);
    }

    @Override
    public void start(Stage stage) throws IOException {
        primaryStage = stage;

        // Настраиваем FXMLLoader с поддержкой Spring
        FXMLLoader loader = new FXMLLoader(getClass().getResource("/fxml/main-view.fxml"));
        loader.setControllerFactory(springContext::getBean);

        Parent root = loader.load();

        Scene scene = new Scene(root, 1200, 750);
        scene.getStylesheets().add(getClass().getResource("/css/main-style.css").toExternalForm());

        stage.setTitle("Книжный магазин - Информационная система");
        stage.setMinWidth(1000);
        stage.setMinHeight(650);

        // Устанавливаем иконку приложения
        try {
            stage.getIcons().add(new Image(getClass().getResourceAsStream("/images/book-icon.png")));
        } catch (Exception e) {
            System.err.println("Иконка не найдена: " + e.getMessage());
        }

        stage.setScene(scene);
        stage.show();

        // Обработка закрытия приложения
        stage.setOnCloseRequest(event -> {
            Platform.exit();
            if (springContext != null) {
                springContext.close();
            }
        });
    }

    @Override
    public void stop() {
        if (springContext != null) {
            springContext.close();
        }
    }
}
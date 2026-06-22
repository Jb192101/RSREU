package org.jedi_bachelor.course_paper_db.client.ui;

import javafx.application.Application;
import javafx.application.Platform;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;
import org.springframework.boot.WebApplicationType;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.boot.autoconfigure.domain.EntityScan;
import org.springframework.boot.builder.SpringApplicationBuilder;
import org.springframework.context.ConfigurableApplicationContext;
import org.springframework.data.jpa.repository.config.EnableJpaRepositories;

@SpringBootApplication(scanBasePackages = "org.jedi_bachelor.course_paper_db.client")
@EntityScan("org.jedi_bachelor.course_paper_db.client.entity")
@EnableJpaRepositories("org.jedi_bachelor.course_paper_db.client.repository")
public class MainApplication extends Application {
    private static ConfigurableApplicationContext springContext;

    private static Stage primaryStage;

    @Override
    public void init() {
        // Запускаем Spring без веб-сервера (для JavaFX)
        springContext = new SpringApplicationBuilder(MainApplication.class)
                .web(WebApplicationType.NONE)  // Отключаем Tomcat
                .run();
    }

    @Override
    public void start(Stage stage) throws Exception {
        primaryStage = stage;

        FXMLLoader loader = new FXMLLoader(getClass().getResource("/fxml/main-view.fxml"));
        loader.setControllerFactory(springContext::getBean);
        Parent root = loader.load();

        Scene scene = new Scene(root, 1200, 750);
        stage.setTitle("Книжный магазин - Информационная система");
        stage.setScene(scene);
        stage.show();

        stage.setOnCloseRequest(event -> {
            Platform.exit();
            springContext.close();
        });
    }

    @Override
    public void stop() {
        if (springContext != null) {
            springContext.close();
        }
    }

    public static Stage getPrimaryStage() {
        return primaryStage;
    }

    public static ConfigurableApplicationContext getSpringContext() {
        return springContext;
    }
}
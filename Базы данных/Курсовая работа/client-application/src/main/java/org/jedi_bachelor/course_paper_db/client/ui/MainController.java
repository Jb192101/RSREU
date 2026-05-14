package org.jedi_bachelor.course_paper_db.client.ui;

import javafx.animation.FadeTransition;
import javafx.animation.TranslateTransition;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.StackPane;
import javafx.scene.layout.VBox;
import javafx.stage.Modality;
import javafx.stage.Stage;
import javafx.util.Duration;
import org.jedi_bachelor.course_paper_db.client.service.BusinessService;
import org.jedi_bachelor.course_paper_db.client.service.RepositoryService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

@Component
public class MainController {

    @FXML private BorderPane mainBorderPane;
    @FXML private VBox navigationMenu;
    @FXML private StackPane contentArea;
    @FXML private Label statusLabel;
    @FXML private Label usernameLabel;
    @FXML private Button toggleMenuButton;

    @Autowired private RepositoryService repositoryService;
    @Autowired private BusinessService businessService;

    private boolean isMenuExpanded = true;
    private final Map<String, Parent> viewCache = new HashMap<>();
    private final Map<String, Object> controllerCache = new HashMap<>();

    @FXML
    public void initialize() {
        setupNavigationButtons();
        loadDefaultView();
        updateStatusBar("Готов к работе");
    }

    private void setupNavigationButtons() {
        // Настраиваем обработчики для кнопок навигации
        for (var node : navigationMenu.getChildren()) {
            if (node instanceof Button button) {
                String viewName = (String) button.getUserData();
                if (viewName != null) {
                    button.setOnAction(event -> loadView(viewName));
                }
            }
        }
    }

    private void loadDefaultView() {
        loadView("books");
    }

    private void loadView(String viewName) {
        try {
            Parent view;

            // Проверяем кэш
            if (viewCache.containsKey(viewName)) {
                view = viewCache.get(viewName);
            } else {
                // Загружаем новое представление
                String fxmlPath = "/fxml/" + viewName + "-view.fxml";
                FXMLLoader loader = new FXMLLoader(getClass().getResource(fxmlPath));
                loader.setControllerFactory(MainApplication.getSpringContext()::getBean);
                view = loader.load();

                viewCache.put(viewName, view);
                controllerCache.put(viewName, loader.getController());
            }

            // Анимация перехода
            FadeTransition fadeOut = new FadeTransition(Duration.millis(150), contentArea.getChildren().get(0));
            fadeOut.setFromValue(1.0);
            fadeOut.setToValue(0.0);

            fadeOut.setOnFinished(event -> {
                contentArea.getChildren().setAll(view);

                FadeTransition fadeIn = new FadeTransition(Duration.millis(150), view);
                fadeIn.setFromValue(0.0);
                fadeIn.setToValue(1.0);
                fadeIn.play();
            });

            fadeOut.play();

            updateStatusBar("Загружен раздел: " + getViewTitle(viewName));

        } catch (IOException e) {
            showError("Ошибка загрузки", "Не удалось загрузить представление: " + viewName);
            updateStatusBar("Ошибка загрузки: " + viewName);
            e.printStackTrace();
        }
    }

    @FXML
    private void toggleMenu() {
        TranslateTransition transition = new TranslateTransition(Duration.millis(300), navigationMenu);

        if (isMenuExpanded) {
            transition.setToX(-navigationMenu.getWidth());
            toggleMenuButton.setText("☰");
        } else {
            transition.setToX(0);
            toggleMenuButton.setText("✕");
        }

        transition.play();
        isMenuExpanded = !isMenuExpanded;
    }

    @FXML
    private void showAboutDialog() {
        Alert alert = new Alert(Alert.AlertType.INFORMATION);
        alert.setTitle("О программе");
        alert.setHeaderText("Книжный магазин - Информационная система");
        alert.setContentText("Версия 1.0\n\n" +
                "Разработчик: Барышев Г.А.\n" +
                "Группа: 3413\n\n" +
                "Рязанский государственный радиотехнический университет\n" +
                "имени В.Ф. Уткина\n" +
                "Кафедра ЭВМ\n" +
                "2026 год");

        alert.showAndWait();
    }

    @FXML
    private void exitApplication() {
        Alert alert = new Alert(Alert.AlertType.CONFIRMATION);
        alert.setTitle("Подтверждение выхода");
        alert.setHeaderText("Вы действительно хотите выйти?");
        alert.setContentText("Все несохранённые данные будут потеряны.");

        alert.showAndWait().ifPresent(response -> {
            if (response == ButtonType.OK) {
                javafx.application.Platform.exit();
            }
        });
    }

    @FXML
    private void openReports() {
        loadView("reports");
    }

    @FXML
    private void refreshCurrentView() {
        // Очищаем кэш и перезагружаем текущий раздел
        viewCache.clear();
        controllerCache.clear();
        loadDefaultView();
        updateStatusBar("Данные обновлены");
    }

    private void updateStatusBar(String message) {
        statusLabel.setText(message);
    }

    private String getViewTitle(String viewName) {
        return switch (viewName) {
            case "books" -> "Книги";
            case "authors" -> "Авторы";
            case "publishers" -> "Издательства";
            case "genres" -> "Жанры";
            case "clients" -> "Клиенты";
            case "orders" -> "Заказы";
            case "inventory" -> "Склад";
            case "promocodes" -> "Промокоды";
            case "reviews" -> "Отзывы";
            case "delivery" -> "Доставка";
            case "reports" -> "Отчёты";
            default -> viewName;
        };
    }

    private void showError(String title, String message) {
        Alert alert = new Alert(Alert.AlertType.ERROR);
        alert.setTitle(title);
        alert.setHeaderText(null);
        alert.setContentText(message);
        alert.showAndWait();
    }
}
package org.jedi_bachelor;

import javafx.application.Application;
import javafx.geometry.Insets;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.VBox;
import javafx.stage.Stage;

public class Main extends Application {
    @Override
    public void start(Stage primaryStage) {
        // Создаем элементы интерфейса
        Label aLabel = new Label("Координата a:");
        TextField aField = new TextField();
        aField.setPromptText("Введите координату a");

        Label bLabel = new Label("Координата b:");
        TextField bField = new TextField();
        bField.setPromptText("Введите координату b");

        Label angleLabel = new Label("Угол α (в градусах):");
        TextField angleField = new TextField();
        angleField.setPromptText("Введите угол поворота");

        Button calculateButton = new Button("Вычислить матрицу");

        Label resultLabel = new Label("Матрица поворота будет отображена здесь");
        resultLabel.setWrapText(true);

        calculateButton.setOnAction(e -> {
            try {
                double a = Double.parseDouble(aField.getText());
                double b = Double.parseDouble(bField.getText());
                double alphaDeg = Double.parseDouble(angleField.getText());

                double alphaRad = Math.toRadians(alphaDeg);
                double cosAlpha = Math.cos(alphaRad);
                double sinAlpha = Math.sin(alphaRad);

                double m1 = -a*cosAlpha + b*sinAlpha + a;
                double m2 = -a*sinAlpha -b*cosAlpha+b;

                String matrix = String.format(
                        "Матрица поворота вокруг точки (%.2f, %.2f) на угол %.2f°:\n" +
                                "[ %.4f  %.4f  0.0000 ]\n" +
                                "[ %.4f  %.4f  0.0000 ]\n" +
                                "[ %.4f  %.4f  1.0000 ]",
                        a, b, alphaDeg, cosAlpha, sinAlpha, alphaDeg, -sinAlpha, cosAlpha, m1, m2
                );

                resultLabel.setText(matrix);
            } catch (NumberFormatException ex) {
                resultLabel.setText("Ошибка: пожалуйста, введите корректные числовые значения");
            }
        });

        // Создаем layout
        GridPane inputGrid = new GridPane();
        inputGrid.setHgap(10);
        inputGrid.setVgap(10);
        inputGrid.setPadding(new Insets(10));

        inputGrid.add(aLabel, 0, 0);
        inputGrid.add(aField, 1, 0);
        inputGrid.add(bLabel, 0, 1);
        inputGrid.add(bField, 1, 1);
        inputGrid.add(angleLabel, 0, 2);
        inputGrid.add(angleField, 1, 2);

        VBox root = new VBox(10);
        root.setPadding(new Insets(10));
        root.getChildren().addAll(inputGrid, calculateButton, resultLabel);

        Scene scene = new Scene(root, 400, 300);
        primaryStage.setTitle("Матрица поворота вокруг точки");
        primaryStage.setScene(scene);
        primaryStage.show();
    }

    public static void main(String[] args) {
        launch(args);
    }
}
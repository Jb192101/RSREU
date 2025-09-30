import javafx.geometry.Point2D;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import javafx.scene.shape.LineTo;
import javafx.scene.shape.MoveTo;
import javafx.scene.shape.Path;
import javafx.stage.Stage;
import javafx.scene.layout.Pane;

import java.util.ArrayList;

public class MainWindow extends Stage {
    private Pane drawingPane;
    private TextField x1Field, y1Field, x2Field, y2Field;
    private Label errorLabel;

    ViewModel vm;

    public MainWindow(ViewModel _vm) {
        this.vm = _vm;

        x1Field = new TextField();
        y1Field = new TextField();
        x2Field = new TextField();
        y2Field = new TextField();

        Label x1Label = new Label("X1:");
        Label y1Label = new Label("Y1:");
        Label x2Label = new Label("X2:");
        Label y2Label = new Label("Y2:");

        errorLabel = new Label("");

        Button paintButton = new Button("Нарисовать");
        paintButton.setOnAction(e -> drawLine());

        VBox inputBox = new VBox(10);
        inputBox.getChildren().addAll(
            new HBox(5, x1Label, x1Field),
            new HBox(5, y1Label, y1Field),
            new HBox(5, x2Label, x2Field),
            new HBox(5, y2Label, y2Field),
            new HBox(),
            new HBox(errorLabel),
            new HBox(),
            new HBox(paintButton)
        );

        drawingPane = new Pane();
        drawingPane.setPrefSize(400, 400);
        drawingPane.setStyle("-fx-background-color: white;");

        HBox root = new HBox(10);
        root.getChildren().addAll(inputBox, drawingPane);

        Scene scene = new Scene(root, 600, 400);

        Stage primaryStage = new Stage();
        primaryStage.setTitle("Построение прямой по координатам");
        primaryStage.setScene(scene);
        primaryStage.show();
    }

    // Осталось реализовать
    private void drawLine() {
        ArrayList<Point2D> points = null;
        try {
            int x1 = Integer.parseInt(x1Field.getText());
            int y1 = Integer.parseInt(y1Field.getText());
            int x2 = Integer.parseInt(x2Field.getText());
            int y2 = Integer.parseInt(y2Field.getText());
            points = vm.getModel().drawLine(x1, y1, x2, y2);
            errorLabel.setText("");
        } catch (NumberFormatException e) {
            errorLabel.setText("Все значения должны\nбыть введены корректно");
        }

        if (points.size() < 2) return;

        Path path = new Path();
        path.setStroke(Color.BLACK);
        path.setFill(null);

        MoveTo moveTo = new MoveTo(points.get(0).getX(), points.get(0).getY());
        path.getElements().add(moveTo);

        for (int i = 1; i < points.size(); i++) {
            Point2D point = points.get(i);
            path.getElements().add(new LineTo(point.getX(), point.getY()));
        }

        drawingPane.getChildren().add(path);
    }
}

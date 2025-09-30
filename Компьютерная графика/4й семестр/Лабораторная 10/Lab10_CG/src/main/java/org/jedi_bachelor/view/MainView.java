package org.jedi_bachelor.view;

import javafx.scene.Parent;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.Slider;
import javafx.scene.control.TextField;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import org.jedi_bachelor.viewmodel.LineViewModel;

public class MainView {
    private final LineViewModel viewModel;
    private final LineView lineView;

    public MainView(LineViewModel viewModel) {
        this.viewModel = viewModel;
        this.lineView = new LineView(viewModel);
    }

    public Parent getView() {
        BorderPane layout = new BorderPane();

        layout.setCenter(lineView.getView());

        VBox controls = new VBox(10);

        Slider angleSlider = new Slider(-180, 180, 0);
        angleSlider.setShowTickLabels(true);
        angleSlider.setShowTickMarks(true);
        angleSlider.setMajorTickUnit(90);
        angleSlider.setMinorTickCount(45);
        angleSlider.valueProperty().bindBidirectional(viewModel.rotationAngleProperty());

        TextField pivotXField = new TextField();
        pivotXField.textProperty().bindBidirectional(viewModel.pivotXProperty(), new javafx.util.StringConverter<Number>() {
            @Override public String toString(Number object) { return String.format("%.1f", object.doubleValue()); }
            @Override public Number fromString(String string) { return Double.parseDouble(string); }
        });

        TextField pivotYField = new TextField();
        pivotYField.textProperty().bindBidirectional(viewModel.pivotYProperty(), new javafx.util.StringConverter<Number>() {
            @Override public String toString(Number object) { return String.format("%.1f", object.doubleValue()); }
            @Override public Number fromString(String string) { return Double.parseDouble(string); }
        });

        Button centerPivotButton = new Button("Center Pivot");
        centerPivotButton.setOnAction(e -> viewModel.updatePivotToCenter());

        HBox pivotControls = new HBox(10,
                new Label("Pivot X:"), pivotXField,
                new Label("Pivot Y:"), pivotYField,
                centerPivotButton
        );

        controls.getChildren().addAll(
                new Label("Rotation Angle:"),
                angleSlider,
                pivotControls
        );

        layout.setBottom(controls);

        return layout;
    }
}

package org.jedi_bachelor.view;

import javafx.scene.Parent;
import javafx.scene.layout.Pane;
import javafx.scene.paint.Color;
import javafx.scene.shape.Circle;
import javafx.scene.shape.Line;
import org.jedi_bachelor.viewmodel.LineViewModel;

public class LineView {
    private final Pane pane;
    private final LineViewModel viewModel;
    private Line originalLineShape;
    private Line rotatedLineShape;
    private Circle pivotMarker;

    public LineView(LineViewModel viewModel) {
        this.viewModel = viewModel;
        this.pane = new Pane();
        this.pane.setPrefSize(600, 400);

        initializeShapes();
        setupBindings();
    }

    private void initializeShapes() {
        originalLineShape = new Line();
        originalLineShape.setStroke(Color.BLUE);
        originalLineShape.setStrokeWidth(2);

        rotatedLineShape = new Line();
        rotatedLineShape.setStroke(Color.RED);
        rotatedLineShape.setStrokeWidth(2);
        rotatedLineShape.getStrokeDashArray().addAll(5d, 5d);

        pivotMarker = new Circle(5, Color.GREEN);

        pane.getChildren().addAll(originalLineShape, rotatedLineShape, pivotMarker);
    }

    private void setupBindings() {
        viewModel.originalLineProperty().addListener((obs, oldLine, newLine) -> updateLineShapes());
        viewModel.rotatedLineProperty().addListener((obs, oldLine, newLine) -> updateLineShapes());
        viewModel.pivotXProperty().addListener((obs, oldVal, newVal) -> updatePivotMarker());
        viewModel.pivotYProperty().addListener((obs, oldVal, newVal) -> updatePivotMarker());

        updateLineShapes();
        updatePivotMarker();
    }

    private void updateLineShapes() {
        originalLineShape.setStartX(viewModel.originalLineProperty().get().getStartX());
        originalLineShape.setStartY(viewModel.originalLineProperty().get().getStartY());
        originalLineShape.setEndX(viewModel.originalLineProperty().get().getEndX());
        originalLineShape.setEndY(viewModel.originalLineProperty().get().getEndY());

        rotatedLineShape.setStartX(viewModel.rotatedLineProperty().get().getStartX());
        rotatedLineShape.setStartY(viewModel.rotatedLineProperty().get().getStartY());
        rotatedLineShape.setEndX(viewModel.rotatedLineProperty().get().getEndX());
        rotatedLineShape.setEndY(viewModel.rotatedLineProperty().get().getEndY());
    }

    private void updatePivotMarker() {
        pivotMarker.setCenterX(viewModel.pivotXProperty().get());
        pivotMarker.setCenterY(viewModel.pivotYProperty().get());
    }

    public Parent getView() {
        return pane;
    }
}

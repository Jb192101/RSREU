package org.jedi_bachelor.viewmodel;

import org.jedi_bachelor.model.AffineTransformation;
import org.jedi_bachelor.model.Line;
import javafx.beans.property.DoubleProperty;
import javafx.beans.property.SimpleDoubleProperty;
import javafx.beans.property.SimpleObjectProperty;
import javafx.beans.property.ObjectProperty;

public class LineViewModel {
    private final ObjectProperty<Line> originalLine = new SimpleObjectProperty<>();
    private final ObjectProperty<Line> rotatedLine = new SimpleObjectProperty<>();
    private final DoubleProperty rotationAngle = new SimpleDoubleProperty();
    private final DoubleProperty pivotX = new SimpleDoubleProperty();
    private final DoubleProperty pivotY = new SimpleDoubleProperty();

    public LineViewModel() {
        originalLine.set(new Line(100, 100, 300, 200));
        rotatedLine.set(new Line(100, 100, 300, 200));

        updatePivotToCenter();

        rotationAngle.addListener((obs, oldVal, newVal) -> updateRotatedLine());
        pivotX.addListener((obs, oldVal, newVal) -> updateRotatedLine());
        pivotY.addListener((obs, oldVal, newVal) -> updateRotatedLine());
    }

    private void updateRotatedLine() {
        Line original = originalLine.get();
        if (original != null) {
            Line rotated = AffineTransformation.rotateLine(
                    original,
                    rotationAngle.get(),
                    pivotX.get(),
                    pivotY.get()
            );
            rotatedLine.set(rotated);
        }
    }

    public void updatePivotToCenter() {
        Line line = originalLine.get();
        if (line != null) {
            pivotX.set((line.getStartX() + line.getEndX()) / 2);
            pivotY.set((line.getStartY() + line.getEndY()) / 2);
        }
    }

    public ObjectProperty<Line> originalLineProperty() { return originalLine; }
    public ObjectProperty<Line> rotatedLineProperty() { return rotatedLine; }
    public DoubleProperty rotationAngleProperty() { return rotationAngle; }
    public DoubleProperty pivotXProperty() { return pivotX; }
    public DoubleProperty pivotYProperty() { return pivotY; }
}
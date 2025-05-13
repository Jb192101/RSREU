package org.jedi_bachelor.model;

public class AffineTransformation {
    public static Line rotateLine(Line line, double angleDegrees, double pivotX, double pivotY) {
        double angleRad = Math.toRadians(angleDegrees);
        double cos = Math.cos(angleRad);
        double sin = Math.sin(angleRad);

        double startX = line.getStartX() - pivotX;
        double startY = line.getStartY() - pivotY;
        double endX = line.getEndX() - pivotX;
        double endY = line.getEndY() - pivotY;

        double newStartX = startX * cos - startY * sin;
        double newStartY = startX * sin + startY * cos;
        double newEndX = endX * cos - endY * sin;
        double newEndY = endX * sin + endY * cos;

        newStartX += pivotX;
        newStartY += pivotY;
        newEndX += pivotX;
        newEndY += pivotY;

        return new Line(newStartX, newStartY, newEndX, newEndY);
    }
}

package org.jedi_bachelor.course_paper_db.client.ui.controllers;

import javafx.fxml.FXML;
import javafx.scene.control.*;
import org.jedi_bachelor.course_paper_db.client.service.BusinessService;
import org.jedi_bachelor.course_paper_db.client.service.RepositoryService;
import org.springframework.beans.factory.annotation.Autowired;

public abstract class BaseController {

    @Autowired protected RepositoryService repositoryService;
    @Autowired protected BusinessService businessService;

    @FXML protected TableView<?> dataTable;
    @FXML protected Button addButton;
    @FXML protected Button editButton;
    @FXML protected Button deleteButton;
    @FXML protected Button refreshButton;
    @FXML protected TextField searchField;

    @FXML
    public abstract void initialize();

    @FXML
    protected abstract void handleAdd();

    @FXML
    protected abstract void handleEdit();

    @FXML
    protected abstract void handleDelete();

    @FXML
    protected abstract void handleRefresh();

    @FXML
    protected abstract void handleSearch();

    protected void showError(String title, String message) {
        Alert alert = new Alert(Alert.AlertType.ERROR);
        alert.setTitle(title);
        alert.setHeaderText(null);
        alert.setContentText(message);
        alert.showAndWait();
    }

    protected void showInfo(String title, String message) {
        Alert alert = new Alert(Alert.AlertType.INFORMATION);
        alert.setTitle(title);
        alert.setHeaderText(null);
        alert.setContentText(message);
        alert.showAndWait();
    }

    protected boolean showConfirmation(String title, String message) {
        Alert alert = new Alert(Alert.AlertType.CONFIRMATION);
        alert.setTitle(title);
        alert.setHeaderText(null);
        alert.setContentText(message);

        return alert.showAndWait()
                .filter(response -> response == ButtonType.OK)
                .isPresent();
    }
}
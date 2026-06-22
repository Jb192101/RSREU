package org.jedi_bachelor.course_paper_db.client.ui.controllers;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.geometry.Insets;
import javafx.scene.control.*;
import javafx.scene.control.cell.PropertyValueFactory;
import javafx.scene.layout.GridPane;
import org.jedi_bachelor.course_paper_db.client.entity.Publisher;
import org.springframework.stereotype.Component;

import java.util.List;

@Component
public class PublishersController extends BaseController {

    @FXML private TableView<Publisher> publishersTable;
    @FXML private TableColumn<Publisher, Long> idColumn;
    @FXML private TableColumn<Publisher, String> nameColumn;
    @FXML private TableColumn<Publisher, String> phoneColumn;
    @FXML private TableColumn<Publisher, String> emailColumn;
    @FXML private TableColumn<Publisher, String> addressColumn;

    private ObservableList<Publisher> publishersList = FXCollections.observableArrayList();

    @FXML
    public void initialize() {
        setupTableColumns();
        handleRefresh();
    }

    private void setupTableColumns() {
        idColumn.setCellValueFactory(new PropertyValueFactory<>("id"));
        nameColumn.setCellValueFactory(new PropertyValueFactory<>("name"));
        phoneColumn.setCellValueFactory(new PropertyValueFactory<>("phone"));
        emailColumn.setCellValueFactory(new PropertyValueFactory<>("email"));
        addressColumn.setCellValueFactory(new PropertyValueFactory<>("address"));

        publishersTable.setItems(publishersList);
    }

    @Override
    protected void handleAdd() {
        showPublisherDialog("Добавление издательства", null);
    }

    @Override
    protected void handleEdit() {
        Publisher selected = publishersTable.getSelectionModel().getSelectedItem();
        if (selected == null) {
            showError("Ошибка", "Выберите издательство");
            return;
        }
        showPublisherDialog("Редактирование издательства", selected);
    }

    @Override
    protected void handleDelete() {
        Publisher selected = publishersTable.getSelectionModel().getSelectedItem();
        if (selected == null) {
            showError("Ошибка", "Выберите издательство");
            return;
        }

        if (showConfirmation("Подтверждение", "Удалить издательство \"" + selected.getName() + "\"?")) {
            try {
                repositoryService.deletePublisher(selected.getId());
                handleRefresh();
                showInfo("Успех", "Издательство удалено");
            } catch (Exception e) {
                showError("Ошибка", e.getMessage());
            }
        }
    }

    @Override
    protected void handleRefresh() {
        try {
            List<Publisher> publishers = repositoryService.getAllPublishers();
            publishersList.setAll(publishers);
            if (statusLabel != null) {
                statusLabel.setText("Загружено издательств: " + publishers.size());
            }
        } catch (Exception e) {
            showError("Ошибка", "Не удалось загрузить издательства: " + e.getMessage());
        }
    }

    @Override
    protected void handleSearch() {
        String searchTerm = searchField.getText();
        if (searchTerm == null || searchTerm.trim().isEmpty()) {
            handleRefresh();
            return;
        }

        List<Publisher> filtered = publishersList.stream()
                .filter(p -> p.getName().toLowerCase().contains(searchTerm.toLowerCase()))
                .toList();
        publishersList.setAll(filtered);
    }

    private void showPublisherDialog(String title, Publisher publisher) {
        Dialog<Publisher> dialog = new Dialog<>();
        dialog.setTitle(title);

        ButtonType saveButtonType = new ButtonType("Сохранить", ButtonBar.ButtonData.OK_DONE);
        dialog.getDialogPane().getButtonTypes().addAll(saveButtonType, ButtonType.CANCEL);

        GridPane grid = new GridPane();
        grid.setHgap(10);
        grid.setVgap(10);
        grid.setPadding(new Insets(20, 150, 10, 10));

        TextField nameField = new TextField();
        TextField phoneField = new TextField();
        TextField emailField = new TextField();
        TextArea addressArea = new TextArea();
        addressArea.setPrefRowCount(3);

        if (publisher != null) {
            nameField.setText(publisher.getName());
            phoneField.setText(publisher.getPhone());
            emailField.setText(publisher.getEmail());
            addressArea.setText(publisher.getAddress());
        }

        grid.add(new Label("Название:"), 0, 0);
        grid.add(nameField, 1, 0);
        grid.add(new Label("Телефон:"), 0, 1);
        grid.add(phoneField, 1, 1);
        grid.add(new Label("Email:"), 0, 2);
        grid.add(emailField, 1, 2);
        grid.add(new Label("Адрес:"), 0, 3);
        grid.add(addressArea, 1, 3);

        dialog.getDialogPane().setContent(grid);

        dialog.setResultConverter(dialogButton -> {
            if (dialogButton == saveButtonType) {
                Publisher newPublisher = publisher != null ? publisher : new Publisher();
                newPublisher.setName(nameField.getText());
                newPublisher.setPhone(phoneField.getText());
                newPublisher.setEmail(emailField.getText());
                newPublisher.setAddress(addressArea.getText());
                return newPublisher;
            }
            return null;
        });

        dialog.showAndWait().ifPresent(result -> {
            try {
                if (publisher == null) {
                    repositoryService.createPublisher(result);
                } else {
                    repositoryService.updatePublisher(result);
                }
                handleRefresh();
                showInfo("Успех", "Издательство сохранено");
            } catch (Exception e) {
                showError("Ошибка", e.getMessage());
            }
        });
    }
}
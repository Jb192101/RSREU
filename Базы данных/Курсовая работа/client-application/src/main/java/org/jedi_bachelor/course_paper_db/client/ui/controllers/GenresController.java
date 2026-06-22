package org.jedi_bachelor.course_paper_db.client.ui.controllers;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.geometry.Insets;
import javafx.scene.control.*;
import javafx.scene.control.cell.PropertyValueFactory;
import javafx.scene.layout.GridPane;
import org.jedi_bachelor.course_paper_db.client.entity.Genre;
import org.springframework.stereotype.Component;

import java.util.List;

@Component
public class GenresController extends BaseController {

    @FXML private TableView<Genre> genresTable;
    @FXML private TableColumn<Genre, Long> idColumn;
    @FXML private TableColumn<Genre, String> nameColumn;
    @FXML private TableColumn<Genre, String> descriptionColumn;

    private ObservableList<Genre> genresList = FXCollections.observableArrayList();

    @FXML
    public void initialize() {
        setupTableColumns();
        handleRefresh();
    }

    private void setupTableColumns() {
        idColumn.setCellValueFactory(new PropertyValueFactory<>("id"));
        nameColumn.setCellValueFactory(new PropertyValueFactory<>("name"));
        descriptionColumn.setCellValueFactory(new PropertyValueFactory<>("description"));

        genresTable.setItems(genresList);
    }

    @Override
    protected void handleAdd() {
        showGenreDialog("Добавление жанра", null);
    }

    @Override
    protected void handleEdit() {
        Genre selected = genresTable.getSelectionModel().getSelectedItem();
        if (selected == null) {
            showError("Ошибка", "Выберите жанр для редактирования");
            return;
        }
        showGenreDialog("Редактирование жанра", selected);
    }

    @Override
    protected void handleDelete() {
        Genre selected = genresTable.getSelectionModel().getSelectedItem();
        if (selected == null) {
            showError("Ошибка", "Выберите жанр для удаления");
            return;
        }

        if (showConfirmation("Подтверждение", "Удалить жанр \"" + selected.getName() + "\"?")) {
            try {
                repositoryService.deleteGenre(selected.getId());
                handleRefresh();
                showInfo("Успех", "Жанр удалён");
            } catch (Exception e) {
                e.printStackTrace();
                showError("Ошибка", "Не удалось удалить жанр: " + e.getMessage());
            }
        }
    }

    @Override
    protected void handleRefresh() {
        try {
            List<Genre> genres = repositoryService.getAllGenres();
            genresList.setAll(genres);
            updateStatus("Загружено жанров: " + genres.size());
        } catch (Exception e) {
            e.printStackTrace();
            showError("Ошибка", "Не удалось загрузить жанры: " + e.getMessage());
        }
    }

    @Override
    protected void handleSearch() {
        String searchTerm = searchField.getText();
        if (searchTerm == null || searchTerm.trim().isEmpty()) {
            handleRefresh();
            return;
        }

        List<Genre> filtered = genresList.stream()
                .filter(g -> g.getName().toLowerCase().contains(searchTerm.toLowerCase()))
                .toList();
        genresList.setAll(filtered);
        updateStatus("Найдено жанров: " + filtered.size());
    }

    private void showGenreDialog(String title, Genre genre) {
        Dialog<Genre> dialog = new Dialog<>();
        dialog.setTitle(title);
        dialog.setHeaderText(null);

        ButtonType saveButtonType = new ButtonType("Сохранить", ButtonBar.ButtonData.OK_DONE);
        dialog.getDialogPane().getButtonTypes().addAll(saveButtonType, ButtonType.CANCEL);

        GridPane grid = new GridPane();
        grid.setHgap(10);
        grid.setVgap(10);
        grid.setPadding(new Insets(20, 150, 10, 10));

        TextField nameField = new TextField();
        nameField.setPromptText("Введите название жанра");
        nameField.setPrefWidth(300);

        TextArea descriptionArea = new TextArea();
        descriptionArea.setPromptText("Введите описание жанра");
        descriptionArea.setPrefRowCount(3);
        descriptionArea.setPrefWidth(300);

        // Если редактируем — заполняем поля
        if (genre != null) {
            nameField.setText(genre.getName());
            descriptionArea.setText(genre.getDescription());
        }

        grid.add(new Label("Название:*"), 0, 0);
        grid.add(nameField, 1, 0);
        grid.add(new Label("Описание:"), 0, 1);
        grid.add(descriptionArea, 1, 1);

        Label infoLabel = new Label("* — обязательное поле");
        infoLabel.setStyle("-fx-text-fill: #7f8c8d; -fx-font-size: 11px;");
        grid.add(infoLabel, 0, 2, 2, 1);

        dialog.getDialogPane().setContent(grid);

        // Валидация перед сохранением
        Button saveButton = (Button) dialog.getDialogPane().lookupButton(saveButtonType);
        saveButton.addEventFilter(javafx.event.ActionEvent.ACTION, event -> {
            if (nameField.getText().trim().isEmpty()) {
                showError("Ошибка", "Название жанра обязательно для заполнения");
                event.consume();
            }
        });

        dialog.setResultConverter(dialogButton -> {
            if (dialogButton == saveButtonType) {
                Genre newGenre = genre != null ? genre : new Genre();
                newGenre.setName(nameField.getText().trim());
                newGenre.setDescription(descriptionArea.getText().trim().isEmpty() ? null : descriptionArea.getText().trim());
                return newGenre;
            }
            return null;
        });

        dialog.showAndWait().ifPresent(result -> {
            try {
                if (genre == null) {
                    repositoryService.createGenre(result);
                    showInfo("Успех", "Жанр добавлен");
                } else {
                    repositoryService.updateGenre(result);
                    showInfo("Успех", "Жанр обновлён");
                }
                handleRefresh();
            } catch (Exception e) {
                e.printStackTrace();
                showError("Ошибка", "Не удалось сохранить жанр: " + e.getMessage());
            }
        });
    }
}
package org.jedi_bachelor.course_paper_db.client.ui.controllers;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.geometry.Insets;
import javafx.scene.control.*;
import javafx.scene.control.cell.PropertyValueFactory;
import javafx.scene.layout.GridPane;
import org.jedi_bachelor.course_paper_db.client.entity.Author;
import org.springframework.stereotype.Component;

import java.time.LocalDate;
import java.time.format.DateTimeFormatter;
import java.util.List;

@Component
public class AuthorsController extends BaseController {

    @FXML private TableView<Author> authorsTable;
    @FXML private TableColumn<Author, Long> idColumn;
    @FXML private TableColumn<Author, String> firstNameColumn;
    @FXML private TableColumn<Author, String> lastNameColumn;
    @FXML private TableColumn<Author, String> countryColumn;
    @FXML private TableColumn<Author, String> birthDateColumn;
    @FXML private TableColumn<Author, String> biographyColumn;

    private ObservableList<Author> authorsList = FXCollections.observableArrayList();
    private final DateTimeFormatter dateFormatter = DateTimeFormatter.ofPattern("dd.MM.yyyy");

    @FXML
    public void initialize() {
        setupTableColumns();
        handleRefresh();
    }

    private void setupTableColumns() {
        idColumn.setCellValueFactory(new PropertyValueFactory<>("id"));
        firstNameColumn.setCellValueFactory(new PropertyValueFactory<>("firstName"));
        lastNameColumn.setCellValueFactory(new PropertyValueFactory<>("lastName"));
        countryColumn.setCellValueFactory(new PropertyValueFactory<>("country"));

        birthDateColumn.setCellValueFactory(cellData -> {
            Author author = cellData.getValue();
            if (author.getBirthDate() != null) {
                return new javafx.beans.property.SimpleStringProperty(
                        author.getBirthDate().format(dateFormatter)
                );
            }
            return new javafx.beans.property.SimpleStringProperty("");
        });

        biographyColumn.setCellValueFactory(new PropertyValueFactory<>("biography"));

        authorsTable.setItems(authorsList);
    }

    @Override
    protected void handleAdd() {
        showAuthorDialog("Добавление автора", null);
    }

    @Override
    protected void handleEdit() {
        Author selected = authorsTable.getSelectionModel().getSelectedItem();
        if (selected == null) {
            showError("Ошибка", "Выберите автора для редактирования");
            return;
        }
        showAuthorDialog("Редактирование автора", selected);
    }

    @Override
    protected void handleDelete() {
        Author selected = authorsTable.getSelectionModel().getSelectedItem();
        if (selected == null) {
            showError("Ошибка", "Выберите автора для удаления");
            return;
        }

        if (showConfirmation("Подтверждение", "Удалить автора \"" + selected.getFirstName() + " " + selected.getLastName() + "\"?")) {
            try {
                repositoryService.deleteAuthor(selected.getId());
                handleRefresh();
                showInfo("Успех", "Автор удалён");
            } catch (Exception e) {
                showError("Ошибка", "Не удалось удалить автора: " + e.getMessage());
            }
        }
    }

    @Override
    protected void handleRefresh() {
        try {
            List<Author> authors = repositoryService.getAllAuthors();
            authorsList.setAll(authors);
            updateStatus("Загружено авторов: " + authors.size());
        } catch (Exception e) {
            e.printStackTrace();
            showError("Ошибка", "Не удалось загрузить авторов: " + e.getMessage());
        }
    }

    @Override
    protected void handleSearch() {
        String searchTerm = searchField.getText();
        if (searchTerm == null || searchTerm.trim().isEmpty()) {
            handleRefresh();
            return;
        }

        List<Author> filtered = authorsList.stream()
                .filter(a -> a.getFirstName().toLowerCase().contains(searchTerm.toLowerCase()) ||
                        a.getLastName().toLowerCase().contains(searchTerm.toLowerCase()) ||
                        (a.getCountry() != null && a.getCountry().toLowerCase().contains(searchTerm.toLowerCase())))
                .toList();
        authorsList.setAll(filtered);
        updateStatus("Найдено авторов: " + filtered.size());
    }

    @FXML
    public void handleMergeAuthors() {
        Author selected = authorsTable.getSelectionModel().getSelectedItem();
        if (selected == null) {
            showError("Ошибка", "Выберите исходного автора для объединения");
            return;
        }

        TextInputDialog dialog = new TextInputDialog();
        dialog.setTitle("Объединение авторов");
        dialog.setHeaderText("Исходный автор: " + selected.getFirstName() + " " + selected.getLastName());
        dialog.setContentText("Введите ID целевого автора (останется в системе):");

        dialog.showAndWait().ifPresent(targetIdStr -> {
            try {
                Long targetId = Long.parseLong(targetIdStr);
                List<Object[]> result = businessService.mergeAuthors(selected.getId(), targetId);
                if (!result.isEmpty()) {
                    Object[] data = result.get(0);
                    showInfo("Успех", "Объединено книг: " + data[0] +
                            "\nАвтор " + data[1] + " объединён с " + data[2]);
                    handleRefresh();
                }
            } catch (Exception e) {
                showError("Ошибка", "Не удалось объединить авторов: " + e.getMessage());
            }
        });
    }

    /**
     * Диалог для добавления/редактирования автора
     */
    private void showAuthorDialog(String title, Author author) {
        Dialog<Author> dialog = new Dialog<>();
        dialog.setTitle(title);
        dialog.setHeaderText(null);

        ButtonType saveButtonType = new ButtonType("Сохранить", ButtonBar.ButtonData.OK_DONE);
        dialog.getDialogPane().getButtonTypes().addAll(saveButtonType, ButtonType.CANCEL);

        GridPane grid = new GridPane();
        grid.setHgap(10);
        grid.setVgap(10);
        grid.setPadding(new Insets(20, 150, 10, 10));

        TextField firstNameField = new TextField();
        firstNameField.setPromptText("Имя");
        TextField lastNameField = new TextField();
        lastNameField.setPromptText("Фамилия");
        TextField countryField = new TextField();
        countryField.setPromptText("Страна");
        DatePicker birthDatePicker = new DatePicker();
        birthDatePicker.setPromptText("Дата рождения");
        TextArea biographyArea = new TextArea();
        biographyArea.setPromptText("Биография");
        biographyArea.setPrefRowCount(4);
        biographyArea.setPrefWidth(300);

        if (author != null) {
            firstNameField.setText(author.getFirstName());
            lastNameField.setText(author.getLastName());
            countryField.setText(author.getCountry());
            if (author.getBirthDate() != null) {
                birthDatePicker.setValue(author.getBirthDate());
            }
            biographyArea.setText(author.getBiography());
        }

        grid.add(new Label("Имя:*"), 0, 0);
        grid.add(firstNameField, 1, 0);
        grid.add(new Label("Фамилия:*"), 0, 1);
        grid.add(lastNameField, 1, 1);
        grid.add(new Label("Страна:"), 0, 2);
        grid.add(countryField, 1, 2);
        grid.add(new Label("Дата рождения:"), 0, 3);
        grid.add(birthDatePicker, 1, 3);
        grid.add(new Label("Биография:"), 0, 4);
        grid.add(biographyArea, 1, 4);

        dialog.getDialogPane().setContent(grid);

        Button saveButton = (Button) dialog.getDialogPane().lookupButton(saveButtonType);
        saveButton.addEventFilter(javafx.event.ActionEvent.ACTION, event -> {
            if (firstNameField.getText().trim().isEmpty() || lastNameField.getText().trim().isEmpty()) {
                showError("Ошибка", "Имя и фамилия обязательны для заполнения");
                event.consume();
            }
        });

        dialog.setResultConverter(dialogButton -> {
            if (dialogButton == saveButtonType) {
                Author newAuthor = author != null ? author : new Author();
                newAuthor.setFirstName(firstNameField.getText().trim());
                newAuthor.setLastName(lastNameField.getText().trim());
                newAuthor.setCountry(countryField.getText().trim().isEmpty() ? null : countryField.getText().trim());
                newAuthor.setBirthDate(birthDatePicker.getValue());
                newAuthor.setBiography(biographyArea.getText().trim().isEmpty() ? null : biographyArea.getText().trim());
                return newAuthor;
            }
            return null;
        });

        dialog.showAndWait().ifPresent(result -> {
            try {
                if (author == null) {
                    repositoryService.createAuthor(result);
                    showInfo("Успех", "Автор добавлен");
                } else {
                    repositoryService.updateAuthor(result);
                    showInfo("Успех", "Автор обновлён");
                }
                handleRefresh();
            } catch (Exception e) {
                showError("Ошибка", "Не удалось сохранить автора: " + e.getMessage());
                e.printStackTrace();
            }
        });
    }
}
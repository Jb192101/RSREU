package org.jedi_bachelor.course_paper_db.client.ui.controllers;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.geometry.Insets;
import javafx.scene.control.*;
import javafx.scene.control.cell.PropertyValueFactory;
import javafx.scene.layout.GridPane;
import org.jedi_bachelor.course_paper_db.client.entity.Book;
import org.jedi_bachelor.course_paper_db.client.entity.Genre;
import org.jedi_bachelor.course_paper_db.client.entity.Publisher;
import org.springframework.stereotype.Component;

import java.math.BigDecimal;
import java.util.List;

@Component
public class BooksController extends BaseController {

    @FXML private TableView<Book> booksTable;
    @FXML private TableColumn<Book, Long> idColumn;
    @FXML private TableColumn<Book, String> isbnColumn;
    @FXML private TableColumn<Book, String> titleColumn;
    @FXML private TableColumn<Book, Integer> yearColumn;
    @FXML private TableColumn<Book, BigDecimal> priceColumn;
    @FXML private TableColumn<Book, String> genreColumn;
    @FXML private TableColumn<Book, String> publisherColumn;
    @FXML private TableColumn<Book, Integer> pagesColumn;  // ДОБАВИТЬ ЭТУ СТРОКУ

    private ObservableList<Book> booksList = FXCollections.observableArrayList();

    @FXML
    public void initialize() {
        setupTableColumns();
        handleRefresh();
    }

    private void setupTableColumns() {
        idColumn.setCellValueFactory(new PropertyValueFactory<>("id"));
        isbnColumn.setCellValueFactory(new PropertyValueFactory<>("isbn"));
        titleColumn.setCellValueFactory(new PropertyValueFactory<>("title"));
        yearColumn.setCellValueFactory(new PropertyValueFactory<>("publicationYear"));
        priceColumn.setCellValueFactory(new PropertyValueFactory<>("price"));
        pagesColumn.setCellValueFactory(new PropertyValueFactory<>("pages"));  // ДОБАВИТЬ

        genreColumn.setCellValueFactory(cellData ->
                new javafx.beans.property.SimpleStringProperty(
                        cellData.getValue().getGenre() != null ? cellData.getValue().getGenre().getName() : ""
                )
        );
        publisherColumn.setCellValueFactory(cellData ->
                new javafx.beans.property.SimpleStringProperty(
                        cellData.getValue().getPublisher() != null ? cellData.getValue().getPublisher().getName() : ""
                )
        );

        booksTable.setItems(booksList);
    }

    @Override
    protected void handleAdd() {
        try {
            System.out.println("=== handleAdd START ===");
            showDialog("Добавление книги", null);
            System.out.println("=== handleAdd END ===");
        } catch (Exception e) {
            e.printStackTrace();
            showError("КРИТИЧЕСКАЯ ОШИБКА", "Ошибка при добавлении: " + e.getMessage() + "\n\n" +
                    "Стек трейс:\n" + getStackTraceAsString(e));
        }
    }

    @Override
    protected void handleEdit() {
        try {
            System.out.println("=== handleEdit START ===");
            Book selected = booksTable.getSelectionModel().getSelectedItem();
            if (selected == null) {
                showError("Ошибка", "Выберите книгу для редактирования");
                return;
            }
            showDialog("Редактирование книги", selected);
            System.out.println("=== handleEdit END ===");
        } catch (Exception e) {
            e.printStackTrace();
            showError("КРИТИЧЕСКАЯ ОШИБКА", "Ошибка при редактировании: " + e.getMessage() + "\n\n" +
                    "Стек трейс:\n" + getStackTraceAsString(e));
        }
    }

    @Override
    protected void handleDelete() {
        Book selected = booksTable.getSelectionModel().getSelectedItem();
        if (selected == null) {
            showError("Ошибка", "Выберите книгу для удаления");
            return;
        }

        if (showConfirmation("Подтверждение", "Удалить книгу \"" + selected.getTitle() + "\"?")) {
            try {
                repositoryService.deleteBook(selected.getId());
                handleRefresh();
                showInfo("Успех", "Книга удалена");
            } catch (Exception e) {
                showError("Ошибка", "Не удалось удалить книгу: " + e.getMessage());
            }
        }
    }

    @Override
    protected void handleRefresh() {
        try {
            List<Book> books = repositoryService.getAllBooks();
            booksList.setAll(books);
            updateStatus("Загружено книг: " + books.size());  // Используем updateStatus вместо showInfo
        } catch (Exception e) {
            e.printStackTrace();
            showError("Ошибка", "Не удалось загрузить книги: " + e.getMessage());
        }
    }

    @Override
    protected void handleSearch() {
        String searchTerm = searchField.getText();
        if (searchTerm == null || searchTerm.trim().isEmpty()) {
            handleRefresh();
            return;
        }

        try {
            List<Object[]> results = businessService.searchBooks(searchTerm, null, null,
                    null, null, null, null, 1, 100);
            showInfo("Поиск", "Найдено записей: " + results.size());
        } catch (Exception e) {
            showError("Ошибка", "Ошибка поиска: " + e.getMessage());
        }
    }

    @FXML
    public void handleClearSearch() {
        if (searchField != null) {
            searchField.clear();
            handleRefresh();
        }
    }

    @FXML
    public void handleBulkPriceUpdate() {
        TextInputDialog dialog = new TextInputDialog();
        dialog.setTitle("Массовое изменение цен");
        dialog.setHeaderText("Изменение цен на книги");
        dialog.setContentText("Введите процент изменения (например, 10 для увеличения на 10%, -5 для уменьшения):");

        dialog.showAndWait().ifPresent(percent -> {
            try {
                BigDecimal percentage = new BigDecimal(percent);
                List<Object[]> results = businessService.bulkUpdatePrices(null, null, percentage);
                handleRefresh();
                showInfo("Успех", "Цены обновлены");
            } catch (Exception e) {
                showError("Ошибка", "Не удалось обновить цены: " + e.getMessage());
            }
        });
    }

    private void showDialog(String title, Book book) {
        System.out.println("=== ОТКРЫТИЕ ДИАЛОГА ===");
        System.out.println("Title: " + title);
        System.out.println("Book: " + (book != null ? book.getTitle() : "null"));

        Dialog<Book> dialog = new Dialog<>();
        dialog.setTitle(title);
        dialog.setHeaderText(null);

        ButtonType saveButtonType = new ButtonType("Сохранить", ButtonBar.ButtonData.OK_DONE);
        dialog.getDialogPane().getButtonTypes().addAll(saveButtonType, ButtonType.CANCEL);

        GridPane grid = new GridPane();
        grid.setHgap(10);
        grid.setVgap(10);
        grid.setPadding(new Insets(20, 150, 10, 10));

        TextField isbnField = new TextField();
        TextField titleField = new TextField();
        TextField yearField = new TextField();
        TextField priceField = new TextField();
        TextField pagesField = new TextField();
        TextArea descriptionArea = new TextArea();
        descriptionArea.setPrefRowCount(3);

        ComboBox<Publisher> publisherCombo = new ComboBox<>();
        ComboBox<Genre> genreCombo = new ComboBox<>();

        try {
            System.out.println("Загрузка издательств...");
            List<Publisher> publishers = repositoryService.getAllPublishers();
            System.out.println("Издательств загружено: " + (publishers != null ? publishers.size() : 0));
            publisherCombo.setItems(FXCollections.observableArrayList(publishers));

            System.out.println("Загрузка жанров...");
            List<Genre> genres = repositoryService.getAllGenres();
            System.out.println("Жанров загружено: " + (genres != null ? genres.size() : 0));
            genreCombo.setItems(FXCollections.observableArrayList(genres));
        } catch (Exception e) {
            System.err.println("ОШИБКА загрузки справочников:");
            e.printStackTrace();
            showError("Ошибка", "Не удалось загрузить справочные данные: " + e.getMessage());
            return; // Выходим, если не загрузили данные
        }

        if (book != null) {
            isbnField.setText(book.getIsbn());
            titleField.setText(book.getTitle());
            yearField.setText(book.getPublicationYear() != null ? book.getPublicationYear().toString() : "");
            priceField.setText(book.getPrice() != null ? book.getPrice().toString() : "");
            pagesField.setText(book.getPages() != null ? book.getPages().toString() : "");
            descriptionArea.setText(book.getDescription());
            publisherCombo.setValue(book.getPublisher());
            genreCombo.setValue(book.getGenre());
        }

        grid.add(new Label("ISBN:"), 0, 0);
        grid.add(isbnField, 1, 0);
        grid.add(new Label("Название:"), 0, 1);
        grid.add(titleField, 1, 1);
        grid.add(new Label("Год:"), 0, 2);
        grid.add(yearField, 1, 2);
        grid.add(new Label("Цена:"), 0, 3);
        grid.add(priceField, 1, 3);
        grid.add(new Label("Страниц:"), 0, 4);
        grid.add(pagesField, 1, 4);
        grid.add(new Label("Издательство:"), 0, 5);
        grid.add(publisherCombo, 1, 5);
        grid.add(new Label("Жанр:"), 0, 6);
        grid.add(genreCombo, 1, 6);
        grid.add(new Label("Описание:"), 0, 7);
        grid.add(descriptionArea, 1, 7);

        dialog.getDialogPane().setContent(grid);

        dialog.setResultConverter(dialogButton -> {
            if (dialogButton == saveButtonType) {
                try {
                    Book newBook = book != null ? book : new Book();
                    newBook.setIsbn(isbnField.getText());
                    newBook.setTitle(titleField.getText());
                    newBook.setPublicationYear(yearField.getText().isEmpty() ? null : Integer.parseInt(yearField.getText()));
                    newBook.setPrice(new BigDecimal(priceField.getText()));
                    newBook.setPages(pagesField.getText().isEmpty() ? null : Integer.parseInt(pagesField.getText()));
                    newBook.setDescription(descriptionArea.getText());
                    newBook.setPublisher(publisherCombo.getValue());
                    newBook.setGenre(genreCombo.getValue());
                    return newBook;
                } catch (Exception e) {
                    e.printStackTrace();
                    showError("Ошибка", "Некорректные данные: " + e.getMessage());
                    return null;
                }
            }
            return null;
        });

        System.out.println("Показ диалога...");
        dialog.showAndWait().ifPresent(result -> {
            System.out.println("Диалог закрыт, результат: " + (result != null ? result.getTitle() : "null"));
            try {
                if (book == null) {
                    repositoryService.createBook(result);
                } else {
                    repositoryService.updateBook(result);
                }
                handleRefresh();
                showInfo("Успех", "Книга сохранена");
            } catch (Exception e) {
                e.printStackTrace();
                showError("Ошибка", "Не удалось сохранить книгу: " + e.getMessage());
            }
        });
        System.out.println("=== ДИАЛОГ ЗАВЕРШЁН ===");
    }

    private String getStackTraceAsString(Exception e) {
        StringBuilder sb = new StringBuilder();
        for (StackTraceElement element : e.getStackTrace()) {
            sb.append(element.toString()).append("\n");
            if (sb.length() > 1000) break; // Ограничиваем длину
        }
        return sb.toString();
    }
}
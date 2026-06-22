package org.jedi_bachelor.course_paper_db.client.ui.controllers;

import javafx.beans.property.SimpleIntegerProperty;
import javafx.beans.property.SimpleObjectProperty;
import javafx.beans.property.SimpleStringProperty;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.scene.control.*;
import javafx.scene.layout.GridPane;
import org.jedi_bachelor.course_paper_db.client.entity.Book;
import org.jedi_bachelor.course_paper_db.client.entity.BookRemnant;
import org.springframework.stereotype.Component;

import java.time.LocalDate;
import java.time.format.DateTimeFormatter;
import java.util.List;

@Component
public class InventoryController extends BaseController {

    @FXML private TableView<InventoryItem> inventoryTable;
    @FXML private TableColumn<InventoryItem, Long> idColumn;
    @FXML private TableColumn<InventoryItem, String> bookColumn;
    @FXML private TableColumn<InventoryItem, Integer> quantityColumn;
    @FXML private TableColumn<InventoryItem, Integer> reservedColumn;
    @FXML private TableColumn<InventoryItem, Integer> availableColumn;
    @FXML private TableColumn<InventoryItem, String> lastRestockedColumn;
    @FXML private Button refreshButton;

    private ObservableList<InventoryItem> inventoryList = FXCollections.observableArrayList();
    private List<Book> allBooks;

    public static class InventoryItem {
        private final Long id;
        private final String bookTitle;
        private final Integer quantity;
        private final Integer reservedQuantity;
        private final Integer availableQuantity;
        private final LocalDate lastRestocked;
        private final Long bookId;

        public InventoryItem(Long id, String bookTitle, Integer quantity, Integer reservedQuantity,
                             Integer availableQuantity, LocalDate lastRestocked, Long bookId) {
            this.id = id;
            this.bookTitle = bookTitle;
            this.quantity = quantity;
            this.reservedQuantity = reservedQuantity;
            this.availableQuantity = availableQuantity;
            this.lastRestocked = lastRestocked;
            this.bookId = bookId;
        }

        public Long getId() { return id; }
        public String getBookTitle() { return bookTitle; }
        public Integer getQuantity() { return quantity; }
        public Integer getReservedQuantity() { return reservedQuantity; }
        public Integer getAvailableQuantity() { return availableQuantity; }
        public LocalDate getLastRestocked() { return lastRestocked; }
        public Long getBookId() { return bookId; }
    }

    @FXML
    public void initialize() {
        setupTableColumns();
        loadBooks();
        handleRefresh();
    }

    private void setupTableColumns() {
        idColumn.setCellValueFactory(cellData -> new SimpleObjectProperty<>(cellData.getValue().getId()));
        bookColumn.setCellValueFactory(cellData -> new SimpleStringProperty(cellData.getValue().getBookTitle()));
        quantityColumn.setCellValueFactory(cellData -> new SimpleIntegerProperty(cellData.getValue().getQuantity()).asObject());
        reservedColumn.setCellValueFactory(cellData -> new SimpleIntegerProperty(cellData.getValue().getReservedQuantity()).asObject());
        availableColumn.setCellValueFactory(cellData -> new SimpleIntegerProperty(cellData.getValue().getAvailableQuantity()).asObject());
        lastRestockedColumn.setCellValueFactory(cellData ->
                new SimpleStringProperty(cellData.getValue().getLastRestocked() != null ?
                        cellData.getValue().getLastRestocked().format(DateTimeFormatter.ofPattern("dd.MM.yyyy")) : ""));

        inventoryTable.setItems(inventoryList);
    }

    private void loadBooks() {
        try {
            allBooks = repositoryService.getAllBooks();
        } catch (Exception e) {
            showError("Ошибка", "Не удалось загрузить список книг: " + e.getMessage());
        }
    }

    @Override
    protected void handleAdd() {
        // Не используется для склада
    }

    @Override
    protected void handleEdit() {
        // Не используется для склада
    }

    @Override
    protected void handleDelete() {
        // Не используется для склада
    }

    @Override
    protected void handleRefresh() {
        try {
            List<BookRemnant> remnants = repositoryService.getAllBookRemnants();
            inventoryList.clear();
            for (BookRemnant r : remnants) {
                inventoryList.add(new InventoryItem(
                        r.getId(),
                        r.getBook() != null ? r.getBook().getTitle() : "Неизвестно",
                        r.getQuantity(),
                        r.getReservedQuantity(),
                        r.getQuantity() - r.getReservedQuantity(),
                        r.getLastRestocked(),
                        r.getBook() != null ? r.getBook().getId() : null
                ));
            }
            updateStatus("Загружено записей на складе: " + inventoryList.size());
        } catch (Exception e) {
            showError("Ошибка", "Не удалось загрузить склад: " + e.getMessage());
        }
    }

    @Override
    protected void handleSearch() {
        String searchTerm = searchField.getText();
        if (searchTerm == null || searchTerm.trim().isEmpty()) {
            handleRefresh();
            return;
        }

        List<InventoryItem> filtered = inventoryList.stream()
                .filter(i -> i.getBookTitle().toLowerCase().contains(searchTerm.toLowerCase()))
                .toList();
        inventoryList.setAll(filtered);
        updateStatus("Найдено записей: " + filtered.size());
    }

    @FXML
    public void handleRestock() {
        InventoryItem selected = inventoryTable.getSelectionModel().getSelectedItem();
        if (selected == null) {
            showError("Ошибка", "Выберите книгу");
            return;
        }

        TextInputDialog dialog = new TextInputDialog();
        dialog.setTitle("Пополнение склада");
        dialog.setHeaderText("Книга: " + selected.getBookTitle() +
                "\nТекущее количество: " + selected.getQuantity());
        dialog.setContentText("Введите количество для пополнения:");

        dialog.showAndWait().ifPresent(quantity -> {
            try {
                List<Object[]> result = businessService.updateInventoryStock(
                        selected.getBookId(),
                        Integer.parseInt(quantity)
                );
                handleRefresh();
                showInfo("Успех", "Склад пополнен на " + quantity + " шт.");
            } catch (Exception e) {
                showError("Ошибка", e.getMessage());
            }
        });
    }

    @FXML
    public void handleWriteOff() {
        InventoryItem selected = inventoryTable.getSelectionModel().getSelectedItem();
        if (selected == null) {
            showError("Ошибка", "Выберите книгу");
            return;
        }

        TextInputDialog dialog = new TextInputDialog();
        dialog.setTitle("Списание со склада");
        dialog.setHeaderText("Книга: " + selected.getBookTitle() +
                "\nДоступно: " + selected.getAvailableQuantity());
        dialog.setContentText("Введите количество для списания:");

        dialog.showAndWait().ifPresent(quantity -> {
            try {
                int qty = Integer.parseInt(quantity);
                List<Object[]> result = businessService.updateInventoryStock(
                        selected.getBookId(),
                        -qty
                );
                handleRefresh();
                showInfo("Успех", "Списано " + quantity + " шт.");
            } catch (Exception e) {
                showError("Ошибка", e.getMessage());
            }
        });
    }

    @FXML
    public void handleLowStock() {
        TextInputDialog dialog = new TextInputDialog("5");
        dialog.setTitle("Книги с низким остатком");
        dialog.setHeaderText("Показать книги с остатком меньше порога");
        dialog.setContentText("Введите пороговое значение:");

        dialog.showAndWait().ifPresent(threshold -> {
            try {
                List<Object[]> lowStock = businessService.getLowStockBooks(Integer.parseInt(threshold));
                if (lowStock.isEmpty()) {
                    showInfo("Низкий остаток", "Книг с низким остатком не найдено");
                } else {
                    StringBuilder sb = new StringBuilder("Книги с низким остатком:\n\n");
                    for (Object[] item : lowStock) {
                        sb.append(String.format("%s - Доступно: %d шт.\n",
                                item[1], item[5]));
                    }
                    showInfo("Низкий остаток", sb.toString());
                }
            } catch (Exception e) {
                showError("Ошибка", e.getMessage());
            }
        });
    }

    @FXML
    public void handleInventoryValue() {
        try {
            List<Object[]> value = businessService.getInventoryValue();
            if (!value.isEmpty()) {
                Object[] data = value.get(0);
                StringBuilder sb = new StringBuilder();
                sb.append("Стоимость складских запасов\n\n");
                sb.append("Всего книг: ").append(data[0]).append(" шт.\n");
                sb.append("Общая стоимость: ").append(data[1]).append(" руб.\n");
                sb.append("Средняя цена: ").append(data[2]).append(" руб.\n");
                sb.append("Книг с низким остатком: ").append(data[4]).append(" шт.\n");
                showInfo("Стоимость запасов", sb.toString());
            }
        } catch (Exception e) {
            showError("Ошибка", e.getMessage());
        }
    }

    public void updateStatus(String message) {
        if (statusLabel != null) {
            statusLabel.setText(message);
        }
    }
}
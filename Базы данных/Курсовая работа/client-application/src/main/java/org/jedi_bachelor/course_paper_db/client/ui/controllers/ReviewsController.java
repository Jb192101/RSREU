package org.jedi_bachelor.course_paper_db.client.ui.controllers;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.geometry.Insets;
import javafx.scene.control.*;
import javafx.scene.control.cell.PropertyValueFactory;
import javafx.scene.layout.GridPane;
import org.jedi_bachelor.course_paper_db.client.entity.Book;
import org.jedi_bachelor.course_paper_db.client.entity.Client;
import org.jedi_bachelor.course_paper_db.client.entity.Response;
import org.springframework.stereotype.Component;

import java.time.format.DateTimeFormatter;
import java.util.List;

@Component
public class ReviewsController extends BaseController {

    @FXML private TableView<Response> reviewsTable;
    @FXML private TableColumn<Response, Long> idColumn;
    @FXML private TableColumn<Response, String> clientColumn;
    @FXML private TableColumn<Response, String> bookColumn;
    @FXML private TableColumn<Response, Integer> ratingColumn;
    @FXML private TableColumn<Response, String> commentColumn;
    @FXML private TableColumn<Response, String> dateColumn;
    @FXML private TableColumn<Response, Boolean> verifiedColumn;

    private ObservableList<Response> reviewsList = FXCollections.observableArrayList();
    private final DateTimeFormatter dateFormatter = DateTimeFormatter.ofPattern("dd.MM.yyyy HH:mm");

    @FXML
    public void initialize() {
        System.out.println("=== ReviewsController INIT ===");
        setupTableColumns();
        handleRefresh();
    }

    private void setupTableColumns() {
        idColumn.setCellValueFactory(new PropertyValueFactory<>("id"));
        ratingColumn.setCellValueFactory(new PropertyValueFactory<>("rating"));
        commentColumn.setCellValueFactory(new PropertyValueFactory<>("comment"));
        verifiedColumn.setCellValueFactory(new PropertyValueFactory<>("isVerifiedPurchase"));

        clientColumn.setCellValueFactory(cellData -> {
            Response r = cellData.getValue();
            if (r.getClient() != null) {
                return new javafx.beans.property.SimpleStringProperty(
                        r.getClient().getFirstName() + " " + r.getClient().getLastName()
                );
            }
            return new javafx.beans.property.SimpleStringProperty("");
        });

        bookColumn.setCellValueFactory(cellData -> {
            Response r = cellData.getValue();
            if (r.getBook() != null) {
                return new javafx.beans.property.SimpleStringProperty(r.getBook().getTitle());
            }
            return new javafx.beans.property.SimpleStringProperty("");
        });

        dateColumn.setCellValueFactory(cellData -> {
            if (cellData.getValue().getReviewDate() != null) {
                return new javafx.beans.property.SimpleStringProperty(
                        cellData.getValue().getReviewDate().format(dateFormatter)
                );
            }
            return new javafx.beans.property.SimpleStringProperty("");
        });

        reviewsTable.setItems(reviewsList);
    }

    @Override
    protected void handleAdd() {
        try {
            showReviewDialog("Добавление отзыва", null);
        } catch (Exception e) {
            e.printStackTrace();
            String fullStackTrace = getFullStackTrace(e);
            showError("Ошибка", "Не удалось открыть диалог:\n\n" + fullStackTrace);
        }
    }

    @Override
    protected void handleEdit() {
        try {
            Response selected = reviewsTable.getSelectionModel().getSelectedItem();
            if (selected == null) {
                showError("Ошибка", "Выберите отзыв для редактирования");
                return;
            }
            showReviewDialog("Редактирование отзыва", selected);
        } catch (Exception e) {
            e.printStackTrace();
            String fullStackTrace = getFullStackTrace(e);
            showError("Ошибка", "Не удалось открыть диалог:\n\n" + fullStackTrace);
        }
    }

    @Override
    protected void handleDelete() {
        Response selected = reviewsTable.getSelectionModel().getSelectedItem();
        if (selected == null) {
            showError("Ошибка", "Выберите отзыв для удаления");
            return;
        }

        if (showConfirmation("Подтверждение", "Удалить отзыв ID=" + selected.getId() + "?")) {
            try {
                repositoryService.deleteResponse(selected.getId());
                handleRefresh();
                showInfo("Успех", "Отзыв удалён");
            } catch (Exception e) {
                showError("Ошибка", "Не удалось удалить отзыв: " + e.getMessage());
            }
        }
    }

    @Override
    protected void handleRefresh() {
        try {
            List<Response> reviews = repositoryService.getAllResponses();
            System.out.println("Loaded reviews: " + reviews.size());
            reviewsList.setAll(reviews);
            updateStatus("Загружено отзывов: " + reviews.size());
        } catch (Exception e) {
            e.printStackTrace();
            showError("Ошибка", "Не удалось загрузить отзывы: " + e.getMessage());
        }
    }

    @Override
    protected void handleSearch() {
        String searchTerm = searchField.getText();
        if (searchTerm == null || searchTerm.trim().isEmpty()) {
            handleRefresh();
            return;
        }

        List<Response> filtered = reviewsList.stream()
                .filter(r -> (r.getComment() != null && r.getComment().toLowerCase().contains(searchTerm.toLowerCase())) ||
                        (r.getClient() != null && (r.getClient().getFirstName() + " " + r.getClient().getLastName())
                                .toLowerCase().contains(searchTerm.toLowerCase())))
                .toList();
        reviewsList.setAll(filtered);
        updateStatus("Найдено отзывов: " + filtered.size());
    }

    @FXML
    public void handleBookRating() {
        Response selected = reviewsTable.getSelectionModel().getSelectedItem();
        if (selected == null || selected.getBook() == null) {
            showError("Ошибка", "Выберите отзыв с книгой");
            return;
        }

        try {
            List<Object[]> rating = businessService.getBookRating(selected.getBook().getId());
            if (!rating.isEmpty()) {
                Object[] data = rating.get(0);
                StringBuilder sb = new StringBuilder();
                sb.append("📊 РЕЙТИНГ КНИГИ\n");
                sb.append("━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
                sb.append("📚 ").append(selected.getBook().getTitle()).append("\n\n");
                sb.append("⭐ Средний рейтинг: ").append(data[0]).append("\n");
                sb.append("📝 Всего отзывов: ").append(data[1]).append("\n");
                sb.append("✅ Подтверждённых: ").append(data[3]).append("\n");
                sb.append("━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
                sb.append("📊 Распределение оценок:\n");
                sb.append("⭐ 5: ").append(data[8] != null ? data[8] : 0).append("\n");
                sb.append("⭐ 4: ").append(data[7] != null ? data[7] : 0).append("\n");
                sb.append("⭐ 3: ").append(data[6] != null ? data[6] : 0).append("\n");
                sb.append("⭐ 2: ").append(data[5] != null ? data[5] : 0).append("\n");
                sb.append("⭐ 1: ").append(data[4] != null ? data[4] : 0);

                Alert alert = new Alert(Alert.AlertType.INFORMATION);
                alert.setTitle("Рейтинг книги");
                alert.setHeaderText(null);
                alert.setContentText(sb.toString());
                alert.getDialogPane().setPrefWidth(350);
                alert.showAndWait();
            } else {
                showInfo("Рейтинг книги", "Отзывов для этой книги пока нет");
            }
        } catch (Exception e) {
            e.printStackTrace();
            showError("Ошибка", "Не удалось получить рейтинг: " + e.getMessage());
        }
    }

    private void showReviewDialog(String title, Response review) {
        System.out.println("=== showReviewDialog START ===");

        Dialog<Response> dialog = new Dialog<>();
        dialog.setTitle(title);
        dialog.setHeaderText(null);

        ButtonType saveButtonType = new ButtonType("Сохранить", ButtonBar.ButtonData.OK_DONE);
        dialog.getDialogPane().getButtonTypes().addAll(saveButtonType, ButtonType.CANCEL);

        GridPane grid = new GridPane();
        grid.setHgap(10);
        grid.setVgap(10);
        grid.setPadding(new Insets(20, 150, 10, 10));

        ComboBox<Client> clientCombo = new ComboBox<>();
        ComboBox<Book> bookCombo = new ComboBox<>();
        ComboBox<Integer> ratingCombo = new ComboBox<>(FXCollections.observableArrayList(1, 2, 3, 4, 5));
        TextArea commentArea = new TextArea();
        commentArea.setPrefRowCount(3);

        try {
            System.out.println("Загрузка клиентов...");
            // Загружаем клиентов и книг через бизнес-сервис
            List<Client> clients = repositoryService.getAllClients();
            System.out.println("Клиентов: " + clients.size());
            clientCombo.setItems(FXCollections.observableArrayList(clients));

            System.out.println("Загрузка книг...");
            List<Book> books = repositoryService.getAllBooks();
            System.out.println("Книг: " + books.size());
            bookCombo.setItems(FXCollections.observableArrayList(books));
        } catch (Exception e) {
            e.printStackTrace();
            showError("Ошибка", "Не удалось загрузить данные: " + e.getMessage());
            return;
        }

        if (review != null) {
            clientCombo.setValue(review.getClient());
            bookCombo.setValue(review.getBook());
            ratingCombo.setValue(review.getRating());
            commentArea.setText(review.getComment());
        }

        grid.add(new Label("Клиент:*"), 0, 0);
        grid.add(clientCombo, 1, 0);
        grid.add(new Label("Книга:*"), 0, 1);
        grid.add(bookCombo, 1, 1);
        grid.add(new Label("Рейтинг (1-5):*"), 0, 2);
        grid.add(ratingCombo, 1, 2);
        grid.add(new Label("Комментарий:"), 0, 3);
        grid.add(commentArea, 1, 3);

        dialog.getDialogPane().setContent(grid);

        dialog.setResultConverter(dialogButton -> {
            if (dialogButton == saveButtonType) {
                try {
                    if (clientCombo.getValue() == null) {
                        showError("Ошибка", "Выберите клиента");
                        return null;
                    }
                    if (bookCombo.getValue() == null) {
                        showError("Ошибка", "Выберите книгу");
                        return null;
                    }
                    if (ratingCombo.getValue() == null) {
                        showError("Ошибка", "Выберите рейтинг");
                        return null;
                    }

                    if (review == null) {
                        Long reviewId = businessService.addReview(
                                clientCombo.getValue().getId(),
                                bookCombo.getValue().getId(),
                                ratingCombo.getValue(),
                                commentArea.getText()
                        );
                        showInfo("Успех", "Отзыв добавлен (ID: " + reviewId + ")");
                        handleRefresh();
                    } else {
                        review.setClient(clientCombo.getValue());
                        review.setBook(bookCombo.getValue());
                        review.setRating(ratingCombo.getValue());
                        review.setComment(commentArea.getText());
                        repositoryService.updateResponse(review);
                        showInfo("Успех", "Отзыв обновлён");
                        handleRefresh();
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                    showError("Ошибка", "Не удалось сохранить отзыв: " + e.getMessage());
                }
            }
            return null;
        });

        System.out.println("Показ диалога...");
        dialog.showAndWait();
    }

    private String getFullStackTrace(Exception e) {
        StringBuilder sb = new StringBuilder();
        sb.append(e.toString()).append("\n\n");
        for (StackTraceElement element : e.getStackTrace()) {
            sb.append(element.toString()).append("\n");
            if (sb.length() > 5000) break; // Ограничиваем длину
        }
        // Добавляем причину, если есть
        if (e.getCause() != null) {
            sb.append("\n\nCaused by: ").append(e.getCause().toString()).append("\n");
            for (StackTraceElement element : e.getCause().getStackTrace()) {
                sb.append(element.toString()).append("\n");
                if (sb.length() > 7000) break;
            }
        }
        return sb.toString();
    }
}
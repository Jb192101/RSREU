package org.jedi_bachelor.course_paper_db.client.ui.controllers;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.geometry.Insets;
import javafx.scene.control.*;
import javafx.scene.control.cell.PropertyValueFactory;
import javafx.scene.layout.GridPane;
import org.jedi_bachelor.course_paper_db.client.entity.Client;
import org.jedi_bachelor.course_paper_db.client.entity.Order;
import org.jedi_bachelor.course_paper_db.client.entity.OrderPosition;
import org.springframework.stereotype.Component;

import java.math.BigDecimal;
import java.time.format.DateTimeFormatter;
import java.util.List;

@Component
public class OrdersController extends BaseController {

    @FXML private TableView<Order> ordersTable;
    @FXML private TableView<OrderPosition> orderItemsTable;
    @FXML private ComboBox<Client> customerFilter;
    @FXML private ComboBox<String> statusFilter;

    private ObservableList<Order> ordersList = FXCollections.observableArrayList();

    @FXML
    public void initialize() {
        setupFilters();
        setupTableColumns();
        handleRefresh();
    }

    private void setupFilters() {
        statusFilter.setItems(FXCollections.observableArrayList(
                "Все", "ожидает оплаты", "оплачен", "обрабатывается",
                "подтверждён", "отправлен", "доставлен", "отменён", "возвращён"
        ));
        statusFilter.setValue("Все");

        try {
            List<Client> clients = repositoryService.getAllClients();
            customerFilter.setItems(FXCollections.observableArrayList(clients));
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void setupTableColumns() {
        TableColumn<Order, Long> idCol = new TableColumn<>("ID");
        idCol.setCellValueFactory(new PropertyValueFactory<>("id"));

        TableColumn<Order, String> dateCol = new TableColumn<>("Дата");
        dateCol.setCellValueFactory(cellData ->
                new javafx.beans.property.SimpleStringProperty(
                        cellData.getValue().getOrderDate() != null ?
                                cellData.getValue().getOrderDate().format(DateTimeFormatter.ofPattern("dd.MM.yyyy HH:mm")) : ""
                )
        );

        TableColumn<Order, String> customerCol = new TableColumn<>("Клиент");
        customerCol.setCellValueFactory(cellData ->
                new javafx.beans.property.SimpleStringProperty(
                        cellData.getValue().getCustomer() != null ?
                                cellData.getValue().getCustomer().getFirstName() + " " + cellData.getValue().getCustomer().getLastName() : ""
                )
        );

        TableColumn<Order, BigDecimal> totalCol = new TableColumn<>("Сумма");
        totalCol.setCellValueFactory(new PropertyValueFactory<>("totalAmount"));

        TableColumn<Order, BigDecimal> discountCol = new TableColumn<>("Скидка");
        discountCol.setCellValueFactory(new PropertyValueFactory<>("discountAmount"));

        TableColumn<Order, String> statusCol = new TableColumn<>("Статус");
        statusCol.setCellValueFactory(cellData -> {
            if (cellData.getValue().getStatus() != null) {
                return new javafx.beans.property.SimpleStringProperty(
                        getRussianStatus(cellData.getValue().getStatus().name())
                );
            } else if (cellData.getValue().getStatusString() != null) {
                return new javafx.beans.property.SimpleStringProperty(cellData.getValue().getStatusString());
            }
            return new javafx.beans.property.SimpleStringProperty("");
        });

        TableColumn<Order, String> addressCol = new TableColumn<>("Адрес доставки");
        addressCol.setCellValueFactory(new PropertyValueFactory<>("shippingAddress"));

        ordersTable.getColumns().setAll(idCol, dateCol, customerCol, totalCol, discountCol, statusCol, addressCol);
        ordersTable.setItems(ordersList);

        // Выбор заказа для отображения позиций
        ordersTable.getSelectionModel().selectedItemProperty().addListener((obs, old, selected) -> {
            if (selected != null && selected.getOrderPositions() != null) {
                orderItemsTable.setItems(FXCollections.observableArrayList(selected.getOrderPositions()));
            } else {
                orderItemsTable.setItems(FXCollections.observableArrayList());
            }
        });
    }

    private String getRussianStatus(String status) {
        return switch (status) {
            case "PENDING_PAYMENT" -> "ожидает оплаты";
            case "PAID" -> "оплачен";
            case "PROCESSING" -> "обрабатывается";
            case "CONFIRMED" -> "подтверждён";
            case "SHIPPED" -> "отправлен";
            case "DELIVERED" -> "доставлен";
            case "CANCELLED" -> "отменён";
            case "REFUNDED" -> "возвращён";
            case "ON_HOLD" -> "на удержании";
            default -> status;
        };
    }

    @Override
    protected void handleAdd() {
        Dialog<Order> dialog = new Dialog<>();
        dialog.setTitle("Создание заказа");
        dialog.setHeaderText(null);

        ButtonType saveButtonType = new ButtonType("Создать", ButtonBar.ButtonData.OK_DONE);
        dialog.getDialogPane().getButtonTypes().addAll(saveButtonType, ButtonType.CANCEL);

        GridPane grid = new GridPane();
        grid.setHgap(10);
        grid.setVgap(10);
        grid.setPadding(new Insets(20, 150, 10, 10));

        ComboBox<Client> clientCombo = new ComboBox<>();
        clientCombo.setPromptText("Выберите клиента");
        TextArea addressArea = new TextArea();
        addressArea.setPromptText("Введите адрес доставки");
        addressArea.setPrefRowCount(2);
        TextField promoField = new TextField();
        promoField.setPromptText("Промокод (опционально)");

        try {
            clientCombo.setItems(FXCollections.observableArrayList(repositoryService.getAllClients()));
        } catch (Exception e) {
            e.printStackTrace();
        }

        grid.add(new Label("Клиент:*"), 0, 0);
        grid.add(clientCombo, 1, 0);
        grid.add(new Label("Адрес доставки:*"), 0, 1);
        grid.add(addressArea, 1, 1);
        grid.add(new Label("Промокод:"), 0, 2);
        grid.add(promoField, 1, 2);

        Label infoLabel = new Label("* — обязательные поля");
        infoLabel.setStyle("-fx-text-fill: #7f8c8d; -fx-font-size: 11px;");
        grid.add(infoLabel, 0, 3, 2, 1);

        dialog.getDialogPane().setContent(grid);

        Button saveButton = (Button) dialog.getDialogPane().lookupButton(saveButtonType);
        saveButton.addEventFilter(javafx.event.ActionEvent.ACTION, event -> {
            if (clientCombo.getValue() == null) {
                showError("Ошибка", "Выберите клиента");
                event.consume();
                return;
            }
            if (addressArea.getText().trim().isEmpty()) {
                showError("Ошибка", "Введите адрес доставки");
                event.consume();
            }
        });

        dialog.setResultConverter(dialogButton -> {
            if (dialogButton == saveButtonType && clientCombo.getValue() != null) {
                try {
                    Long orderId = businessService.createOrder(
                            clientCombo.getValue().getId(),
                            addressArea.getText().trim(),
                            promoField.getText().trim().isEmpty() ? null : promoField.getText().trim()
                    );
                    if (orderId != null && orderId > 0) {
                        showInfo("Успех", "Заказ #" + orderId + " создан");
                        handleRefresh();
                    } else {
                        showError("Ошибка", "Не удалось создать заказ");
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                    showError("Ошибка", "Не удалось создать заказ: " + e.getMessage());
                }
            }
            return null;
        });

        dialog.showAndWait();
    }

    @Override
    protected void handleEdit() {
        Order selected = ordersTable.getSelectionModel().getSelectedItem();
        if (selected == null) {
            showError("Ошибка", "Выберите заказ для редактирования");
            return;
        }

        Dialog<Order> dialog = new Dialog<>();
        dialog.setTitle("Редактирование заказа #" + selected.getId());
        dialog.setHeaderText(null);

        ButtonType saveButtonType = new ButtonType("Сохранить", ButtonBar.ButtonData.OK_DONE);
        dialog.getDialogPane().getButtonTypes().addAll(saveButtonType, ButtonType.CANCEL);

        GridPane grid = new GridPane();
        grid.setHgap(10);
        grid.setVgap(10);
        grid.setPadding(new Insets(20, 150, 10, 10));

        TextArea addressArea = new TextArea();
        addressArea.setText(selected.getShippingAddress());
        addressArea.setPrefRowCount(3);

        grid.add(new Label("Адрес доставки:"), 0, 0);
        grid.add(addressArea, 1, 0);

        dialog.getDialogPane().setContent(grid);

        dialog.setResultConverter(dialogButton -> {
            if (dialogButton == saveButtonType) {
                selected.setShippingAddress(addressArea.getText().trim());
                return selected;
            }
            return null;
        });

        dialog.showAndWait().ifPresent(result -> {
            try {
                repositoryService.updateOrder(result);
                handleRefresh();
                showInfo("Успех", "Заказ обновлён");
            } catch (Exception e) {
                e.printStackTrace();
                showError("Ошибка", "Не удалось обновить заказ: " + e.getMessage());
            }
        });
    }

    @Override
    protected void handleDelete() {
        Order selected = ordersTable.getSelectionModel().getSelectedItem();
        if (selected == null) {
            showError("Ошибка", "Выберите заказ для отмены");
            return;
        }

        if (showConfirmation("Подтверждение", "Отменить заказ #" + selected.getId() + "?")) {
            try {
                businessService.cancelOrder(selected.getId());
                handleRefresh();
                showInfo("Успех", "Заказ #" + selected.getId() + " отменён");
            } catch (Exception e) {
                e.printStackTrace();
                showError("Ошибка", "Не удалось отменить заказ: " + e.getMessage());
            }
        }
    }

    @Override
    protected void handleRefresh() {
        try {
            List<Order> orders = repositoryService.getAllOrders();
            ordersList.setAll(orders);
            updateStatus("Загружено заказов: " + orders.size());
        } catch (Exception e) {
            e.printStackTrace();
            showError("Ошибка", "Не удалось загрузить заказы: " + e.getMessage());
        }
    }

    @Override
    protected void handleSearch() {
        Client selectedClient = customerFilter.getValue();
        String selectedStatus = statusFilter.getValue();

        try {
            // Получаем все заказы заново, чтобы не фильтровать уже отфильтрованные
            List<Order> allOrders = repositoryService.getAllOrders();

            List<Order> filtered = allOrders.stream()
                    .filter(o -> selectedClient == null ||
                            (o.getCustomer() != null && o.getCustomer().getId().equals(selectedClient.getId())))
                    .filter(o -> selectedStatus == null || selectedStatus.equals("Все") ||
                            (o.getStatus() != null && getRussianStatus(o.getStatus().name()).equals(selectedStatus)) ||
                            (o.getStatusString() != null && o.getStatusString().equals(selectedStatus)))
                    .toList();

            ordersList.setAll(filtered);
            updateStatus("Найдено заказов: " + filtered.size());
        } catch (Exception e) {
            e.printStackTrace();
            showError("Ошибка", "Не удалось выполнить поиск: " + e.getMessage());
        }
    }

    @FXML
    public void handleChangeStatus() {
        Order selected = ordersTable.getSelectionModel().getSelectedItem();
        if (selected == null) {
            showError("Ошибка", "Выберите заказ для изменения статуса");
            return;
        }

        String currentStatus = selected.getStatus() != null ?
                getRussianStatus(selected.getStatus().name()) :
                selected.getStatusString();

        ChoiceDialog<String> dialog = new ChoiceDialog<>(
                currentStatus,
                "ожидает оплаты", "оплачен", "обрабатывается", "подтверждён", "отправлен", "доставлен", "отменён", "возвращён"
        );
        dialog.setTitle("Изменение статуса заказа");
        dialog.setHeaderText("Заказ #" + selected.getId());
        dialog.setContentText("Выберите новый статус:");

        dialog.showAndWait().ifPresent(status -> {
            try {
                String englishStatus = switch (status) {
                    case "ожидает оплаты" -> "PENDING_PAYMENT";
                    case "оплачен" -> "PAID";
                    case "обрабатывается" -> "PROCESSING";
                    case "подтверждён" -> "CONFIRMED";
                    case "отправлен" -> "SHIPPED";
                    case "доставлен" -> "DELIVERED";
                    case "отменён" -> "CANCELLED";
                    case "возвращён" -> "REFUNDED";
                    default -> selected.getStatus() != null ? selected.getStatus().name() : "PENDING_PAYMENT";
                };
                businessService.changeOrderStatus(selected.getId(), englishStatus);
                handleRefresh();
                showInfo("Успех", "Статус заказа #" + selected.getId() + " изменён на \"" + status + "\"");
            } catch (Exception e) {
                e.printStackTrace();
                showError("Ошибка", "Не удалось изменить статус: " + e.getMessage());
            }
        });
    }

    @FXML
    public void handleApplyPromoCode() {
        Order selected = ordersTable.getSelectionModel().getSelectedItem();
        if (selected == null) {
            showError("Ошибка", "Выберите заказ для применения промокода");
            return;
        }

        TextInputDialog dialog = new TextInputDialog();
        dialog.setTitle("Применение промокода");
        dialog.setHeaderText("Заказ #" + selected.getId() + "\nСумма: " + selected.getTotalAmount() + " руб.");
        dialog.setContentText("Введите код промокода:");

        dialog.showAndWait().ifPresent(promoCode -> {
            if (promoCode.trim().isEmpty()) {
                showError("Ошибка", "Введите код промокода");
                return;
            }
            try {
                businessService.applyPromocodeToOrder(selected.getId(), promoCode.trim());
                handleRefresh();
                showInfo("Успех", "Промокод \"" + promoCode + "\" применён к заказу #" + selected.getId());
            } catch (Exception e) {
                e.printStackTrace();
                showError("Ошибка", "Не удалось применить промокод: " + e.getMessage());
            }
        });
    }

    @FXML
    public void handleTrackOrder() {
        Order selected = ordersTable.getSelectionModel().getSelectedItem();
        if (selected == null) {
            showError("Ошибка", "Выберите заказ для отслеживания");
            return;
        }

        try {
            List<Object[]> tracking = businessService.trackOrder(selected.getId());
            if (!tracking.isEmpty()) {
                Object[] info = tracking.get(0);
                StringBuilder sb = new StringBuilder();
                sb.append("📦 ОТСЛЕЖИВАНИЕ ЗАКАЗА #").append(info[0]).append("\n\n");
                sb.append("━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
                sb.append("📋 Статус заказа: ").append(info[1]).append("\n");
                sb.append("📅 Дата создания: ").append(info[2]).append("\n");
                sb.append("📍 Адрес доставки: ").append(info[3]).append("\n");
                sb.append("━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
                sb.append("🚚 Перевозчик: ").append(info[4] != null ? info[4] : "Не назначен").append("\n");
                sb.append("🔢 Трек-номер: ").append(info[5] != null ? info[5] : "Не назначен").append("\n");
                sb.append("📊 Статус доставки: ").append(info[6] != null ? info[6] : "Не создана").append("\n");
                if (info[7] != null) {
                    sb.append("📤 Дата отправки: ").append(info[7]).append("\n");
                }
                if (info[8] != null) {
                    sb.append("📥 Дата доставки: ").append(info[8]).append("\n");
                }
                sb.append("━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
                sb.append("📌 ").append(info[9] != null ? info[9] : "Ожидание обработки");

                showInfo("Отслеживание заказа", sb.toString());
            } else {
                showInfo("Отслеживание заказа", "Информация по заказу #" + selected.getId() + " не найдена");
            }
        } catch (Exception e) {
            e.printStackTrace();
            showError("Ошибка", "Не удалось выполнить отслеживание: " + e.getMessage());
        }
    }

    public void updateStatus(String message) {
        if (statusLabel != null) {
            statusLabel.setText(message);
        }
    }
}
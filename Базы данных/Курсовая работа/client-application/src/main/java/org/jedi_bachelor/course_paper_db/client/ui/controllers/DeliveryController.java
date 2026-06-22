package org.jedi_bachelor.course_paper_db.client.ui.controllers;

import javafx.beans.property.SimpleObjectProperty;
import javafx.beans.property.SimpleStringProperty;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.geometry.Insets;
import javafx.scene.control.*;
import javafx.scene.layout.GridPane;
import org.jedi_bachelor.course_paper_db.client.entity.Delivery;
import org.jedi_bachelor.course_paper_db.client.entity.Order;
import org.jedi_bachelor.course_paper_db.client.entity.enums.DeliveryStatus;
import org.springframework.stereotype.Component;

import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.List;

@Component
public class DeliveryController extends BaseController {

    @FXML private TableView<DeliveryItem> deliveryTable;
    @FXML private TableColumn<DeliveryItem, Long> idColumn;
    @FXML private TableColumn<DeliveryItem, Long> orderColumn;
    @FXML private TableColumn<DeliveryItem, String> carrierColumn;
    @FXML private TableColumn<DeliveryItem, String> trackingColumn;
    @FXML private TableColumn<DeliveryItem, String> statusColumn;
    @FXML private TableColumn<DeliveryItem, String> shippedDateColumn;
    @FXML private TableColumn<DeliveryItem, String> deliveredDateColumn;

    private ObservableList<DeliveryItem> deliveryList = FXCollections.observableArrayList();
    private List<Order> allOrders;

    public static class DeliveryItem {
        private final Long id;
        private final Long orderId;
        private final String carrier;
        private final String trackingNumber;
        private final String status;
        private final LocalDateTime shippedDate;
        private final LocalDateTime deliveredDate;

        public DeliveryItem(Long id, Long orderId, String carrier, String trackingNumber,
                            String status, LocalDateTime shippedDate, LocalDateTime deliveredDate) {
            this.id = id;
            this.orderId = orderId;
            this.carrier = carrier;
            this.trackingNumber = trackingNumber;
            this.status = status;
            this.shippedDate = shippedDate;
            this.deliveredDate = deliveredDate;
        }

        public Long getId() { return id; }
        public Long getOrderId() { return orderId; }
        public String getCarrier() { return carrier; }
        public String getTrackingNumber() { return trackingNumber; }
        public String getStatus() { return status; }
        public LocalDateTime getShippedDate() { return shippedDate; }
        public LocalDateTime getDeliveredDate() { return deliveredDate; }
    }

    @FXML
    public void initialize() {
        setupTableColumns();
        loadOrders();
        handleRefresh();
    }

    private void setupTableColumns() {
        idColumn.setCellValueFactory(cellData -> new SimpleObjectProperty<>(cellData.getValue().getId()));
        orderColumn.setCellValueFactory(cellData -> new SimpleObjectProperty<>(cellData.getValue().getOrderId()));
        carrierColumn.setCellValueFactory(cellData -> new SimpleStringProperty(cellData.getValue().getCarrier()));
        trackingColumn.setCellValueFactory(cellData -> new SimpleStringProperty(cellData.getValue().getTrackingNumber()));
        statusColumn.setCellValueFactory(cellData -> new SimpleStringProperty(cellData.getValue().getStatus()));

        shippedDateColumn.setCellValueFactory(cellData -> new SimpleStringProperty(
                cellData.getValue().getShippedDate() != null ?
                        cellData.getValue().getShippedDate().format(DateTimeFormatter.ofPattern("dd.MM.yyyy HH:mm")) : ""
        ));

        deliveredDateColumn.setCellValueFactory(cellData -> new SimpleStringProperty(
                cellData.getValue().getDeliveredDate() != null ?
                        cellData.getValue().getDeliveredDate().format(DateTimeFormatter.ofPattern("dd.MM.yyyy HH:mm")) : ""
        ));

        deliveryTable.setItems(deliveryList);
    }

    private void loadOrders() {
        try {
            allOrders = repositoryService.getAllOrders();
        } catch (Exception e) {
            showError("Ошибка", "Не удалось загрузить заказы: " + e.getMessage());
        }
    }

    @Override
    protected void handleAdd() {
        showDeliveryDialog("Создание отправления", null);
    }

    @Override
    protected void handleEdit() {
        DeliveryItem selected = deliveryTable.getSelectionModel().getSelectedItem();
        if (selected == null) {
            showError("Ошибка", "Выберите отправление для редактирования");
            return;
        }

        try {
            Delivery delivery = repositoryService.getDeliveryById(selected.getId()).orElse(null);
            if (delivery != null) {
                showDeliveryDialog("Редактирование отправления", delivery);
            } else {
                showError("Ошибка", "Отправление не найдено в базе данных");
            }
        } catch (Exception e) {
            showError("Ошибка", "Не удалось загрузить отправление: " + e.getMessage());
            e.printStackTrace();
        }
    }

    @Override
    protected void handleDelete() {
        DeliveryItem selected = deliveryTable.getSelectionModel().getSelectedItem();
        if (selected == null) {
            showError("Ошибка", "Выберите отправление для удаления");
            return;
        }

        if (showConfirmation("Подтверждение", "Удалить отправление #" + selected.getId() + "?")) {
            try {
                repositoryService.deleteDelivery(selected.getId());
                handleRefresh();
                showInfo("Успех", "Отправление удалено");
            } catch (Exception e) {
                showError("Ошибка", "Не удалось удалить отправление: " + e.getMessage());
                e.printStackTrace();
            }
        }
    }

    @Override
    protected void handleRefresh() {
        try {
            List<Delivery> deliveries = repositoryService.getAllDeliveries();
            deliveryList.clear();
            for (Delivery d : deliveries) {
                // Определяем статус для отображения
                String statusDisplay = "";
                if (d.getStatus() != null) {
                    statusDisplay = getRussianStatus(d.getStatus().name());
                } else if (d.getStatusString() != null) {
                    // Если status не инициализирован, используем statusString
                    statusDisplay = d.getStatusString();
                }

                deliveryList.add(new DeliveryItem(
                        d.getId(),
                        d.getOrder() != null ? d.getOrder().getId() : null,
                        d.getCarrier(),
                        d.getTrackingNumber(),
                        statusDisplay,
                        d.getShippedDate(),
                        d.getDeliveredDate()
                ));
            }
            updateStatus("Загружено отправлений: " + deliveryList.size());
        } catch (Exception e) {
            e.printStackTrace();
            showError("Ошибка", "Не удалось загрузить отправления: " + e.getMessage());
        }
    }

    @Override
    protected void handleSearch() {
        String searchTerm = searchField.getText();
        if (searchTerm == null || searchTerm.trim().isEmpty()) {
            handleRefresh();
            return;
        }

        List<DeliveryItem> filtered = deliveryList.stream()
                .filter(d -> (d.getTrackingNumber() != null &&
                        d.getTrackingNumber().toLowerCase().contains(searchTerm.toLowerCase())) ||
                        (d.getCarrier() != null &&
                                d.getCarrier().toLowerCase().contains(searchTerm.toLowerCase())))
                .toList();
        deliveryList.setAll(filtered);
        updateStatus("Найдено отправлений: " + filtered.size());
    }

    @FXML
    public void handleUpdateStatus() {
        DeliveryItem selected = deliveryTable.getSelectionModel().getSelectedItem();
        if (selected == null) {
            showError("Ошибка", "Выберите отправление для изменения статуса");
            return;
        }

        // Создаем диалог выбора статуса
        ChoiceDialog<String> dialog = new ChoiceDialog<>(
                selected.getStatus(),
                "Создано", "Отправлено", "В пути", "У курьера", "Доставлено", "Неудачно", "Возвращено", "Отменено"
        );
        dialog.setTitle("Изменение статуса доставки");
        dialog.setHeaderText("Отправление #" + selected.getId());
        dialog.setContentText("Выберите новый статус:");

        dialog.showAndWait().ifPresent(status -> {
            try {
                // Преобразуем русский статус в английский для enum
                String englishStatus = switch (status) {
                    case "Создано" -> "PENDING";
                    case "Обрабатывается" -> "PROCESSING";
                    case "Отправлено" -> "SHIPPED";
                    case "В пути" -> "IN_TRANSIT";
                    case "У курьера" -> "OUT_FOR_DELIVERY";
                    case "Доставлено" -> "DELIVERED";
                    case "Неудачно" -> "FAILED";
                    case "Возвращено" -> "RETURNED";
                    case "Отменено" -> "CANCELLED";
                    default -> "PENDING";
                };

                // Если статус "Доставлено" — устанавливаем дату доставки
                LocalDateTime deliveredDate = status.equals("Доставлено") ? LocalDateTime.now() : null;

                businessService.updateShipmentStatus(selected.getId(), englishStatus, deliveredDate);
                handleRefresh();
                showInfo("Успех", "Статус доставки изменён на \"" + status + "\"");
            } catch (Exception e) {
                showError("Ошибка", "Не удалось обновить статус: " + e.getMessage());
                e.printStackTrace();
            }
        });
    }

    /**
     * Диалог для создания/редактирования отправления
     */
    private void showDeliveryDialog(String title, Delivery delivery) {
        Dialog<Delivery> dialog = new Dialog<>();
        dialog.setTitle(title);
        dialog.setHeaderText(null);

        ButtonType saveButtonType = new ButtonType("Сохранить", ButtonBar.ButtonData.OK_DONE);
        dialog.getDialogPane().getButtonTypes().addAll(saveButtonType, ButtonType.CANCEL);

        GridPane grid = new GridPane();
        grid.setHgap(10);
        grid.setVgap(10);
        grid.setPadding(new Insets(20, 150, 10, 10));

        ComboBox<Order> orderCombo = new ComboBox<>();
        orderCombo.setPromptText("Выберите заказ");

        TextField carrierField = new TextField();
        carrierField.setPromptText("Название перевозчика");

        TextField trackingField = new TextField();
        trackingField.setPromptText("Трек-номер (опционально)");

        // Загружаем заказы для выбора
        loadOrders();
        // Показываем только заказы, у которых нет доставки
        List<Order> availableOrders = allOrders.stream()
                .filter(o -> delivery == null || !delivery.getOrder().getId().equals(o.getId()))
                .filter(o -> {
                    // Проверяем, есть ли уже доставка для этого заказа
                    try {
                        List<Delivery> existingDeliveries = repositoryService.getAllDeliveries();
                        return existingDeliveries.stream().noneMatch(d ->
                                d.getOrder() != null && d.getOrder().getId().equals(o.getId())
                        );
                    } catch (Exception e) {
                        return true;
                    }
                })
                .toList();

        // Если редактируем, добавляем текущий заказ в список
        if (delivery != null && delivery.getOrder() != null) {
            if (!availableOrders.contains(delivery.getOrder())) {
                orderCombo.getItems().add(delivery.getOrder());
            }
            orderCombo.setValue(delivery.getOrder());
            carrierField.setText(delivery.getCarrier());
            trackingField.setText(delivery.getTrackingNumber());
        }

        orderCombo.setItems(FXCollections.observableArrayList(availableOrders));

        grid.add(new Label("Заказ:*"), 0, 0);
        grid.add(orderCombo, 1, 0);
        grid.add(new Label("Перевозчик:*"), 0, 1);
        grid.add(carrierField, 1, 1);
        grid.add(new Label("Трек-номер:"), 0, 2);
        grid.add(trackingField, 1, 2);

        // Информационная подсказка
        Label infoLabel = new Label("* — обязательные поля");
        infoLabel.setStyle("-fx-text-fill: #7f8c8d; -fx-font-size: 11px;");
        grid.add(infoLabel, 0, 3, 2, 1);

        dialog.getDialogPane().setContent(grid);

        // Валидация перед сохранением
        Button saveButton = (Button) dialog.getDialogPane().lookupButton(saveButtonType);
        saveButton.addEventFilter(javafx.event.ActionEvent.ACTION, event -> {
            if (orderCombo.getValue() == null) {
                showError("Ошибка", "Выберите заказ");
                event.consume();
                return;
            }
            if (carrierField.getText().trim().isEmpty()) {
                showError("Ошибка", "Введите название перевозчика");
                event.consume();
            }
        });

        dialog.setResultConverter(dialogButton -> {
            if (dialogButton == saveButtonType && orderCombo.getValue() != null) {
                try {
                    if (delivery == null) {
                        // Создание нового отправления через хранимую процедуру
                        Long shipmentId = businessService.createShipment(
                                orderCombo.getValue().getId(),
                                carrierField.getText().trim(),
                                trackingField.getText().trim().isEmpty() ? null : trackingField.getText().trim()
                        );
                        showInfo("Успех", "Отправление создано (ID: " + shipmentId + ")");
                        handleRefresh();
                    } else {
                        // Редактирование существующего отправления
                        delivery.setOrder(orderCombo.getValue());
                        delivery.setCarrier(carrierField.getText().trim());
                        delivery.setTrackingNumber(trackingField.getText().trim().isEmpty() ? null : trackingField.getText().trim());
                        // Сохраняем статус в строку для БД
                        if (delivery.getStatus() != null) {
                            delivery.setStatusString(switch (delivery.getStatus()) {
                                case PENDING -> "создано";
                                case PROCESSING -> "обрабатывается";
                                case SHIPPED -> "отправлено";
                                case IN_TRANSIT -> "в пути";
                                case OUT_FOR_DELIVERY -> "у курьера";
                                case DELIVERED -> "доставлено";
                                case FAILED -> "неудачно";
                                case RETURNED -> "возвращено";
                                case CANCELLED -> "отменено";
                            });
                        }
                        repositoryService.updateDelivery(delivery);
                        showInfo("Успех", "Отправление обновлено");
                        handleRefresh();
                    }
                } catch (Exception e) {
                    showError("Ошибка", "Не удалось сохранить отправление: " + e.getMessage());
                    e.printStackTrace();
                }
            }
            return null;
        });

        dialog.showAndWait();
    }

    private String getRussianStatus(String status) {
        return switch (status) {
            case "PENDING" -> "Создано";
            case "PROCESSING" -> "Обрабатывается";
            case "SHIPPED" -> "Отправлено";
            case "IN_TRANSIT" -> "В пути";
            case "OUT_FOR_DELIVERY" -> "У курьера";
            case "DELIVERED" -> "Доставлено";
            case "FAILED" -> "Неудачно";
            case "RETURNED" -> "Возвращено";
            case "CANCELLED" -> "Отменено";
            default -> status;
        };
    }

    public void updateStatus(String message) {
        if (statusLabel != null) {
            statusLabel.setText(message);
        }
    }
}
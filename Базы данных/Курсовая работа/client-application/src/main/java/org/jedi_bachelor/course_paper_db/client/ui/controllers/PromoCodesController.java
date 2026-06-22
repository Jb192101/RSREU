package org.jedi_bachelor.course_paper_db.client.ui.controllers;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.scene.control.TableCell;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TableView;
import javafx.scene.control.TextInputDialog;
import javafx.scene.control.cell.PropertyValueFactory;
import org.jedi_bachelor.course_paper_db.client.entity.PromoCode;
import org.springframework.stereotype.Component;

import java.math.BigDecimal;
import java.time.LocalDate;
import java.time.format.DateTimeFormatter;
import java.util.List;
import javafx.geometry.Insets;
import javafx.scene.control.*;
import javafx.scene.layout.GridPane;
import org.jedi_bachelor.course_paper_db.client.entity.enums.DiscountType;

@Component
public class PromoCodesController extends BaseController {

    @FXML private TableView<PromoCode> promoCodesTable;

    @FXML private TableColumn<PromoCode, Long> idColumn;
    @FXML private TableColumn<PromoCode, String> codeColumn;
    @FXML private TableColumn<PromoCode, String> discountTypeColumn;
    @FXML private TableColumn<PromoCode, BigDecimal> discountValueColumn;
    @FXML private TableColumn<PromoCode, LocalDate> validFromColumn;
    @FXML private TableColumn<PromoCode, LocalDate> validToColumn;
    @FXML private TableColumn<PromoCode, Integer> usedCountColumn;
    @FXML private TableColumn<PromoCode, Integer> maxUsesColumn;
    @FXML private TableColumn<PromoCode, BigDecimal> minOrderColumn;

    private ObservableList<PromoCode> promoCodesList = FXCollections.observableArrayList();
    private final DateTimeFormatter dateFormatter = DateTimeFormatter.ofPattern("dd.MM.yyyy");

    @FXML
    public void initialize() {
        setupTableColumns();
        handleRefresh();
    }

    private void setupTableColumns() {
        idColumn.setCellValueFactory(new PropertyValueFactory<>("id"));
        codeColumn.setCellValueFactory(new PropertyValueFactory<>("code"));

        discountTypeColumn.setCellValueFactory(cellData -> {
            PromoCode pc = cellData.getValue();
            if (pc.getDiscountType() != null) {
                String type = pc.getDiscountType().name();
                return new javafx.beans.property.SimpleStringProperty(type.equals("PERCENT") ? "Процент" : "Фиксированная");
            }
            return new javafx.beans.property.SimpleStringProperty("");
        });

        discountValueColumn.setCellValueFactory(new PropertyValueFactory<>("discountValue"));

        validFromColumn.setCellValueFactory(cellData -> {
            LocalDate date = cellData.getValue().getValidFrom();
            return new javafx.beans.property.SimpleObjectProperty<>(date);
        });

        validToColumn.setCellValueFactory(cellData -> {
            LocalDate date = cellData.getValue().getValidTo();
            return new javafx.beans.property.SimpleObjectProperty<>(date);
        });

        validFromColumn.setCellFactory(column -> new TableCell<PromoCode, LocalDate>() {
            @Override
            protected void updateItem(LocalDate item, boolean empty) {
                super.updateItem(item, empty);
                if (empty || item == null) {
                    setText(null);
                } else {
                    setText(item.format(dateFormatter));
                }
            }
        });

        validToColumn.setCellFactory(column -> new TableCell<PromoCode, LocalDate>() {
            @Override
            protected void updateItem(LocalDate item, boolean empty) {
                super.updateItem(item, empty);
                if (empty || item == null) {
                    setText(null);
                } else {
                    setText(item.format(dateFormatter));
                }
            }
        });

        usedCountColumn.setCellValueFactory(new PropertyValueFactory<>("usedCount"));
        maxUsesColumn.setCellValueFactory(new PropertyValueFactory<>("maxUses"));
        minOrderColumn.setCellValueFactory(new PropertyValueFactory<>("minOrderAmount"));

        promoCodesTable.setItems(promoCodesList);
    }

    @Override
    protected void handleAdd() {
        showPromoCodeDialog("Добавление промокода", null);
    }

    @Override
    protected void handleEdit() {
        PromoCode selected = promoCodesTable.getSelectionModel().getSelectedItem();
        if (selected == null) {
            showError("Ошибка", "Выберите промокод для редактирования");
            return;
        }
        showPromoCodeDialog("Редактирование промокода", selected);
    }

    @Override
    protected void handleDelete() {
        PromoCode selected = promoCodesTable.getSelectionModel().getSelectedItem();
        if (selected == null) {
            showError("Ошибка", "Выберите промокод для удаления");
            return;
        }

        if (showConfirmation("Подтверждение", "Удалить промокод \"" + selected.getCode() + "\"?")) {
            try {
                repositoryService.deletePromoCode(selected.getId());
                handleRefresh();
                showInfo("Успех", "Промокод удалён");
            } catch (Exception e) {
                showError("Ошибка", "Не удалось удалить промокод: " + e.getMessage());
            }
        }
    }

    @Override
    protected void handleRefresh() {
        try {
            List<PromoCode> promoCodes = repositoryService.getAllPromoCodes();
            promoCodesList.setAll(promoCodes);
            updateStatus("Загружено промокодов: " + promoCodes.size());
        } catch (Exception e) {
            e.printStackTrace();
            showError("Ошибка", "Не удалось загрузить промокоды: " + e.getMessage());
        }
    }

    @Override
    protected void handleSearch() {
        String searchTerm = searchField.getText();
        if (searchTerm == null || searchTerm.trim().isEmpty()) {
            handleRefresh();
            return;
        }

        List<PromoCode> filtered = promoCodesList.stream()
                .filter(p -> p.getCode().toLowerCase().contains(searchTerm.toLowerCase()))
                .toList();
        promoCodesList.setAll(filtered);
        updateStatus("Найдено промокодов: " + filtered.size());
    }

    @FXML
    public void handleValidate() {
        TextInputDialog dialog = new TextInputDialog();
        dialog.setTitle("Проверка промокода");
        dialog.setHeaderText("Проверка валидности промокода");
        dialog.setContentText("Введите код промокода:");

        dialog.showAndWait().ifPresent(code -> {
            TextInputDialog amountDialog = new TextInputDialog("1000");
            amountDialog.setTitle("Сумма заказа");
            amountDialog.setHeaderText("Промокод: " + code);
            amountDialog.setContentText("Введите сумму заказа:");

            amountDialog.showAndWait().ifPresent(amount -> {
                try {
                    List<Object[]> result = businessService.validatePromocode(code, new BigDecimal(amount));
                    if (!result.isEmpty()) {
                        Object[] data = result.get(0);
                        boolean isValid = (Boolean) data[0];
                        if (isValid) {
                            showInfo("Промокод валиден",
                                    "Тип скидки: " + data[1] + "\n" +
                                            "Значение: " + data[2] + "\n" +
                                            "Скидка: " + data[3] + " руб.");
                        } else {
                            showError("Промокод недействителен", (String) data[4]);
                        }
                    }
                } catch (Exception e) {
                    showError("Ошибка", e.getMessage());
                }
            });
        });
    }

    private void showPromoCodeDialog(String title, PromoCode promoCode) {
        Dialog<PromoCode> dialog = new Dialog<>();
        dialog.setTitle(title);
        dialog.setHeaderText(null);

        ButtonType saveButtonType = new ButtonType("Сохранить", ButtonBar.ButtonData.OK_DONE);
        dialog.getDialogPane().getButtonTypes().addAll(saveButtonType, ButtonType.CANCEL);

        GridPane grid = new GridPane();
        grid.setHgap(10);
        grid.setVgap(10);
        grid.setPadding(new Insets(20, 150, 10, 10));

        TextField codeField = new TextField();
        codeField.setPromptText("Код промокода");

        ComboBox<DiscountType> typeCombo = new ComboBox<>(FXCollections.observableArrayList(DiscountType.values()));
        typeCombo.setPromptText("Тип скидки");

        TextField valueField = new TextField();
        valueField.setPromptText("Значение скидки");

        DatePicker validFromPicker = new DatePicker();
        validFromPicker.setPromptText("Дата начала");

        DatePicker validToPicker = new DatePicker();
        validToPicker.setPromptText("Дата окончания");

        TextField maxUsesField = new TextField();
        maxUsesField.setPromptText("Макс. использований (опционально)");

        TextField minOrderField = new TextField();
        minOrderField.setPromptText("Мин. сумма заказа");

        if (promoCode != null) {
            codeField.setText(promoCode.getCode());
            typeCombo.setValue(promoCode.getDiscountType());
            valueField.setText(promoCode.getDiscountValue().toString());
            validFromPicker.setValue(promoCode.getValidFrom());
            validToPicker.setValue(promoCode.getValidTo());
            maxUsesField.setText(promoCode.getMaxUses() != null ? promoCode.getMaxUses().toString() : "");
            minOrderField.setText(promoCode.getMinOrderAmount().toString());
        } else {
            validFromPicker.setValue(LocalDate.now());
            validToPicker.setValue(LocalDate.now().plusMonths(1));
            minOrderField.setText("0");
        }

        grid.add(new Label("Код:*"), 0, 0);
        grid.add(codeField, 1, 0);
        grid.add(new Label("Тип скидки:*"), 0, 1);
        grid.add(typeCombo, 1, 1);
        grid.add(new Label("Значение:*"), 0, 2);
        grid.add(valueField, 1, 2);
        grid.add(new Label("Действует с:*"), 0, 3);
        grid.add(validFromPicker, 1, 3);
        grid.add(new Label("Действует до:*"), 0, 4);
        grid.add(validToPicker, 1, 4);
        grid.add(new Label("Макс. использований:"), 0, 5);
        grid.add(maxUsesField, 1, 5);
        grid.add(new Label("Мин. сумма заказа:"), 0, 6);
        grid.add(minOrderField, 1, 6);

        Label infoLabel = new Label("* — обязательные поля");
        infoLabel.setStyle("-fx-text-fill: #7f8c8d; -fx-font-size: 11px;");
        grid.add(infoLabel, 0, 7, 2, 1);

        dialog.getDialogPane().setContent(grid);

        Button saveButton = (Button) dialog.getDialogPane().lookupButton(saveButtonType);
        saveButton.addEventFilter(javafx.event.ActionEvent.ACTION, event -> {
            if (codeField.getText().trim().isEmpty()) {
                showError("Ошибка", "Введите код промокода");
                event.consume();
                return;
            }
            if (typeCombo.getValue() == null) {
                showError("Ошибка", "Выберите тип скидки");
                event.consume();
                return;
            }
            if (valueField.getText().trim().isEmpty()) {
                showError("Ошибка", "Введите значение скидки");
                event.consume();
                return;
            }
            if (validFromPicker.getValue() == null) {
                showError("Ошибка", "Выберите дату начала действия");
                event.consume();
                return;
            }
            if (validToPicker.getValue() == null) {
                showError("Ошибка", "Выберите дату окончания действия");
                event.consume();
                return;
            }
            if (validFromPicker.getValue().isAfter(validToPicker.getValue())) {
                showError("Ошибка", "Дата начала не может быть позже даты окончания");
                event.consume();
            }
        });

        dialog.setResultConverter(dialogButton -> {
            if (dialogButton == saveButtonType) {
                try {
                    PromoCode newCode = promoCode != null ? promoCode : new PromoCode();
                    newCode.setCode(codeField.getText().trim());
                    newCode.setDiscountType(typeCombo.getValue());
                    newCode.setDiscountValue(new BigDecimal(valueField.getText().trim()));
                    newCode.setValidFrom(validFromPicker.getValue());
                    newCode.setValidTo(validToPicker.getValue());
                    newCode.setMaxUses(maxUsesField.getText().trim().isEmpty() ? null : Integer.parseInt(maxUsesField.getText().trim()));
                    newCode.setMinOrderAmount(new BigDecimal(minOrderField.getText().trim()));
                    if (promoCode == null) {
                        newCode.setUsedCount(0);
                    }
                    return newCode;
                } catch (NumberFormatException e) {
                    showError("Ошибка", "Введите корректные числовые значения");
                    return null;
                }
            }
            return null;
        });

        dialog.showAndWait().ifPresent(result -> {
            try {
                if (promoCode == null) {
                    repositoryService.createPromoCode(result);
                    showInfo("Успех", "Промокод добавлен");
                } else {
                    repositoryService.updatePromoCode(result);
                    showInfo("Успех", "Промокод обновлён");
                }
                handleRefresh();
            } catch (Exception e) {
                e.printStackTrace();
                showError("Ошибка", "Не удалось сохранить промокод: " + e.getMessage());
            }
        });
    }

    public void updateStatus(String message) {
        if (statusLabel != null) {
            statusLabel.setText(message);
        }
    }
}
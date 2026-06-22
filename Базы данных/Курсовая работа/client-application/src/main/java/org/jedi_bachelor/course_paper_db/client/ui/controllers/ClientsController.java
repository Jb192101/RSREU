package org.jedi_bachelor.course_paper_db.client.ui.controllers;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.geometry.Insets;
import javafx.scene.control.*;
import javafx.scene.control.cell.PropertyValueFactory;
import javafx.scene.layout.GridPane;
import org.jedi_bachelor.course_paper_db.client.entity.Client;
import org.springframework.stereotype.Component;

import java.time.LocalDate;
import java.util.List;

@Component
public class ClientsController extends BaseController {

    @FXML private TableView<Client> clientsTable;
    @FXML private TableColumn<Client, Long> idColumn;
    @FXML private TableColumn<Client, String> firstNameColumn;
    @FXML private TableColumn<Client, String> lastNameColumn;
    @FXML private TableColumn<Client, String> emailColumn;
    @FXML private TableColumn<Client, String> phoneColumn;
    @FXML private TableColumn<Client, LocalDate> regDateColumn;
    @FXML private TableColumn<Client, Integer> pointsColumn;

    private ObservableList<Client> clientsList = FXCollections.observableArrayList();

    @FXML
    public void initialize() {
        setupTableColumns();
        handleRefresh();
    }

    private void setupTableColumns() {
        idColumn.setCellValueFactory(new PropertyValueFactory<>("id"));
        firstNameColumn.setCellValueFactory(new PropertyValueFactory<>("firstName"));
        lastNameColumn.setCellValueFactory(new PropertyValueFactory<>("lastName"));
        emailColumn.setCellValueFactory(new PropertyValueFactory<>("email"));
        phoneColumn.setCellValueFactory(new PropertyValueFactory<>("phone"));
        regDateColumn.setCellValueFactory(new PropertyValueFactory<>("registrationDate"));
        pointsColumn.setCellValueFactory(new PropertyValueFactory<>("loyaltyPoints"));

        clientsTable.setItems(clientsList);
    }

    @Override
    protected void handleAdd() {
        showClientDialog("Добавление клиента", null);
    }

    @Override
    protected void handleEdit() {
        Client selected = clientsTable.getSelectionModel().getSelectedItem();
        if (selected == null) {
            showError("Ошибка", "Выберите клиента для редактирования");
            return;
        }
        showClientDialog("Редактирование клиента", selected);
    }

    @Override
    protected void handleDelete() {
        Client selected = clientsTable.getSelectionModel().getSelectedItem();
        if (selected == null) {
            showError("Ошибка", "Выберите клиента для удаления");
            return;
        }

        if (showConfirmation("Подтверждение", "Удалить клиента \"" + selected.getFirstName() + " " + selected.getLastName() + "\"?")) {
            try {
                repositoryService.deleteClient(selected.getId());
                handleRefresh();
                showInfo("Успех", "Клиент удалён");
            } catch (Exception e) {
                showError("Ошибка", "Не удалось удалить клиента: " + e.getMessage());
            }
        }
    }

    @Override
    protected void handleRefresh() {
        try {
            List<Client> clients = repositoryService.getAllClients();
            clientsList.setAll(clients);
            updateStatus("Загружено клиентов: " + clients.size());
        } catch (Exception e) {
            showError("Ошибка", "Не удалось загрузить клиентов: " + e.getMessage());
        }
    }

    @Override
    protected void handleSearch() {
        String searchTerm = searchField.getText();
        if (searchTerm == null || searchTerm.trim().isEmpty()) {
            handleRefresh();
            return;
        }

        // Фильтрация в памяти
        List<Client> filtered = clientsList.stream()
                .filter(c -> c.getFirstName().toLowerCase().contains(searchTerm.toLowerCase()) ||
                        c.getLastName().toLowerCase().contains(searchTerm.toLowerCase()) ||
                        (c.getEmail() != null && c.getEmail().toLowerCase().contains(searchTerm.toLowerCase())))
                .toList();
        clientsList.setAll(filtered);
        updateStatus("Найдено клиентов: " + filtered.size());
    }

    @FXML
    public void handleAddPoints() {
        Client selected = clientsTable.getSelectionModel().getSelectedItem();
        if (selected == null) {
            showError("Ошибка", "Выберите клиента");
            return;
        }

        TextInputDialog dialog = new TextInputDialog();
        dialog.setTitle("Начисление баллов");
        dialog.setHeaderText("Клиент: " + selected.getFirstName() + " " + selected.getLastName());
        dialog.setContentText("Введите количество баллов:");

        dialog.showAndWait().ifPresent(points -> {
            try {
                businessService.addLoyaltyPoints(selected.getId(), Integer.parseInt(points));
                handleRefresh();
                showInfo("Успех", "Начислено " + points + " баллов");
            } catch (Exception e) {
                showError("Ошибка", e.getMessage());
            }
        });
    }

    @FXML
    public void handleSpendPoints() {
        Client selected = clientsTable.getSelectionModel().getSelectedItem();
        if (selected == null) {
            showError("Ошибка", "Выберите клиента");
            return;
        }

        TextInputDialog dialog = new TextInputDialog();
        dialog.setTitle("Списание баллов");
        dialog.setHeaderText("Клиент: " + selected.getFirstName() + " " + selected.getLastName() +
                "\nДоступно баллов: " + selected.getLoyaltyPoints());
        dialog.setContentText("Введите количество баллов для списания:");

        dialog.showAndWait().ifPresent(points -> {
            try {
                businessService.spendLoyaltyPoints(selected.getId(), Integer.parseInt(points));
                handleRefresh();
                showInfo("Успех", "Списано " + points + " баллов");
            } catch (Exception e) {
                showError("Ошибка", e.getMessage());
            }
        });
    }

    @FXML
    public void handleTopCustomers() {
        TextInputDialog dialog = new TextInputDialog("10");
        dialog.setTitle("Топ клиентов");
        dialog.setHeaderText("Показать лучших клиентов");
        dialog.setContentText("Введите количество:");

        dialog.showAndWait().ifPresent(limit -> {
            try {
                List<Object[]> topCustomers = businessService.getTopCustomers(Integer.parseInt(limit));
                StringBuilder sb = new StringBuilder("Топ клиентов:\n\n");
                for (Object[] customer : topCustomers) {
                    sb.append(String.format("%s %s - Сумма: %.2f руб.\n",
                            customer[1], customer[2], customer[4]));
                }
                showInfo("Топ клиентов", sb.toString());
            } catch (Exception e) {
                showError("Ошибка", e.getMessage());
            }
        });
    }

    private void showClientDialog(String title, Client client) {
        Dialog<Client> dialog = new Dialog<>();
        dialog.setTitle(title);
        dialog.setHeaderText(null);

        ButtonType saveButtonType = new ButtonType("Сохранить", ButtonBar.ButtonData.OK_DONE);
        dialog.getDialogPane().getButtonTypes().addAll(saveButtonType, ButtonType.CANCEL);

        GridPane grid = new GridPane();
        grid.setHgap(10);
        grid.setVgap(10);
        grid.setPadding(new Insets(20, 150, 10, 10));

        TextField firstNameField = new TextField();
        TextField lastNameField = new TextField();
        TextField emailField = new TextField();
        TextField phoneField = new TextField();

        if (client != null) {
            firstNameField.setText(client.getFirstName());
            lastNameField.setText(client.getLastName());
            emailField.setText(client.getEmail());
            phoneField.setText(client.getPhone());
        }

        grid.add(new Label("Имя:"), 0, 0);
        grid.add(firstNameField, 1, 0);
        grid.add(new Label("Фамилия:"), 0, 1);
        grid.add(lastNameField, 1, 1);
        grid.add(new Label("Email:"), 0, 2);
        grid.add(emailField, 1, 2);
        grid.add(new Label("Телефон:"), 0, 3);
        grid.add(phoneField, 1, 3);

        dialog.getDialogPane().setContent(grid);

        dialog.setResultConverter(dialogButton -> {
            if (dialogButton == saveButtonType) {
                Client newClient = client != null ? client : new Client();
                newClient.setFirstName(firstNameField.getText());
                newClient.setLastName(lastNameField.getText());
                newClient.setEmail(emailField.getText());
                newClient.setPhone(phoneField.getText());
                if (client == null) {
                    newClient.setRegistrationDate(LocalDate.now());
                    newClient.setLoyaltyPoints(0);
                }
                return newClient;
            }
            return null;
        });

        dialog.showAndWait().ifPresent(result -> {
            try {
                if (client == null) {
                    repositoryService.createClient(result);
                } else {
                    repositoryService.updateClient(result);
                }
                handleRefresh();
                showInfo("Успех", "Клиент сохранён");
            } catch (Exception e) {
                showError("Ошибка", "Не удалось сохранить клиента: " + e.getMessage());
            }
        });
    }

    public void updateStatus(String message) {
        // Обновить статус в главном окне
        if (statusLabel != null) {
            statusLabel.setText(message);
        }
    }
}
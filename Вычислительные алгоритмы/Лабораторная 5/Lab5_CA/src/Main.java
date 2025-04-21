import javafx.application.Application;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.layout.VBox;
import javafx.stage.Stage;

import model.ModelTaskFirst;
import model.ModelTaskSecond;
import model.ModelTaskThird;
import viewmodel.ViewModelTaskFirst;
import viewmodel.ViewModelTaskSecond;
import viewmodel.ViewModelTaskThird;

public class Main extends Application {
    ViewModelTaskFirst vmtf;
    ViewModelTaskSecond vmts;
    ViewModelTaskThird vmtt;

    public static void main(String[] args) {
        launch(args);
    }

    @Override
    public void start(Stage stage) throws Exception {
        Stage primaryStage = new Stage();
        this.vmtf = new ViewModelTaskFirst(new ModelTaskFirst(1, 2.6f, (float) Math.E));
        this.vmts = new ViewModelTaskSecond(new ModelTaskSecond(0, 2));
        this.vmtt = new ViewModelTaskThird(new ModelTaskThird());

        // Создаем кнопки меню
        Button button1 = new Button("Задание 1");
        Button button2 = new Button("Задание 2");
        Button button3 = new Button("Задание 3");

        // Устанавливаем действия для кнопок
        button1.setOnAction(e -> vmtf.openWindow());
        button2.setOnAction(e -> vmts.openWindow());
        button3.setOnAction(e -> vmtt.openWindow());

        // Создаем вертикальный контейнер для кнопок
        VBox root = new VBox(10, button1, button2, button3);
        root.setStyle("-fx-padding: 20; -fx-alignment: center;");

        // Настраиваем главное окно
        Scene scene = new Scene(root, 300, 250);
        primaryStage.setTitle("Главное меню");
        primaryStage.setScene(scene);
        primaryStage.show();
    }
}
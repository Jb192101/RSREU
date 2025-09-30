import controller.MainController;
import javafx.application.Application;
import javafx.stage.Stage;
import model.ImageModel;
import view.MainView;

public class Main extends Application {
    @Override
    public void start(Stage primaryStage) {
        ImageModel model = new ImageModel();
        MainView view = new MainView(primaryStage);
        MainController controller = new MainController(model, view);

        view.show();
    }

    public static void main(String[] args) {
        launch(args);
    }
}
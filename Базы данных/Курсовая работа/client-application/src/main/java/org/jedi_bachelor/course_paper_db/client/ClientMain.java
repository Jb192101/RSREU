package org.jedi_bachelor.course_paper_db.client;

import javafx.application.Application;
import org.jedi_bachelor.course_paper_db.client.ui.MainApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

@SpringBootApplication
public class ClientMain {
    public static void main(String[] args) {
        Application.launch(MainApplication.class, args);
    }
}
package org.jedi_bachelor.orders;

import org.jedi_bachelor.orders.model.MainLogic;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

@SpringBootApplication
public class MainApplication {
	public static void main(String[] args) {
		SpringApplication.run(MainApplication.class, args);

		MainLogic mainLogic = new MainLogic();
		mainLogic.startApp();
	}
}

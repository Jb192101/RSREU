package org.jedi_bachelor.course_paper_db.config;

import io.swagger.v3.oas.models.OpenAPI;
import io.swagger.v3.oas.models.info.Contact;
import io.swagger.v3.oas.models.info.Info;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

@Configuration
public class SwaggerConfiguration {
    @Bean
    public OpenAPI customOpenAPI() {
        return new OpenAPI()
                .info(new Info()
                        .title("Books Reading API")
                        .version("1.0")
                        .description("API для работы по управлению книгами")
                        .contact(new Contact()
                                .name("Барышев Г.А.")
                                .email("baryshevgrigorion@yandex.ru")));
    }
}

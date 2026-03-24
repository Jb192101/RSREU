package org.jedi_bachelor.course_paper_db.entity;

import jakarta.persistence.*;
import jakarta.validation.constraints.NotBlank;
import jakarta.validation.constraints.NotNull;
import lombok.Data;

import java.time.LocalDate;
import java.util.UUID;

@Entity
@Table(name = "authors")
@Data
public class Author {
    @Id
    @GeneratedValue(strategy = GenerationType.AUTO)
    @Column(name = "author_id")
    private UUID authorId;

    @Column(name = "first_name", columnDefinition = "varchar(100)")
    @NotNull
    private String firstName;

    @Column(name = "last_name", columnDefinition = "varchar(100)")
    @NotNull
    private String lastName;

    @Column(name = "birth_date")
    private LocalDate birthDate;

    @NotBlank
    @Column(name = "author_id", columnDefinition = "varchar(100)")
    private String country;
}

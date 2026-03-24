package org.jedi_bachelor.course_paper_db.entity;

import jakarta.persistence.*;
import jakarta.validation.constraints.Email;
import jakarta.validation.constraints.NotBlank;
import jakarta.validation.constraints.NotNull;
import lombok.Data;

import java.util.UUID;

@Entity
@Table(name = "publishers")
@Data
public class Publisher {
    @Id
    @GeneratedValue(strategy = GenerationType.AUTO)
    @Column(name = "publisher_id")
    private UUID publisherId;

    @Column(name = "name", columnDefinition = "varchar(255)")
    @NotNull
    @NotBlank
    private String name;

    @Column(name = "phone")
    private String phone;

    @Email
    @Column(name = "email")
    private String email;
}

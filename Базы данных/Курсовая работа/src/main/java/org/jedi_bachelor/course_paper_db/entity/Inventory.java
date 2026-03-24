package org.jedi_bachelor.course_paper_db.entity;

import jakarta.persistence.*;
import jakarta.validation.constraints.NotNull;
import lombok.Data;

import java.util.UUID;

@Entity
@Table(name = "inventory")
@Data
public class Inventory {
    @Id
    @GeneratedValue(strategy = GenerationType.AUTO)
    private UUID inventoryId;

    @Column(name = "book_id")
    private Book book;

    @Column(name = "quantity")
    @NotNull
    private Integer quantity;
}

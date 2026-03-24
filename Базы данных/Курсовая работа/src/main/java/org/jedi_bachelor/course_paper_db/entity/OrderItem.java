package org.jedi_bachelor.course_paper_db.entity;

import jakarta.persistence.*;
import jakarta.validation.constraints.NotBlank;
import jakarta.validation.constraints.NotNull;
import jakarta.validation.constraints.Positive;
import lombok.Data;

import java.math.BigDecimal;
import java.util.UUID;

@Entity
@Table(name = "order_items")
@Data
public class OrderItem {
    @Id
    @GeneratedValue(strategy = GenerationType.AUTO)
    @Column(name = "order_item_id")
    private UUID orderItemId;

    @NotNull
    @Column(name = "order_id")
    private Order order;

    @NotNull
    @NotBlank
    @Column(name = "book_id")
    private UUID bookId;

    @Positive
    @Column(name = "quantity")
    private Integer quantity;

    @Column(name = "unit_price", columnDefinition = "default 1.0")
    private BigDecimal unitPrice;
}

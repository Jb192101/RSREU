package org.jedi_bachelor.course_paper_db.entity;

import jakarta.persistence.*;
import jakarta.validation.constraints.DecimalMin;
import lombok.Data;

import java.math.BigDecimal;
import java.sql.Timestamp;
import java.util.UUID;

@Entity
@Table(name = "orders")
@Data
public class Order {
    @Id
    @GeneratedValue(strategy = GenerationType.AUTO)
    @Column(name = "order_id")
    private UUID orderId;

    private Customer customer;

    private Timestamp orderDateCreation;

    @DecimalMin(value = "1.0")
    private BigDecimal totalAmount;

    private OrderStatus orderStatus;

    private String shippingAddress;
}

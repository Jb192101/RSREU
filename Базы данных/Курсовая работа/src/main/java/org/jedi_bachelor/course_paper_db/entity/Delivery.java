package org.jedi_bachelor.course_paper_db.entity;

import jakarta.persistence.*;
import lombok.Data;
import org.jedi_bachelor.course_paper_db.entity.enums.DeliveryStatus;
import java.time.LocalDateTime;

@Entity
@Table(name = "shipments")
@Data
public class Delivery {
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;

    @Column(name = "tracking_number", length = 100)
    private String trackingNumber;

    @Column(name = "carrier", nullable = false, length = 100)
    private String carrier;

    @Column(name = "shipped_date")
    private LocalDateTime shippedDate;

    @Column(name = "delivered_date")
    private LocalDateTime deliveredDate;

    @Column(name = "status", nullable = false, length = 50)
    @Enumerated(EnumType.STRING)
    private DeliveryStatus status;

    @OneToOne(fetch = FetchType.LAZY)
    @JoinColumn(name = "order_id", unique = true)
    private Order order;
}
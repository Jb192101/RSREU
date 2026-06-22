package org.jedi_bachelor.course_paper_db.client.entity;

import jakarta.persistence.*;
import lombok.Data;
import org.jedi_bachelor.course_paper_db.client.entity.enums.DeliveryStatus;

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
    private String statusString;

    @Transient
    private DeliveryStatus status;

    @OneToOne(fetch = FetchType.EAGER)
    @JoinColumn(name = "order_id", unique = true)
    private Order order;

    @PostLoad
    private void convertStatus() {
        if (statusString != null) {
            switch (statusString) {
                case "создано" -> this.status = DeliveryStatus.PENDING;
                case "обрабатывается" -> this.status = DeliveryStatus.PROCESSING;
                case "отправлено" -> this.status = DeliveryStatus.SHIPPED;
                case "в пути" -> this.status = DeliveryStatus.IN_TRANSIT;
                case "у курьера" -> this.status = DeliveryStatus.OUT_FOR_DELIVERY;
                case "доставлено" -> this.status = DeliveryStatus.DELIVERED;
                case "неудачно" -> this.status = DeliveryStatus.FAILED;
                case "возвращено" -> this.status = DeliveryStatus.RETURNED;
                case "отменено" -> this.status = DeliveryStatus.CANCELLED;
                default -> this.status = DeliveryStatus.PENDING;
            }
        }
    }

    @PrePersist
    @PreUpdate
    private void convertStatusString() {
        if (status != null) {
            this.statusString = switch (status) {
                case PENDING -> "создано";
                case PROCESSING -> "обрабатывается";
                case SHIPPED -> "отправлено";
                case IN_TRANSIT -> "в пути";
                case OUT_FOR_DELIVERY -> "у курьера";
                case DELIVERED -> "доставлено";
                case FAILED -> "неудачно";
                case RETURNED -> "возвращено";
                case CANCELLED -> "отменено";
            };
        }
    }
}
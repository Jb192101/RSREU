package org.jedi_bachelor.course_paper_db.client.entity;

import jakarta.persistence.*;
import lombok.Data;
import lombok.Getter;
import lombok.Setter;
import lombok.ToString;
import org.jedi_bachelor.course_paper_db.client.entity.enums.OrderStatus;

import java.math.BigDecimal;
import java.time.LocalDateTime;
import java.util.List;

@Entity
@Table(name = "orders")
@Data
public class Order {
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;

    @ManyToOne(fetch = FetchType.EAGER)
    @JoinColumn(name = "customer_id", nullable = false)
    private Client customer;

    @Column(name = "order_date", nullable = false)
    private LocalDateTime orderDate;

    @Column(name = "total_amount", nullable = false)
    private BigDecimal totalAmount;

    @Column(name = "discount_amount", nullable = false)
    private BigDecimal discountAmount;

    @Setter
    @Getter
    @Column(name = "status", nullable = false, length = 50)
    private String statusString;

    @Column(name = "shipping_address", nullable = false)
    private String shippingAddress;

    @ManyToOne(fetch = FetchType.EAGER)
    @JoinColumn(name = "promo_code_id")
    @ToString.Exclude
    private PromoCode promocode;

    @OneToMany(mappedBy = "order", cascade = CascadeType.ALL)
    @ToString.Exclude
    private List<OrderPosition> orderPositions;

    @Setter
    @Getter
    @Transient
    private OrderStatus status;

    @PostLoad
    private void convertStatus() {
        if (statusString != null) {
            switch (statusString) {
                case "ожидает оплаты" -> this.status = OrderStatus.PENDING_PAYMENT;
                case "оплачен" -> this.status = OrderStatus.PAID;
                case "обрабатывается" -> this.status = OrderStatus.PROCESSING;
                case "подтверждён" -> this.status = OrderStatus.CONFIRMED;
                case "отправлен" -> this.status = OrderStatus.SHIPPED;
                case "доставлен" -> this.status = OrderStatus.DELIVERED;
                case "отменён" -> this.status = OrderStatus.CANCELLED;
                case "возвращён" -> this.status = OrderStatus.REFUNDED;
                case "на удержании" -> this.status = OrderStatus.ON_HOLD;
                default -> this.status = OrderStatus.PENDING_PAYMENT;
            }
        }
    }

    @PrePersist
    @PreUpdate
    private void convertStatusString() {
        if (status != null) {
            this.statusString = switch (status) {
                case PENDING_PAYMENT -> "ожидает оплаты";
                case PAID -> "оплачен";
                case PROCESSING -> "обрабатывается";
                case CONFIRMED -> "подтверждён";
                case SHIPPED -> "отправлен";
                case DELIVERED -> "доставлен";
                case CANCELLED -> "отменён";
                case REFUNDED -> "возвращён";
                case ON_HOLD -> "на удержании";
            };
        }
    }
}
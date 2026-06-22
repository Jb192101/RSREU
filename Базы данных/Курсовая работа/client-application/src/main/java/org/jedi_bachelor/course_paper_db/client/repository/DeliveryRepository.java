package org.jedi_bachelor.course_paper_db.client.repository;

import org.jedi_bachelor.course_paper_db.client.entity.Delivery;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Modifying;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;
import org.springframework.stereotype.Repository;

@Repository
public interface DeliveryRepository extends JpaRepository<Delivery, Long> {
    @Modifying
    @Query(value = "DELETE FROM shipments WHERE shipment_id = :id", nativeQuery = true)
    void deleteDeliveryById(@Param("id") Long id);

    @Modifying
    @Query(value = "UPDATE shipments SET tracking_number = :#{#delivery.trackingNumber}, " +
            "carrier = :#{#delivery.carrier}, " +
            "shipped_date = :#{#delivery.shippedDate}, " +
            "delivered_date = :#{#delivery.deliveredDate}, " +
            "status = :#{#delivery.statusString}, " +
            "order_id = :#{#delivery.order.id} " +
            "WHERE shipment_id = :#{#delivery.id}", nativeQuery = true)
    void updateDelivery(@Param("delivery") Delivery delivery);

    @Query(value = "SELECT sp_create_shipment(:orderId, :carrier, :trackingNumber)", nativeQuery = true)
    Long createShipment(@Param("orderId") Long orderId,
                        @Param("carrier") String carrier,
                        @Param("trackingNumber") String trackingNumber);

    @Modifying
    @Query(value = "SELECT sp_update_shipment_status(:shipmentId, :newStatus, :deliveredDate)",
            nativeQuery = true)
    void updateShipmentStatus(@Param("shipmentId") Long shipmentId,
                              @Param("newStatus") String newStatus,
                              @Param("deliveredDate") java.time.LocalDateTime deliveredDate);
}
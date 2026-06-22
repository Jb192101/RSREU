package org.jedi_bachelor.course_paper_db.client.repository;

import org.jedi_bachelor.course_paper_db.client.entity.OrderPosition;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Modifying;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;
import org.springframework.stereotype.Repository;
import java.util.List;
import java.util.Optional;

@Repository
public interface OrderPositionRepository extends JpaRepository<OrderPosition, Long> {
    @Query(value = "SELECT oi.*, o.*, b.* FROM order_items oi " +
            "LEFT JOIN orders o ON oi.order_id = o.order_id " +
            "LEFT JOIN books b ON oi.book_id = b.book_id " +
            "WHERE oi.order_item_id = :id", nativeQuery = true)
    Optional<OrderPosition> findOrderPositionById(@Param("id") Long id);

    @Query(value = "SELECT oi.*, o.*, b.* FROM order_items oi " +
            "LEFT JOIN orders o ON oi.order_id = o.order_id " +
            "LEFT JOIN books b ON oi.book_id = b.book_id", nativeQuery = true)
    List<OrderPosition> findAllOrderPositions();

    @Modifying
    @Query(value = "DELETE FROM order_items WHERE order_item_id = :id", nativeQuery = true)
    void deleteOrderPositionById(@Param("id") Long id);

    @Modifying
    @Query(value = "UPDATE order_items SET quantity = :#{#position.quantity}, " +
            "unit_price = :#{#position.unitPrice}, " +
            "order_id = :#{#position.order.id}, book_id = :#{#position.book.id} " +
            "WHERE order_item_id = :#{#position.id}", nativeQuery = true)
    void updateOrderPosition(@Param("position") OrderPosition position);

    @Query(value = "SELECT sp_add_item_to_order(:orderId, :bookId, :quantity)", nativeQuery = true)
    Long addItemToOrder(@Param("orderId") Long orderId,
                        @Param("bookId") Long bookId,
                        @Param("quantity") Integer quantity);

    @Modifying
    @Query(value = "SELECT sp_remove_item_from_order(:orderItemId)", nativeQuery = true)
    void removeItemFromOrder(@Param("orderItemId") Long orderItemId);

    @Modifying
    @Query(value = "SELECT sp_update_order_item_quantity(:orderItemId, :newQuantity)", nativeQuery = true)
    void updateOrderItemQuantity(@Param("orderItemId") Long orderItemId,
                                 @Param("newQuantity") Integer newQuantity);
}
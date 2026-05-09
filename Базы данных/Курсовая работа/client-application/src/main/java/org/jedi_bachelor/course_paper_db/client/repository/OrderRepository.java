package org.jedi_bachelor.course_paper_db.client.repository;

import org.jedi_bachelor.course_paper_db.client.entity.Order;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Modifying;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;
import org.springframework.stereotype.Repository;
import java.util.List;
import java.util.Optional;

@Repository
public interface OrderRepository extends JpaRepository<Order, Long> {

    @Query(value = "SELECT o.*, c.*, pc.* FROM orders o " +
            "LEFT JOIN customers c ON o.customer_id = c.customer_id " +
            "LEFT JOIN promocodes pc ON o.promo_code_id = pc.promo_code_id " +
            "WHERE o.order_id = :id", nativeQuery = true)
    Optional<Order> findOrderById(@Param("id") Long id);

    @Query(value = "SELECT o.*, c.*, pc.* FROM orders o " +
            "LEFT JOIN customers c ON o.customer_id = c.customer_id " +
            "LEFT JOIN promocodes pc ON o.promo_code_id = pc.promo_code_id",
            nativeQuery = true)
    List<Order> findAllOrders();

    @Modifying
    @Query(value = "DELETE FROM orders WHERE order_id = :id", nativeQuery = true)
    void deleteOrderById(@Param("id") Long id);

    @Modifying
    @Query(value = "UPDATE orders SET total_amount = :#{#order.totalAmount}, " +
            "discount_amount = :#{#order.discountAmount}, status = :#{#order.status.name()}, " +
            "shipping_address = :#{#order.shippingAddress}, " +
            "customer_id = :#{#order.customer.id}, promo_code_id = :#{#order.promocode?.id} " +
            "WHERE order_id = :#{#order.id}", nativeQuery = true)
    void updateOrder(@Param("order") Order order);

    @Query(value = "SELECT sp_create_order(:customerId, :shippingAddress, :promoCode)",
            nativeQuery = true)
    Long createOrder(@Param("customerId") Long customerId,
                     @Param("shippingAddress") String shippingAddress,
                     @Param("promoCode") String promoCode);

    @Modifying
    @Query(value = "SELECT sp_change_order_status(:orderId, :newStatus)", nativeQuery = true)
    void changeOrderStatus(@Param("orderId") Long orderId,
                           @Param("newStatus") String newStatus);

    @Modifying
    @Query(value = "SELECT sp_apply_promocode_to_order(:orderId, :promoCode)", nativeQuery = true)
    void applyPromocodeToOrder(@Param("orderId") Long orderId,
                               @Param("promoCode") String promoCode);

    @Modifying
    @Query(value = "SELECT sp_cancel_order(:orderId)", nativeQuery = true)
    void cancelOrder(@Param("orderId") Long orderId);

    @Query(value = "SELECT * FROM sp_get_customer_orders(:customerId)", nativeQuery = true)
    List<Object[]> getCustomerOrders(@Param("customerId") Long customerId);

    @Query(value = "SELECT * FROM sp_track_order(:orderId)", nativeQuery = true)
    List<Object[]> trackOrder(@Param("orderId") Long orderId);

    // Отчёт о продажах
    @Query(value = "SELECT * FROM sp_get_sales_report(:dateFrom, :dateTo, :genreId)", nativeQuery = true)
    List<Object[]> getSalesReport(@Param("dateFrom") java.time.LocalDate dateFrom,
                                  @Param("dateTo") java.time.LocalDate dateTo,
                                  @Param("genreId") Long genreId);

    // Выручка по жанрам
    @Query(value = "SELECT * FROM sp_get_revenue_by_genre(:dateFrom, :dateTo)", nativeQuery = true)
    List<Object[]> getRevenueByGenre(@Param("dateFrom") java.time.LocalDate dateFrom,
                                     @Param("dateTo") java.time.LocalDate dateTo);

    // Статистика в определённый день
    @Query(value = "SELECT * FROM sp_get_daily_stats(:reportDate)", nativeQuery = true)
    List<Object[]> getDailyStats(@Param("reportDate") java.time.LocalDate reportDate);

    // Обработка жизненного цикла заказа
    @Query(value = "SELECT * FROM sp_process_order_workflow(:orderId)", nativeQuery = true)
    List<Object[]> processOrderWorkflow(@Param("orderId") Long orderId);
}
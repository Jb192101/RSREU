package org.jedi_bachelor.course_paper_db.client.repository;

import org.jedi_bachelor.course_paper_db.client.entity.Order;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Modifying;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;
import org.springframework.stereotype.Repository;
import org.springframework.transaction.annotation.Transactional;

import java.util.List;

@Repository
public interface OrderRepository extends JpaRepository<Order, Long> {

    @Modifying
    @Transactional
    @Query(value = "DELETE FROM orders WHERE order_id = :id", nativeQuery = true)
    void deleteOrderById(@Param("id") Long id);

    @Modifying
    @Transactional
    @Query(value = "UPDATE orders SET total_amount = :#{#order.totalAmount}, " +
            "discount_amount = :#{#order.discountAmount}, " +
            "status = :#{#order.statusString}, " +
            "shipping_address = :#{#order.shippingAddress}, " +
            "customer_id = :#{#order.customer.id}, " +
            "promo_code_id = :#{#order.promocode != null ? order.promocode.id : null} " +
            "WHERE order_id = :#{#order.id}", nativeQuery = true)
    void updateOrder(@Param("order") Order order);

    @Query(value = "SELECT sp_create_order(:customerId, :shippingAddress, :promoCode)",
            nativeQuery = true)
    Long createOrder(@Param("customerId") Long customerId,
                     @Param("shippingAddress") String shippingAddress,
                     @Param("promoCode") String promoCode);

    @Query("SELECT DISTINCT o FROM Order o LEFT JOIN FETCH o.customer LEFT JOIN FETCH o.orderPositions")
    List<Order> findAllWithDetails();

    @Modifying
    @Transactional
    @Query(value = "SELECT sp_change_order_status(:orderId, :newStatus)", nativeQuery = true)
    void changeOrderStatus(@Param("orderId") Long orderId,
                           @Param("newStatus") String newStatus);

    @Modifying
    @Transactional
    @Query(value = "SELECT sp_apply_promocode_to_order(:orderId, :promoCode)", nativeQuery = true)
    void applyPromocodeToOrder(@Param("orderId") Long orderId,
                               @Param("promoCode") String promoCode);

    @Modifying
    @Transactional
    @Query(value = "SELECT sp_cancel_order(:orderId)", nativeQuery = true)
    void cancelOrder(@Param("orderId") Long orderId);

    @Query(value = "SELECT * FROM sp_get_customer_orders(:customerId)", nativeQuery = true)
    List<Object[]> getCustomerOrders(@Param("customerId") Long customerId);

    @Query(value = "SELECT * FROM sp_track_order(:orderId)", nativeQuery = true)
    List<Object[]> trackOrder(@Param("orderId") Long orderId);

    @Query(value = "SELECT * FROM sp_get_sales_report(:dateFrom, :dateTo, :genreId)", nativeQuery = true)
    List<Object[]> getSalesReport(@Param("dateFrom") java.time.LocalDate dateFrom,
                                  @Param("dateTo") java.time.LocalDate dateTo,
                                  @Param("genreId") Long genreId);

    @Query(value = "SELECT * FROM sp_get_revenue_by_genre(:dateFrom, :dateTo)", nativeQuery = true)
    List<Object[]> getRevenueByGenre(@Param("dateFrom") java.time.LocalDate dateFrom,
                                     @Param("dateTo") java.time.LocalDate dateTo);

    @Query(value = "SELECT * FROM sp_get_daily_stats(:reportDate)", nativeQuery = true)
    List<Object[]> getDailyStats(@Param("reportDate") java.time.LocalDate reportDate);

    @Query(value = "SELECT * FROM sp_process_order_workflow(:orderId)", nativeQuery = true)
    List<Object[]> processOrderWorkflow(@Param("orderId") Long orderId);
}
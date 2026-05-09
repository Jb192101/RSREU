package org.jedi_bachelor.course_paper_db.client.repository;

import org.jedi_bachelor.course_paper_db.client.entity.BookRemnant;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Modifying;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;
import org.springframework.stereotype.Repository;
import java.util.List;
import java.util.Optional;

@Repository
public interface InventoryRepository extends JpaRepository<BookRemnant, Long> {

    @Query(value = "SELECT i.*, b.* FROM inventory i " +
            "LEFT JOIN books b ON i.book_id = b.book_id " +
            "WHERE i.inventory_id = :id", nativeQuery = true)
    Optional<BookRemnant> findInventoryById(@Param("id") Long id);

    @Query(value = "SELECT i.*, b.* FROM inventory i " +
            "LEFT JOIN books b ON i.book_id = b.book_id", nativeQuery = true)
    List<BookRemnant> findAllInventory();

    @Modifying
    @Query(value = "DELETE FROM inventory WHERE inventory_id = :id", nativeQuery = true)
    void deleteInventoryById(@Param("id") Long id);

    @Modifying
    @Query(value = "UPDATE inventory SET quantity = :#{#inventory.quantity}, " +
            "reserved_quantity = :#{#inventory.reservedQuantity}, " +
            "last_restocked = :#{#inventory.lastRestocked}, " +
            "book_id = :#{#inventory.book.id} " +
            "WHERE inventory_id = :#{#inventory.id}", nativeQuery = true)
    void updateInventory(@Param("inventory") BookRemnant inventory);

    // Операция со складом
    @Query(value = "SELECT * FROM sp_update_inventory(:bookId, :quantityChange)", nativeQuery = true)
    List<Object[]> updateInventoryStock(@Param("bookId") Long bookId,
                                        @Param("quantityChange") Integer quantityChange);

    // Книги с низким остатком
    @Query(value = "SELECT * FROM sp_get_low_stock_books(:threshold)", nativeQuery = true)
    List<Object[]> getLowStockBooks(@Param("threshold") Integer threshold);

    // Стоимость складских запасов
    @Query(value = "SELECT * FROM sp_get_inventory_value()", nativeQuery = true)
    List<Object[]> getInventoryValue();
}
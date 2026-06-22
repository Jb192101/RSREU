package org.jedi_bachelor.course_paper_db.client.repository;

import org.jedi_bachelor.course_paper_db.client.entity.Book;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Modifying;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;
import org.springframework.stereotype.Repository;
import java.util.List;

@Repository
public interface BookRepository extends JpaRepository<Book, Long> {
    @Modifying
    @Query(value = "DELETE FROM books WHERE book_id = :id", nativeQuery = true)
    void deleteBookById(@Param("id") Long id);

    @Query(value = "SELECT * FROM sp_search_books(:searchTerm, :genreId, :authorId, " +
            ":minPrice, :maxPrice, :yearFrom, :yearTo, :pageNumber, :pageSize)",
            nativeQuery = true)
    List<Object[]> searchBooks(@Param("searchTerm") String searchTerm,
                               @Param("genreId") Long genreId,
                               @Param("authorId") Long authorId,
                               @Param("minPrice") java.math.BigDecimal minPrice,
                               @Param("maxPrice") java.math.BigDecimal maxPrice,
                               @Param("yearFrom") Integer yearFrom,
                               @Param("yearTo") Integer yearTo,
                               @Param("pageNumber") Integer pageNumber,
                               @Param("pageSize") Integer pageSize);

    @Query(value = "SELECT * FROM sp_get_books_by_author(:authorId)", nativeQuery = true)
    List<Object[]> getBooksByAuthor(@Param("authorId") Long authorId);

    @Modifying
    @Query(value = "SELECT * FROM sp_bulk_update_prices(:percentageChange, :genreId, :publisherId)",
            nativeQuery = true)
    List<Object[]> bulkUpdatePrices(@Param("genreId") Long genreId,
                                    @Param("publisherId") Long publisherId,
                                    @Param("percentageChange") java.math.BigDecimal percentageChange);

    @Query("SELECT DISTINCT b FROM Book b LEFT JOIN FETCH b.publisher LEFT JOIN FETCH b.genre")
    List<Book> findAllWithDetails();
}
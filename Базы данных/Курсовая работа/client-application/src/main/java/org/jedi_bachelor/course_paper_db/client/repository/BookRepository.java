package org.jedi_bachelor.course_paper_db.client.repository;

import org.jedi_bachelor.course_paper_db.client.entity.Book;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Modifying;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;
import org.springframework.stereotype.Repository;
import java.util.List;
import java.util.Optional;

@Repository
public interface BookRepository extends JpaRepository<Book, Long> {

    @Query(value = "SELECT b.*, p.*, g.* FROM books b " +
            "LEFT JOIN publishers p ON b.publisher_id = p.publisher_id " +
            "LEFT JOIN genres g ON b.genre_id = g.genre_id " +
            "WHERE b.book_id = :id", nativeQuery = true)
    Optional<Book> findBookById(@Param("id") Long id);

    @Query(value = "SELECT b.*, p.*, g.* FROM books b " +
            "LEFT JOIN publishers p ON b.publisher_id = p.publisher_id " +
            "LEFT JOIN genres g ON b.genre_id = g.genre_id", nativeQuery = true)
    List<Book> findAllBooks();

    @Modifying
    @Query(value = "DELETE FROM books WHERE book_id = :id", nativeQuery = true)
    void deleteBookById(@Param("id") Long id);

    @Modifying
    @Query(value = "UPDATE books SET isbn = :#{#book.isbn}, title = :#{#book.title}, " +
            "publication_year = :#{#book.publicationYear}, price = :#{#book.price}, " +
            "pages = :#{#book.pages}, description = :#{#book.description}, " +
            "publisher_id = :#{#book.publisher.id}, genre_id = :#{#book.genre.id} " +
            "WHERE book_id = :#{#book.id}", nativeQuery = true)
    void updateBook(@Param("book") Book book);

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

    // Массовое изменение цен
    @Modifying
    @Query(value = "SELECT * FROM sp_bulk_update_prices(:genreId, :publisherId, :percentageChange)",
            nativeQuery = true)
    List<Object[]> bulkUpdatePrices(@Param("genreId") Long genreId,
                                    @Param("publisherId") Long publisherId,
                                    @Param("percentageChange") java.math.BigDecimal percentageChange);
}
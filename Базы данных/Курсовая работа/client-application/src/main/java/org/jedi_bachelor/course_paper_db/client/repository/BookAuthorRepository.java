package org.jedi_bachelor.course_paper_db.client.repository;

import org.jedi_bachelor.course_paper_db.client.entity.BookAuthor;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Modifying;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;
import org.springframework.stereotype.Repository;
import java.util.List;

@Repository
public interface BookAuthorRepository extends JpaRepository<BookAuthor, BookAuthor.BookAuthorId> {

    @Query(value = "SELECT ba.*, b.*, a.* FROM book_authors ba " +
            "LEFT JOIN books b ON ba.book_id = b.book_id " +
            "LEFT JOIN authors a ON ba.author_id = a.author_id " +
            "WHERE ba.book_id = :bookId AND ba.author_id = :authorId",
            nativeQuery = true)
    BookAuthor findBookAuthorById(@Param("bookId") Long bookId,
                                  @Param("authorId") Long authorId);

    @Query(value = "SELECT ba.*, b.*, a.* FROM book_authors ba " +
            "LEFT JOIN books b ON ba.book_id = b.book_id " +
            "LEFT JOIN authors a ON ba.author_id = a.author_id",
            nativeQuery = true)
    List<BookAuthor> findAllBookAuthors();

    @Modifying
    @Query(value = "DELETE FROM book_authors WHERE book_id = :bookId AND author_id = :authorId",
            nativeQuery = true)
    void deleteBookAuthorById(@Param("bookId") Long bookId,
                              @Param("authorId") Long authorId);

    @Modifying
    @Query(value = "UPDATE book_authors SET author_order = :order " +
            "WHERE book_id = :bookId AND author_id = :authorId",
            nativeQuery = true)
    void updateBookAuthorOrder(@Param("bookId") Long bookId,
                               @Param("authorId") Long authorId,
                               @Param("order") Integer order);
}
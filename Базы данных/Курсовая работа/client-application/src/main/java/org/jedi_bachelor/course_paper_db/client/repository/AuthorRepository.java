package org.jedi_bachelor.course_paper_db.client.repository;

import org.jedi_bachelor.course_paper_db.client.entity.Author;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Modifying;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;
import org.springframework.stereotype.Repository;
import java.util.List;
import java.util.Optional;

@Repository
public interface AuthorRepository extends JpaRepository<Author, Long> {

    @Query(value = "SELECT * FROM authors WHERE author_id = :id", nativeQuery = true)
    Optional<Author> findAuthorById(@Param("id") Long id);

    @Query(value = "SELECT * FROM authors", nativeQuery = true)
    List<Author> findAllAuthors();

    @Modifying
    @Query(value = "DELETE FROM authors WHERE author_id = :id", nativeQuery = true)
    void deleteAuthorById(@Param("id") Long id);

    @Modifying
    @Query(value = "UPDATE authors SET first_name = :#{#author.firstName}, " +
            "last_name = :#{#author.lastName}, birth_date = :#{#author.birthDate}, " +
            "country = :#{#author.country}, biography = :#{#author.biography} " +
            "WHERE author_id = :#{#author.id}", nativeQuery = true)
    void updateAuthor(@Param("author") Author author);

    @Modifying
    @Query(value = "SELECT sp_link_author_to_book(:bookId, :authorId, :authorOrder)", nativeQuery = true)
    void linkAuthorToBook(@Param("bookId") Long bookId,
                          @Param("authorId") Long authorId,
                          @Param("authorOrder") Integer authorOrder);

    @Modifying
    @Query(value = "SELECT sp_unlink_author_from_book(:bookId, :authorId)", nativeQuery = true)
    void unlinkAuthorFromBook(@Param("bookId") Long bookId,
                              @Param("authorId") Long authorId);

    @Query(value = "SELECT * FROM sp_merge_authors(:sourceAuthorId, :targetAuthorId)", nativeQuery = true)
    List<Object[]> mergeAuthors(@Param("sourceAuthorId") Long sourceAuthorId,
                                @Param("targetAuthorId") Long targetAuthorId);
}
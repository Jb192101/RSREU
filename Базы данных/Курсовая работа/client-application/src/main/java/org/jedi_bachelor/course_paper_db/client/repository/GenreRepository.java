package org.jedi_bachelor.course_paper_db.client.repository;

import org.jedi_bachelor.course_paper_db.client.entity.Genre;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Modifying;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;
import org.springframework.stereotype.Repository;
import java.util.List;
import java.util.Optional;

@Repository
public interface GenreRepository extends JpaRepository<Genre, Long> {

    @Query(value = "SELECT g1.*, g2.* FROM genres g1 " +
            "LEFT JOIN genres g2 ON g1.parent_genre_id = g2.genre_id " +
            "WHERE g1.genre_id = :id", nativeQuery = true)
    Optional<Genre> findGenreById(@Param("id") Long id);

    @Query(value = "SELECT g1.*, g2.* FROM genres g1 " +
            "LEFT JOIN genres g2 ON g1.parent_genre_id = g2.genre_id",
            nativeQuery = true)
    List<Genre> findAllGenres();

    @Modifying
    @Query(value = "DELETE FROM genres WHERE genre_id = :id", nativeQuery = true)
    void deleteGenreById(@Param("id") Long id);

    @Modifying
    @Query(value = "UPDATE genres SET name = :#{#genre.name}, " +
            "parent_genre_id = :#{#genre.parentGenre?.id}, " +
            "description = :#{#genre.description} " +
            "WHERE genre_id = :#{#genre.id}", nativeQuery = true)
    void updateGenre(@Param("genre") Genre genre);
}
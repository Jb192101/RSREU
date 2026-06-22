package org.jedi_bachelor.course_paper_db.client.repository;

import org.jedi_bachelor.course_paper_db.client.entity.Genre;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Modifying;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;
import org.springframework.stereotype.Repository;
import org.springframework.transaction.annotation.Transactional;

@Repository
public interface GenreRepository extends JpaRepository<Genre, Long> {

    @Modifying
    @Transactional
    @Query(value = "DELETE FROM genres WHERE genre_id = :id", nativeQuery = true)
    void deleteGenreById(@Param("id") Long id);

    @Modifying
    @Transactional
    @Query(value = "UPDATE genres SET name = :#{#genre.name}, " +
            "description = :#{#genre.description} " +
            "WHERE genre_id = :#{#genre.id}", nativeQuery = true)
    void updateGenre(@Param("genre") Genre genre);
}
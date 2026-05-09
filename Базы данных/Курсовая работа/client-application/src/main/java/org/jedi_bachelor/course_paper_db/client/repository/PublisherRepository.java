package org.jedi_bachelor.course_paper_db.client.repository;

import org.jedi_bachelor.course_paper_db.client.entity.Publisher;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Modifying;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;
import org.springframework.stereotype.Repository;
import java.util.List;
import java.util.Optional;

@Repository
public interface PublisherRepository extends JpaRepository<Publisher, Long> {

    @Query(value = "SELECT * FROM publishers WHERE publisher_id = :id", nativeQuery = true)
    Optional<Publisher> findPublisherById(@Param("id") Long id);

    @Query(value = "SELECT * FROM publishers", nativeQuery = true)
    List<Publisher> findAllPublishers();

    @Modifying
    @Query(value = "DELETE FROM publishers WHERE publisher_id = :id", nativeQuery = true)
    void deletePublisherById(@Param("id") Long id);

    @Modifying
    @Query(value = "UPDATE publishers SET name = :#{#publisher.name}, " +
            "phone = :#{#publisher.phone}, email = :#{#publisher.email}, " +
            "address = :#{#publisher.address} " +
            "WHERE publisher_id = :#{#publisher.id}", nativeQuery = true)
    void updatePublisher(@Param("publisher") Publisher publisher);
}
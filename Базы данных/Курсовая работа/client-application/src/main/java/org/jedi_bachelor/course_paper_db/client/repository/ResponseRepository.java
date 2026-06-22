package org.jedi_bachelor.course_paper_db.client.repository;

import org.jedi_bachelor.course_paper_db.client.entity.Response;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Modifying;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;
import org.springframework.stereotype.Repository;
import java.util.List;
import java.util.Optional;

@Repository
public interface ResponseRepository extends JpaRepository<Response, Long> {
    @Modifying
    @Query(value = "DELETE FROM reviews WHERE review_id = :id", nativeQuery = true)
    void deleteResponseById(@Param("id") Long id);

    @Modifying
    @Query(value = "UPDATE reviews SET rating = :#{#response.rating}, " +
            "comment = :#{#response.comment}, " +
            "is_verified_purchase = :#{#response.isVerifiedPurchase}, " +
            "customer_id = :#{#response.client.id}, book_id = :#{#response.book.id} " +
            "WHERE review_id = :#{#response.id}", nativeQuery = true)
    void updateResponse(@Param("response") Response response);

    @Query(value = "SELECT sp_add_review(:customerId, :bookId, :rating, :comment)", nativeQuery = true)
    Long addReview(@Param("customerId") Long customerId,
                   @Param("bookId") Long bookId,
                   @Param("rating") Integer rating,
                   @Param("comment") String comment);

    @Query(value = "SELECT * FROM sp_get_book_rating(:bookId)", nativeQuery = true)
    List<Object[]> getBookRating(@Param("bookId") Long bookId);
}
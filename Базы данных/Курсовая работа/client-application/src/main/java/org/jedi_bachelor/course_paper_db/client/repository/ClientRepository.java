package org.jedi_bachelor.course_paper_db.client.repository;

import org.jedi_bachelor.course_paper_db.client.entity.Client;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Modifying;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;
import org.springframework.stereotype.Repository;
import org.springframework.transaction.annotation.Transactional;

import java.util.List;

@Repository
public interface ClientRepository extends JpaRepository<Client, Long> {

    @Modifying
    @Transactional
    @Query(value = "CALL sp_add_loyalty_points(:customerId, :pointsAmount)", nativeQuery = true)
    void addLoyaltyPoints(@Param("customerId") Long customerId,
                          @Param("pointsAmount") Integer pointsAmount);

    @Modifying
    @Transactional
    @Query(value = "CALL sp_spend_loyalty_points(:customerId, :pointsAmount)", nativeQuery = true)
    void spendLoyaltyPoints(@Param("customerId") Long customerId,
                            @Param("pointsAmount") Integer pointsAmount);

    @Query(value = "SELECT * FROM sp_get_top_customers(:limitCount)", nativeQuery = true)
    List<Object[]> getTopCustomers(@Param("limitCount") Integer limitCount);
}
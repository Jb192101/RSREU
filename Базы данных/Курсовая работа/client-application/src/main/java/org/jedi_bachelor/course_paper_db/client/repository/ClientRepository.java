package org.jedi_bachelor.course_paper_db.client.repository;

import org.jedi_bachelor.course_paper_db.client.entity.Client;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Modifying;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;
import org.springframework.stereotype.Repository;
import java.util.List;
import java.util.Optional;

@Repository
public interface ClientRepository extends JpaRepository<Client, Long> {

    @Query(value = "SELECT * FROM customers WHERE customer_id = :id", nativeQuery = true)
    Optional<Client> findClientById(@Param("id") Long id);

    @Query(value = "SELECT * FROM customers", nativeQuery = true)
    List<Client> findAllClients();

    @Modifying
    @Query(value = "DELETE FROM customers WHERE customer_id = :id", nativeQuery = true)
    void deleteClientById(@Param("id") Long id);

    @Modifying
    @Query(value = "UPDATE customers SET first_name = :#{#client.firstName}, " +
            "last_name = :#{#client.lastName}, email = :#{#client.email}, " +
            "phone = :#{#client.phone}, loyalty_points = :#{#client.loyaltyPoints} " +
            "WHERE customer_id = :#{#client.id}", nativeQuery = true)
    void updateClient(@Param("client") Client client);

    @Modifying
    @Query(value = "SELECT sp_add_loyalty_points(:customerId, :pointsAmount)", nativeQuery = true)
    void addLoyaltyPoints(@Param("customerId") Long customerId,
                          @Param("pointsAmount") Integer pointsAmount);

    @Modifying
    @Query(value = "SELECT sp_spend_loyalty_points(:customerId, :pointsAmount)", nativeQuery = true)
    void spendLoyaltyPoints(@Param("customerId") Long customerId,
                            @Param("pointsAmount") Integer pointsAmount);

    @Query(value = "SELECT * FROM sp_get_top_customers(:limitCount)", nativeQuery = true)
    List<Object[]> getTopCustomers(@Param("limitCount") Integer limitCount);
}
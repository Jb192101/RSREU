package org.jedi_bachelor.course_paper_db.client.repository;

import org.jedi_bachelor.course_paper_db.client.entity.PromoCode;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Modifying;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;
import org.springframework.stereotype.Repository;
import java.util.List;
import java.util.Optional;

@Repository
public interface PromoCodeRepository extends JpaRepository<PromoCode, Long> {

    @Query(value = "SELECT * FROM promocodes WHERE promo_code_id = :id", nativeQuery = true)
    Optional<PromoCode> findPromoCodeById(@Param("id") Long id);

    @Query(value = "SELECT * FROM promocodes", nativeQuery = true)
    List<PromoCode> findAllPromoCodes();

    @Modifying
    @Query(value = "DELETE FROM promocodes WHERE promo_code_id = :id", nativeQuery = true)
    void deletePromoCodeById(@Param("id") Long id);

    @Modifying
    @Query(value = "UPDATE promocodes SET code = :#{#promoCode.code}, " +
            "discount_type = :#{#promoCode.discountType.name()}, " +
            "discount_value = :#{#promoCode.discountValue}, " +
            "valid_from = :#{#promoCode.validFrom}, valid_to = :#{#promoCode.validTo}, " +
            "max_uses = :#{#promoCode.maxUses}, used_count = :#{#promoCode.usedCount}, " +
            "min_order_amount = :#{#promoCode.minOrderAmount} " +
            "WHERE promo_code_id = :#{#promoCode.id}", nativeQuery = true)
    void updatePromoCode(@Param("promoCode") PromoCode promoCode);

    @Query(value = "SELECT * FROM sp_validate_promocode(:code, :orderAmount)", nativeQuery = true)
    List<Object[]> validatePromocode(@Param("code") String code,
                                     @Param("orderAmount") java.math.BigDecimal orderAmount);
}
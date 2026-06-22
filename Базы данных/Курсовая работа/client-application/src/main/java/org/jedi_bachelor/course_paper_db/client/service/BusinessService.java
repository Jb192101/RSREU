package org.jedi_bachelor.course_paper_db.client.service;

import org.jedi_bachelor.course_paper_db.client.entity.*;
import org.jedi_bachelor.course_paper_db.client.repository.*;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Lazy;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.math.BigDecimal;
import java.time.LocalDateTime;
import java.util.List;

@Service
@Transactional
public class BusinessService {
    @Lazy
    @Autowired
    private BookRepository bookRepository;

    @Lazy
    @Autowired
    private OrderRepository orderRepository;

    @Lazy
    @Autowired
    private OrderPositionRepository orderPositionRepository;

    @Lazy
    @Autowired
    private ClientRepository clientRepository;

    @Lazy
    @Autowired
    private AuthorRepository authorRepository;

    @Lazy
    @Autowired
    private GenreRepository genreRepository;

    @Lazy
    @Autowired
    private InventoryRepository inventoryRepository;

    @Lazy
    @Autowired
    private ResponseRepository responseRepository;

    @Lazy
    @Autowired
    private PromoCodeRepository promoCodeRepository;

    @Lazy
    @Autowired
    private DeliveryRepository deliveryRepository;

    public List<Object[]> searchBooks(String searchTerm, Long genreId, Long authorId,
                                      BigDecimal minPrice, BigDecimal maxPrice,
                                      Integer yearFrom, Integer yearTo,
                                      Integer pageNumber, Integer pageSize) {
        return bookRepository.searchBooks(searchTerm, genreId, authorId,
                minPrice, maxPrice, yearFrom, yearTo, pageNumber, pageSize);
    }

    public List<Object[]> getBooksByAuthor(Long authorId) {
        return bookRepository.getBooksByAuthor(authorId);
    }

    public Long createOrder(Long customerId, String shippingAddress, String promoCode) {
        return orderRepository.createOrder(customerId, shippingAddress, promoCode);
    }

    public Long addItemToOrder(Long orderId, Long bookId, Integer quantity) {
        return orderPositionRepository.addItemToOrder(orderId, bookId, quantity);
    }

    public void removeItemFromOrder(Long orderItemId) {
        orderPositionRepository.removeItemFromOrder(orderItemId);
    }

    public void updateOrderItemQuantity(Long orderItemId, Integer newQuantity) {
        orderPositionRepository.updateOrderItemQuantity(orderItemId, newQuantity);
    }

    public void changeOrderStatus(Long orderId, String newStatus) {
        orderRepository.changeOrderStatus(orderId, newStatus);
    }

    public void applyPromocodeToOrder(Long orderId, String promoCode) {
        orderRepository.applyPromocodeToOrder(orderId, promoCode);
    }

    public void cancelOrder(Long orderId) {
        orderRepository.cancelOrder(orderId);
    }

    public List<Object[]> getCustomerOrders(Long customerId) {
        return orderRepository.getCustomerOrders(customerId);
    }

    public List<Object[]> trackOrder(Long orderId) {
        return orderRepository.trackOrder(orderId);
    }

    public void addLoyaltyPoints(Long customerId, Integer pointsAmount) {
        clientRepository.addLoyaltyPoints(customerId, pointsAmount);
    }

    public void spendLoyaltyPoints(Long customerId, Integer pointsAmount) {
        clientRepository.spendLoyaltyPoints(customerId, pointsAmount);
    }

    public void linkAuthorToBook(Long bookId, Long authorId, Integer authorOrder) {
        authorRepository.linkAuthorToBook(bookId, authorId, authorOrder);
    }

    public void unlinkAuthorFromBook(Long bookId, Long authorId) {
        authorRepository.unlinkAuthorFromBook(bookId, authorId);
    }

    public List<Object[]> mergeAuthors(Long sourceAuthorId, Long targetAuthorId) {
        return authorRepository.mergeAuthors(sourceAuthorId, targetAuthorId);
    }

    public List<Object[]> updateInventoryStock(Long bookId, Integer quantityChange) {
        return inventoryRepository.updateInventoryStock(bookId, quantityChange);
    }

    public List<Object[]> getLowStockBooks(Integer threshold) {
        return inventoryRepository.getLowStockBooks(threshold);
    }

    public List<Object[]> getInventoryValue() {
        return inventoryRepository.getInventoryValue();
    }

    public Long addReview(Long customerId, Long bookId, Integer rating, String comment) {
        return responseRepository.addReview(customerId, bookId, rating, comment);
    }

    public List<Object[]> getBookRating(Long bookId) {
        return responseRepository.getBookRating(bookId);
    }

    public List<Object[]> validatePromocode(String code, BigDecimal orderAmount) {
        return promoCodeRepository.validatePromocode(code, orderAmount);
    }

    public Long createShipment(Long orderId, String carrier, String trackingNumber) {
        return deliveryRepository.createShipment(orderId, carrier, trackingNumber);
    }

    public void updateShipmentStatus(Long shipmentId, String newStatus, LocalDateTime deliveredDate) {
        deliveryRepository.updateShipmentStatus(shipmentId, newStatus, deliveredDate);
    }

    public List<Object[]> getSalesReport(java.time.LocalDate dateFrom,
                                         java.time.LocalDate dateTo,
                                         Long genreId) {
        // Нужно добавить метод в OrderRepository
        return orderRepository.getSalesReport(dateFrom, dateTo, genreId);
    }

    public List<Object[]> getTopCustomers(Integer limitCount) {
        // Нужно добавить метод в ClientRepository
        return clientRepository.getTopCustomers(limitCount);
    }

    public List<Object[]> getRevenueByGenre(java.time.LocalDate dateFrom,
                                            java.time.LocalDate dateTo) {
        // Нужно добавить метод в OrderRepository
        return orderRepository.getRevenueByGenre(dateFrom, dateTo);
    }

    public List<Object[]> getDailyStats(java.time.LocalDate reportDate) {
        // Нужно добавить метод в OrderRepository
        return orderRepository.getDailyStats(reportDate);
    }

    public List<Object[]> bulkUpdatePrices(Long genreId, Long publisherId, BigDecimal percentageChange) {
        return bookRepository.bulkUpdatePrices(genreId, publisherId, percentageChange);
    }

    public List<Object[]> processOrderWorkflow(Long orderId) {
        return orderRepository.processOrderWorkflow(orderId);
    }
}
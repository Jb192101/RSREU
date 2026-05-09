package org.jedi_bachelor.course_paper_db.client.service;

import lombok.RequiredArgsConstructor;
import org.jedi_bachelor.course_paper_db.client.entity.*;
import org.jedi_bachelor.course_paper_db.client.repository.*;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.List;
import java.util.Optional;

@Service
@RequiredArgsConstructor
@Transactional
public class RepositoryService {
    private final AuthorRepository authorRepository;

    private final BookAuthorRepository bookAuthorRepository;

    private final InventoryRepository bookRemnantRepository;

    private final BookRepository bookRepository;

    private final ClientRepository clientRepository;

    private final DeliveryRepository deliveryRepository;

    private final GenreRepository genreRepository;

    private final OrderPositionRepository orderPositionRepository;

    private final OrderRepository orderRepository;

    private final PromoCodeRepository promoCodeRepository;

    private final PublisherRepository publisherRepository;

    private final ResponseRepository responseRepository;

    // ========== Authors ==========

    public Author createAuthor(Author author) {
        return this.authorRepository.save(author);
    }

    public Optional<Author> getAuthorById(Long id) {
        return this.authorRepository.findAuthorById(id);
    }

    public List<Author> getAllAuthors() {
        return this.authorRepository.findAllAuthors();
    }

    public void updateAuthor(Author author) {
        this.authorRepository.updateAuthor(author);
    }

    public void deleteAuthor(Long id) {
        this.authorRepository.deleteAuthorById(id);
    }

    // ========== Books ==========

    public Book createBook(Book book) {
        return this.bookRepository.save(book);
    }

    public Optional<Book> getBookById(Long id) {
        return this.bookRepository.findBookById(id);
    }

    public List<Book> getAllBooks() {
        return this.bookRepository.findAllBooks();
    }

    public void updateBook(Book book) {
        this.bookRepository.updateBook(book);
    }

    public void deleteBook(Long id) {
        this.bookRepository.deleteBookById(id);
    }

    // ========== Clients ==========

    public Client createClient(Client client) {
        return this.clientRepository.save(client);
    }

    public Optional<Client> getClientById(Long id) {
        return this.clientRepository.findClientById(id);
    }

    public List<Client> getAllClients() {
        return this.clientRepository.findAllClients();
    }

    public void updateClient(Client client) {
        this.clientRepository.updateClient(client);
    }

    public void deleteClient(Long id) {
        this.clientRepository.deleteClientById(id);
    }

    // ========== BookAuthors ==========

    public BookAuthor createBookAuthor(BookAuthor bookAuthor) {
        return this.bookAuthorRepository.save(bookAuthor);
    }

    public Optional<BookAuthor> getBookAuthorById(BookAuthor.BookAuthorId id) {
        return this.bookAuthorRepository.findById(id);
    }

    public List<BookAuthor> getAllBookAuthors() {
        return this.bookAuthorRepository.findAll();
    }

    public void updateBookAuthor(BookAuthor bookAuthor) {
        this.bookAuthorRepository.save(bookAuthor);
    }

    public void deleteBookAuthor(BookAuthor.BookAuthorId id) {
        this.bookAuthorRepository.deleteById(id);
    }

    // ========== BookRemnants (Inventory) ==========

    public BookRemnant createBookRemnant(BookRemnant bookRemnant) {
        return this.bookRemnantRepository.save(bookRemnant);
    }

    public Optional<BookRemnant> getBookRemnantById(Long id) {
        return this.bookRemnantRepository.findById(id);
    }

    public List<BookRemnant> getAllBookRemnants() {
        return this.bookRemnantRepository.findAll();
    }

    public void updateBookRemnant(BookRemnant bookRemnant) {
        this.bookRemnantRepository.save(bookRemnant);
    }

    public void deleteBookRemnant(Long id) {
        this.bookRemnantRepository.deleteById(id);
    }

    // ========== Genres ==========

    public Genre createGenre(Genre genre) {
        return this.genreRepository.save(genre);
    }

    public Optional<Genre> getGenreById(Long id) {
        return this.genreRepository.findById(id);
    }

    public List<Genre> getAllGenres() {
        return this.genreRepository.findAll();
    }

    public void updateGenre(Genre genre) {
        this.genreRepository.save(genre);
    }

    public void deleteGenre(Long id) {
        this.genreRepository.deleteById(id);
    }

    // ========== Orders ==========

    public Long createNewOrder(Long customerId, String shippingAddress, String promoCode) {
        return this.orderRepository.createOrder(customerId, shippingAddress, promoCode);
    }

    public Optional<Order> getOrderById(Long id) {
        return this.orderRepository.findById(id);
    }

    public List<Order> getAllOrders() {
        return this.orderRepository.findAll();
    }

    public void updateOrder(Order order) {
        this.orderRepository.save(order);
    }

    public void deleteOrder(Long id) {
        this.orderRepository.deleteById(id);
    }

    // ========== OrderPositions ==========

    public OrderPosition createOrderPosition(OrderPosition orderPosition) {
        return this.orderPositionRepository.save(orderPosition);
    }

    public Optional<OrderPosition> getOrderPositionById(Long id) {
        return this.orderPositionRepository.findById(id);
    }

    public List<OrderPosition> getAllOrderPositions() {
        return this.orderPositionRepository.findAll();
    }

    public void updateOrderPosition(OrderPosition orderPosition) {
        this.orderPositionRepository.save(orderPosition);
    }

    public void deleteOrderPosition(Long id) {
        this.orderPositionRepository.deleteById(id);
    }

    // ========== PromoCodes ==========

    public PromoCode createPromoCode(PromoCode promoCode) {
        return this.promoCodeRepository.save(promoCode);
    }

    public Optional<PromoCode> getPromoCodeById(Long id) {
        return this.promoCodeRepository.findById(id);
    }

    public List<PromoCode> getAllPromoCodes() {
        return this.promoCodeRepository.findAll();
    }

    public void updatePromoCode(PromoCode promoCode) {
        this.promoCodeRepository.save(promoCode);
    }

    public void deletePromoCode(Long id) {
        this.promoCodeRepository.deleteById(id);
    }

    // ========== Publishers ==========

    public Publisher createPublisher(Publisher publisher) {
        return this.publisherRepository.save(publisher);
    }

    public Optional<Publisher> getPublisherById(Long id) {
        return this.publisherRepository.findById(id);
    }

    public List<Publisher> getAllPublishers() {
        return this.publisherRepository.findAll();
    }

    public void updatePublisher(Publisher publisher) {
        this.publisherRepository.save(publisher);
    }

    public void deletePublisher(Long id) {
        this.publisherRepository.deleteById(id);
    }

    // ========== Responses (Reviews) ==========

    public Response createResponse(Response response) {
        return this.responseRepository.save(response);
    }

    public Optional<Response> getResponseById(Long id) {
        return this.responseRepository.findById(id);
    }

    public List<Response> getAllResponses() {
        return this.responseRepository.findAll();
    }

    public void updateResponse(Response response) {
        this.responseRepository.save(response);
    }

    public void deleteResponse(Long id) {
        this.responseRepository.deleteById(id);
    }

    // ========== Deliveries ==========

    public Delivery createDelivery(Delivery delivery) {
        return this.deliveryRepository.save(delivery);
    }

    public Optional<Delivery> getDeliveryById(Long id) {
        return this.deliveryRepository.findById(id);
    }

    public List<Delivery> getAllDeliveries() {
        return this.deliveryRepository.findAll();
    }

    public void updateDelivery(Delivery delivery) {
        this.deliveryRepository.save(delivery);
    }

    public void deleteDelivery(Long id) {
        this.deliveryRepository.deleteById(id);
    }
}
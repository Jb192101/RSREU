package org.jedi_bachelor.course_paper_db.client.entity;

import jakarta.persistence.*;
import lombok.Data;
import lombok.EqualsAndHashCode;
import lombok.ToString;

import java.io.Serializable;

@Entity
@Table(name = "book_authors")
@IdClass(BookAuthor.BookAuthorId.class)
@Data
public class BookAuthor {
    @Id
    @ManyToOne(fetch = FetchType.EAGER)
    @JoinColumn(name = "book_id", nullable = false)
    private Book book;

    @Id
    @ManyToOne(fetch = FetchType.EAGER)
    @ToString.Exclude
    @JoinColumn(name = "author_id", nullable = false)
    private Author author;

    @Column(name = "author_order", nullable = false)
    private Integer authorOrder = 1;

    @EqualsAndHashCode
    public static class BookAuthorId implements Serializable {
        private Long book;
        private Long author;
    }
}
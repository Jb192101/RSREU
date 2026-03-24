package org.jedi_bachelor.course_paper_db.entity;

import jakarta.persistence.*;
import lombok.Data;
import org.jedi_bachelor.course_paper_db.entity.keys.BookAuthorKey;

@Entity
@Table(name = "book_authors")
@IdClass(BookAuthorKey.class) // составной ключ (book_id, author_id)
@Data
public class BookAuthorRelation {
    @Id
    private Book book;

    @Id
    private Author author;

    @Column(name = "author_order")
    private Integer authorOrder; // Порядок автора (если авторов у книг несколько)
}

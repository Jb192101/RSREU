package org.jedi_bachelor.course_paper_db.entity.keys;

import lombok.Data;
import org.jedi_bachelor.course_paper_db.entity.Author;
import org.jedi_bachelor.course_paper_db.entity.Book;

@Data
public class BookAuthorKey {
    public Book book;
    private Author author;
}

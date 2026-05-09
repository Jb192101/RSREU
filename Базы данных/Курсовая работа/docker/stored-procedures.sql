-- ============================================================
-- ХРАНИМЫЕ ПРОЦЕДУРЫ ДЛЯ БАЗЫ ДАННЫХ «КНИЖНЫЙ МАГАЗИН»
-- Версия PostgreSQL 15+
-- ============================================================

-- ============================================================
-- МОДУЛЬ 1: ПОИСК И ФИЛЬТРАЦИЯ КНИГ
-- ============================================================

-- 1. Комплексный поиск книг
CREATE OR REPLACE FUNCTION sp_search_books(
    p_search_term VARCHAR DEFAULT NULL,
    p_genre_id BIGINT DEFAULT NULL,
    p_author_id BIGINT DEFAULT NULL,
    p_min_price DECIMAL(10,2) DEFAULT NULL,
    p_max_price DECIMAL(10,2) DEFAULT NULL,
    p_year_from INTEGER DEFAULT NULL,
    p_year_to INTEGER DEFAULT NULL,
    p_page_number INTEGER DEFAULT 1,
    p_page_size INTEGER DEFAULT 20
)
RETURNS TABLE(
    book_id BIGINT,
    isbn VARCHAR,
    title VARCHAR,
    publication_year INTEGER,
    price DECIMAL(10,2),
    pages INTEGER,
    description TEXT,
    publisher_name VARCHAR,
    genre_name VARCHAR,
    authors TEXT,
    available_quantity INTEGER,
    avg_rating DECIMAL(3,2),
    total_reviews INTEGER
) AS $$
DECLARE
    v_offset INTEGER;
BEGIN
    v_offset := (p_page_number - 1) * p_page_size;

    RETURN QUERY
    SELECT
        b.book_id,
        b.isbn,
        b.title,
        b.publication_year,
        b.price,
        b.pages,
        b.description,
        p.name AS publisher_name,
        g.name AS genre_name,
        STRING_AGG(a.first_name || ' ' || a.last_name, ', ' ORDER BY ba.author_order) AS authors,
        (i.quantity - i.reserved_quantity) AS available_quantity,
        ROUND(AVG(r.rating)::DECIMAL, 2) AS avg_rating,
        COUNT(DISTINCT r.review_id)::INTEGER AS total_reviews
    FROM books b
    JOIN publishers p ON b.publisher_id = p.publisher_id
    JOIN genres g ON b.genre_id = g.genre_id
    LEFT JOIN book_authors ba ON b.book_id = ba.book_id
    LEFT JOIN authors a ON ba.author_id = a.author_id
    LEFT JOIN inventory i ON b.book_id = i.book_id
    LEFT JOIN reviews r ON b.book_id = r.book_id
    WHERE
        (p_search_term IS NULL OR
         b.title ILIKE '%' || p_search_term || '%' OR
         b.isbn ILIKE '%' || p_search_term || '%')
        AND (p_genre_id IS NULL OR b.genre_id = p_genre_id)
        AND (p_author_id IS NULL OR EXISTS (
            SELECT 1 FROM book_authors ba2
            WHERE ba2.book_id = b.book_id AND ba2.author_id = p_author_id
        ))
        AND (p_min_price IS NULL OR b.price >= p_min_price)
        AND (p_max_price IS NULL OR b.price <= p_max_price)
        AND (p_year_from IS NULL OR b.publication_year >= p_year_from)
        AND (p_year_to IS NULL OR b.publication_year <= p_year_to)
    GROUP BY b.book_id, b.isbn, b.title, b.publication_year, b.price,
             b.pages, b.description, p.name, g.name, i.quantity, i.reserved_quantity
    ORDER BY b.title
    LIMIT p_page_size
    OFFSET v_offset;
END;
$$ LANGUAGE plpgsql;

-- 2. Получение книг автора с учётом соавторства
CREATE OR REPLACE FUNCTION sp_get_books_by_author(
    p_author_id BIGINT
)
RETURNS TABLE(
    book_id BIGINT,
    title VARCHAR,
    isbn VARCHAR,
    publication_year INTEGER,
    price DECIMAL(10,2),
    author_order INTEGER,
    total_authors BIGINT,
    genre_name VARCHAR,
    publisher_name VARCHAR
) AS $$
BEGIN
    RETURN QUERY
    SELECT
        b.book_id,
        b.title,
        b.isbn,
        b.publication_year,
        b.price,
        ba.author_order,
        COUNT(ba2.author_id) OVER (PARTITION BY b.book_id) AS total_authors,
        g.name AS genre_name,
        p.name AS publisher_name
    FROM books b
    JOIN book_authors ba ON b.book_id = ba.book_id
    JOIN genres g ON b.genre_id = g.genre_id
    JOIN publishers p ON b.publisher_id = p.publisher_id
    LEFT JOIN book_authors ba2 ON b.book_id = ba2.book_id
    WHERE ba.author_id = p_author_id
    ORDER BY b.publication_year DESC, ba.author_order;
END;
$$ LANGUAGE plpgsql;

-- ============================================================
-- МОДУЛЬ 2: УПРАВЛЕНИЕ СВЯЗЯМИ АВТОРОВ И КНИГ
-- ============================================================

-- 3. Привязка автора к книге
CREATE OR REPLACE FUNCTION sp_link_author_to_book(
    p_book_id BIGINT,
    p_author_id BIGINT,
    p_author_order INTEGER DEFAULT NULL
)
RETURNS VOID AS $$
DECLARE
    v_next_order INTEGER;
    v_existing_count INTEGER;
BEGIN
    -- Проверяем существование книги
    IF NOT EXISTS (SELECT 1 FROM books WHERE book_id = p_book_id) THEN
        RAISE EXCEPTION 'Книга с ID % не найдена', p_book_id;
    END IF;

    -- Проверяем существование автора
    IF NOT EXISTS (SELECT 1 FROM authors WHERE author_id = p_author_id) THEN
        RAISE EXCEPTION 'Автор с ID % не найден', p_author_id;
    END IF;

    -- Проверяем, что связь ещё не существует
    SELECT COUNT(*) INTO v_existing_count
    FROM book_authors
    WHERE book_id = p_book_id AND author_id = p_author_id;

    IF v_existing_count > 0 THEN
        RAISE EXCEPTION 'Автор (ID: %) уже привязан к книге (ID: %)', p_author_id, p_book_id;
    END IF;

    -- Если порядковый номер не указан, назначаем следующий
    IF p_author_order IS NULL THEN
        SELECT COALESCE(MAX(author_order), 0) + 1 INTO v_next_order
        FROM book_authors
        WHERE book_id = p_book_id;
    ELSE
        -- Проверяем, что указанный номер не занят
        IF EXISTS (
            SELECT 1 FROM book_authors
            WHERE book_id = p_book_id AND author_order = p_author_order
        ) THEN
            RAISE EXCEPTION 'У книги (ID: %) уже есть автор с порядковым номером %',
                p_book_id, p_author_order;
        END IF;
        v_next_order := p_author_order;
    END IF;

    -- Создаём связь
    INSERT INTO book_authors (book_id, author_id, author_order)
    VALUES (p_book_id, p_author_id, v_next_order);

END;
$$ LANGUAGE plpgsql;

-- 4. Отвязка автора от книги
CREATE OR REPLACE FUNCTION sp_unlink_author_from_book(
    p_book_id BIGINT,
    p_author_id BIGINT
)
RETURNS VOID AS $$
DECLARE
    v_deleted_order INTEGER;
BEGIN
    -- Получаем порядковый номер удаляемого автора
    SELECT author_order INTO v_deleted_order
    FROM book_authors
    WHERE book_id = p_book_id AND author_id = p_author_id;

    IF NOT FOUND THEN
        RAISE EXCEPTION 'Связь между книгой (ID: %) и автором (ID: %) не найдена',
            p_book_id, p_author_id;
    END IF;

    -- Удаляем связь
    DELETE FROM book_authors
    WHERE book_id = p_book_id AND author_id = p_author_id;

    -- Пересчитываем порядковые номера оставшихся авторов
    UPDATE book_authors ba
    SET author_order = new_order.new_author_order
    FROM (
        SELECT
            ba2.book_id,
            ba2.author_id,
            ROW_NUMBER() OVER (ORDER BY ba2.author_order) AS new_author_order
        FROM book_authors ba2
        WHERE ba2.book_id = p_book_id
    ) new_order
    WHERE ba.book_id = new_order.book_id
      AND ba.author_id = new_order.author_id;

END;
$$ LANGUAGE plpgsql;

-- 6. Создание заказа
CREATE OR REPLACE FUNCTION sp_create_order(
    p_customer_id BIGINT,
    p_shipping_address TEXT,
    p_promo_code VARCHAR DEFAULT NULL
)
RETURNS BIGINT AS $$
DECLARE
    v_order_id BIGINT;
    v_promo_code_id BIGINT;
BEGIN
    -- Проверяем существование клиента
    IF NOT EXISTS (SELECT 1 FROM customers WHERE customer_id = p_customer_id) THEN
        RAISE EXCEPTION 'Клиент с ID % не найден', p_customer_id;
    END IF;

    -- Если передан промокод, ищем его
    IF p_promo_code IS NOT NULL THEN
        SELECT promo_code_id INTO v_promo_code_id
        FROM promocodes
        WHERE code = p_promo_code;

        IF NOT FOUND THEN
            RAISE EXCEPTION 'Промокод "%" не найден', p_promo_code;
        END IF;
    END IF;

    -- Создаём заказ
    INSERT INTO orders (
        customer_id,
        shipping_address,
        promo_code_id,
        status,
        total_amount,
        discount_amount
    )
    VALUES (
        p_customer_id,
        p_shipping_address,
        v_promo_code_id,
        'ожидает оплаты',
        0,
        0
    )
    RETURNING order_id INTO v_order_id;

    -- Если указан промокод, проверяем минимальную сумму (пока 0, но промокод применится позже)
    IF v_promo_code_id IS NOT NULL THEN
        PERFORM sp_apply_promocode_to_order(v_order_id, p_promo_code);
    END IF;

    RETURN v_order_id;
END;
$$ LANGUAGE plpgsql;

-- 7. Добавление книги в заказ
CREATE OR REPLACE FUNCTION sp_add_item_to_order(
    p_order_id BIGINT,
    p_book_id BIGINT,
    p_quantity INTEGER
)
RETURNS BIGINT AS $$
DECLARE
    v_order_status VARCHAR(50);
    v_current_price DECIMAL(10,2);
    v_available INTEGER;
    v_order_item_id BIGINT;
BEGIN
    -- Проверяем статус заказа
    SELECT status INTO v_order_status
    FROM orders
    WHERE order_id = p_order_id;

    IF NOT FOUND THEN
        RAISE EXCEPTION 'Заказ с ID % не найден', p_order_id;
    END IF;

    IF v_order_status != 'ожидает оплаты' THEN
        RAISE EXCEPTION 'Нельзя изменить заказ в статусе "%". Редактирование возможно только в статусе "ожидает оплаты"',
            v_order_status;
    END IF;

    -- Проверяем существование книги и получаем цену
    SELECT price INTO v_current_price
    FROM books
    WHERE book_id = p_book_id;

    IF NOT FOUND THEN
        RAISE EXCEPTION 'Книга с ID % не найдена', p_book_id;
    END IF;

    -- Проверяем доступность на складе
    SELECT (quantity - reserved_quantity) INTO v_available
    FROM inventory
    WHERE book_id = p_book_id;

    IF NOT FOUND THEN
        RAISE EXCEPTION 'Книга с ID % отсутствует на складе', p_book_id;
    END IF;

    IF p_quantity > v_available THEN
        RAISE EXCEPTION 'Недостаточно книг на складе. Запрошено: %, доступно: %',
            p_quantity, v_available;
    END IF;

    -- Добавляем позицию в заказ
    INSERT INTO order_items (order_id, book_id, quantity, unit_price)
    VALUES (p_order_id, p_book_id, p_quantity, v_current_price)
    RETURNING order_item_id INTO v_order_item_id;

    -- Резервируем книги на складе
    UPDATE inventory
    SET reserved_quantity = reserved_quantity + p_quantity
    WHERE book_id = p_book_id;

    -- Пересчитываем сумму заказа
    PERFORM sp_calculate_order_total(p_order_id);

    RETURN v_order_item_id;
END;
$$ LANGUAGE plpgsql;

-- 8. Удаление книги из заказа
CREATE OR REPLACE FUNCTION sp_remove_item_from_order(
    p_order_item_id BIGINT
)
RETURNS VOID AS $$
DECLARE
    v_order_id BIGINT;
    v_order_status VARCHAR(50);
    v_book_id BIGINT;
    v_quantity INTEGER;
BEGIN
    -- Получаем информацию о позиции заказа
    SELECT oi.order_id, oi.book_id, oi.quantity, o.status
    INTO v_order_id, v_book_id, v_quantity, v_order_status
    FROM order_items oi
    JOIN orders o ON oi.order_id = o.order_id
    WHERE oi.order_item_id = p_order_item_id;

    IF NOT FOUND THEN
        RAISE EXCEPTION 'Позиция заказа с ID % не найдена', p_order_item_id;
    END IF;

    IF v_order_status != 'ожидает оплаты' THEN
        RAISE EXCEPTION 'Нельзя изменить заказ в статусе "%". Редактирование возможно только в статусе "ожидает оплаты"',
            v_order_status;
    END IF;

    -- Возвращаем резерв на склад
    UPDATE inventory
    SET reserved_quantity = reserved_quantity - v_quantity
    WHERE book_id = v_book_id;

    -- Удаляем позицию
    DELETE FROM order_items
    WHERE order_item_id = p_order_item_id;

    -- Пересчитываем сумму заказа
    PERFORM sp_calculate_order_total(v_order_id);
END;
$$ LANGUAGE plpgsql;

-- 9. Изменение количества экземпляров в позиции
CREATE OR REPLACE FUNCTION sp_update_order_item_quantity(
    p_order_item_id BIGINT,
    p_new_quantity INTEGER
)
RETURNS VOID AS $$
DECLARE
    v_order_id BIGINT;
    v_order_status VARCHAR(50);
    v_book_id BIGINT;
    v_old_quantity INTEGER;
    v_available INTEGER;
    v_quantity_difference INTEGER;
BEGIN
    -- Получаем информацию о позиции
    SELECT oi.order_id, oi.book_id, oi.quantity, o.status
    INTO v_order_id, v_book_id, v_old_quantity, v_order_status
    FROM order_items oi
    JOIN orders o ON oi.order_id = o.order_id
    WHERE oi.order_item_id = p_order_item_id;

    IF NOT FOUND THEN
        RAISE EXCEPTION 'Позиция заказа с ID % не найдена', p_order_item_id;
    END IF;

    IF v_order_status != 'ожидает оплаты' THEN
        RAISE EXCEPTION 'Нельзя изменить заказ в статусе "%"', v_order_status;
    END IF;

    -- Вычисляем разницу в количестве
    v_quantity_difference := p_new_quantity - v_old_quantity;

    IF v_quantity_difference > 0 THEN
        -- Нужно больше книг - проверяем доступность
        SELECT (quantity - reserved_quantity) INTO v_available
        FROM inventory
        WHERE book_id = v_book_id;

        IF v_quantity_difference > v_available THEN
            RAISE EXCEPTION 'Недостаточно книг на складе. Дополнительно запрошено: %, доступно: %',
                v_quantity_difference, v_available;
        END IF;
    END IF;

    -- Обновляем количество в позиции
    UPDATE order_items
    SET quantity = p_new_quantity
    WHERE order_item_id = p_order_item_id;

    -- Обновляем резерв на складе
    UPDATE inventory
    SET reserved_quantity = reserved_quantity + v_quantity_difference
    WHERE book_id = v_book_id;

    -- Пересчитываем сумму заказа
    PERFORM sp_calculate_order_total(v_order_id);
END;
$$ LANGUAGE plpgsql;

-- 10. Изменение статуса заказа
CREATE OR REPLACE FUNCTION sp_change_order_status(
    p_order_id BIGINT,
    p_new_status VARCHAR(50)
)
RETURNS VOID AS $$
DECLARE
    v_old_status VARCHAR(50);
    v_item RECORD;
BEGIN
    -- Получаем текущий статус
    SELECT status INTO v_old_status
    FROM orders
    WHERE order_id = p_order_id;

    IF NOT FOUND THEN
        RAISE EXCEPTION 'Заказ с ID % не найден', p_order_id;
    END IF;

    -- Проверяем допустимость перехода (дублирует триггер для информативности)
    CASE v_old_status
        WHEN 'ожидает оплаты' THEN
            IF p_new_status NOT IN ('оплачен', 'отменён') THEN
                RAISE EXCEPTION 'Недопустимый переход: % -> %', v_old_status, p_new_status;
            END IF;
        WHEN 'оплачен' THEN
            IF p_new_status NOT IN ('обрабатывается', 'отменён') THEN
                RAISE EXCEPTION 'Недопустимый переход: % -> %', v_old_status, p_new_status;
            END IF;
        WHEN 'обрабатывается' THEN
            IF p_new_status NOT IN ('подтверждён', 'отменён') THEN
                RAISE EXCEPTION 'Недопустимый переход: % -> %', v_old_status, p_new_status;
            END IF;
        WHEN 'подтверждён' THEN
            IF p_new_status NOT IN ('отправлен', 'отменён') THEN
                RAISE EXCEPTION 'Недопустимый переход: % -> %', v_old_status, p_new_status;
            END IF;
        WHEN 'отправлен' THEN
            IF p_new_status NOT IN ('доставлен', 'возвращён') THEN
                RAISE EXCEPTION 'Недопустимый переход: % -> %', v_old_status, p_new_status;
            END IF;
        WHEN 'доставлен' THEN
            IF p_new_status != 'возвращён' THEN
                RAISE EXCEPTION 'Недопустимый переход: % -> %', v_old_status, p_new_status;
            END IF;
        WHEN 'отменён', 'возвращён' THEN
            RAISE EXCEPTION 'Статус "%" является конечным и не может быть изменён', v_old_status;
        ELSE
            RAISE EXCEPTION 'Неизвестный статус: %', v_old_status;
    END CASE;

    -- Обрабатываем складские операции в зависимости от нового статуса
    IF p_new_status = 'отменён' AND v_old_status IN ('ожидает оплаты', 'оплачен', 'обрабатывается') THEN
        -- Возвращаем резерв на склад
        FOR v_item IN
            SELECT book_id, quantity
            FROM order_items
            WHERE order_id = p_order_id
        LOOP
            UPDATE inventory
            SET reserved_quantity = reserved_quantity - v_item.quantity
            WHERE book_id = v_item.book_id;
        END LOOP;

    ELSIF p_new_status = 'подтверждён' THEN
        -- Списываем книги со склада
        FOR v_item IN
            SELECT book_id, quantity
            FROM order_items
            WHERE order_id = p_order_id
        LOOP
            UPDATE inventory
            SET quantity = quantity - v_item.quantity,
                reserved_quantity = reserved_quantity - v_item.quantity
            WHERE book_id = v_item.book_id;
        END LOOP;

    ELSIF p_new_status = 'возвращён' AND v_old_status IN ('отправлен', 'доставлен') THEN
        -- Возвращаем книги на склад
        FOR v_item IN
            SELECT book_id, quantity
            FROM order_items
            WHERE order_id = p_order_id
        LOOP
            UPDATE inventory
            SET quantity = quantity + v_item.quantity
            WHERE book_id = v_item.book_id;
        END LOOP;
    END IF;

    -- Обновляем статус заказа
    UPDATE orders
    SET status = p_new_status
    WHERE order_id = p_order_id;

    -- Логируем изменение статуса
    INSERT INTO audit_log (table_name, record_id, operation, old_data, new_data)
    VALUES ('orders', p_order_id, 'STATUS_CHANGE',
            jsonb_build_object('status', v_old_status),
            jsonb_build_object('status', p_new_status));

END;
$$ LANGUAGE plpgsql;

-- 11. Применение/замена промокода к заказу
CREATE OR REPLACE FUNCTION sp_apply_promocode_to_order(
    p_order_id BIGINT,
    p_promo_code VARCHAR
)
RETURNS VOID AS $$
DECLARE
    v_order_status VARCHAR(50);
    v_total_amount DECIMAL(10,2);
    v_old_promo_code_id BIGINT;
    v_new_promo_code_id BIGINT;
    v_promo_record RECORD;
    v_discount DECIMAL(10,2);
BEGIN
    -- Получаем информацию о заказе
    SELECT status, total_amount, promo_code_id
    INTO v_order_status, v_total_amount, v_old_promo_code_id
    FROM orders
    WHERE order_id = p_order_id;

    IF NOT FOUND THEN
        RAISE EXCEPTION 'Заказ с ID % не найден', p_order_id;
    END IF;

    IF v_order_status != 'ожидает оплаты' THEN
        RAISE EXCEPTION 'Нельзя применить промокод к заказу в статусе "%"', v_order_status;
    END IF;

    -- Ищем новый промокод
    SELECT * INTO v_promo_record
    FROM promocodes
    WHERE code = p_promo_code;

    IF NOT FOUND THEN
        RAISE EXCEPTION 'Промокод "%" не найден', p_promo_code;
    END IF;

    -- Проверяем срок действия
    IF CURRENT_DATE < v_promo_record.valid_from OR CURRENT_DATE > v_promo_record.valid_to THEN
        RAISE EXCEPTION 'Срок действия промокода истёк или ещё не начался';
    END IF;

    -- Проверяем лимит использований
    IF v_promo_record.max_uses IS NOT NULL AND v_promo_record.used_count >= v_promo_record.max_uses THEN
        RAISE EXCEPTION 'Лимит использований промокода исчерпан';
    END IF;

    -- Проверяем минимальную сумму заказа
    IF v_promo_record.min_order_amount > 0 AND v_total_amount < v_promo_record.min_order_amount THEN
        RAISE EXCEPTION 'Сумма заказа (%) меньше минимальной для применения промокода (%)',
            v_total_amount, v_promo_record.min_order_amount;
    END IF;

    -- Рассчитываем скидку
    IF v_promo_record.discount_type = 'PERCENT' THEN
        v_discount := v_total_amount * v_promo_record.discount_value / 100;
    ELSE
        v_discount := v_promo_record.discount_value;
    END IF;

    -- Скидка не может превышать сумму заказа
    IF v_discount > v_total_amount THEN
        v_discount := v_total_amount;
    END IF;

    -- Если был старый промокод, уменьшаем его счётчик
    IF v_old_promo_code_id IS NOT NULL AND v_old_promo_code_id != v_promo_record.promo_code_id THEN
        UPDATE promocodes
        SET used_count = used_count - 1
        WHERE promo_code_id = v_old_promo_code_id AND used_count > 0;
    END IF;

    -- Применяем новый промокод
    UPDATE orders
    SET promo_code_id = v_promo_record.promo_code_id,
        discount_amount = v_discount
    WHERE order_id = p_order_id;

    -- Увеличиваем счётчик использований
    UPDATE promocodes
    SET used_count = used_count + 1
    WHERE promo_code_id = v_promo_record.promo_code_id;

END;
$$ LANGUAGE plpgsql;

-- 12. Полная отмена заказа
CREATE OR REPLACE FUNCTION sp_cancel_order(
    p_order_id BIGINT
)
RETURNS VOID AS $$
DECLARE
    v_order_status VARCHAR(50);
    v_promo_code_id BIGINT;
    v_item RECORD;
BEGIN
    -- Получаем информацию о заказе
    SELECT status, promo_code_id
    INTO v_order_status, v_promo_code_id
    FROM orders
    WHERE order_id = p_order_id;

    IF NOT FOUND THEN
        RAISE EXCEPTION 'Заказ с ID % не найден', p_order_id;
    END IF;

    -- Проверяем, можно ли отменить заказ
    IF v_order_status IN ('отменён', 'возвращён', 'доставлен') THEN
        RAISE EXCEPTION 'Невозможно отменить заказ в статусе "%"', v_order_status;
    END IF;

    -- Возвращаем резерв на склад для активных заказов
    IF v_order_status IN ('ожидает оплаты', 'оплачен', 'обрабатывается') THEN
        FOR v_item IN
            SELECT book_id, quantity
            FROM order_items
            WHERE order_id = p_order_id
        LOOP
            UPDATE inventory
            SET reserved_quantity = reserved_quantity - v_item.quantity
            WHERE book_id = v_item.book_id;
        END LOOP;
    END IF;

    -- Если был применён промокод, уменьшаем счётчик
    IF v_promo_code_id IS NOT NULL THEN
        UPDATE promocodes
        SET used_count = used_count - 1
        WHERE promo_code_id = v_promo_code_id AND used_count > 0;
    END IF;

    -- Меняем статус заказа
    UPDATE orders
    SET status = 'отменён'
    WHERE order_id = p_order_id;

END;
$$ LANGUAGE plpgsql;

-- 13. Получение заказов клиента со статистикой
CREATE OR REPLACE FUNCTION sp_get_customer_orders(
    p_customer_id BIGINT
)
RETURNS TABLE(
    order_id BIGINT,
    order_date TIMESTAMP,
    status VARCHAR,
    total_amount DECIMAL(10,2),
    discount_amount DECIMAL(10,2),
    final_amount DECIMAL(10,2),
    items_count BIGINT,
    tracking_number VARCHAR,
    carrier VARCHAR
) AS $$
BEGIN
    RETURN QUERY
    SELECT
        o.order_id,
        o.order_date,
        o.status,
        o.total_amount,
        o.discount_amount,
        (o.total_amount - o.discount_amount) AS final_amount,
        COUNT(oi.order_item_id) AS items_count,
        s.tracking_number,
        s.carrier
    FROM orders o
    LEFT JOIN order_items oi ON o.order_id = oi.order_id
    LEFT JOIN shipments s ON o.order_id = s.order_id
    WHERE o.customer_id = p_customer_id
    GROUP BY o.order_id, o.order_date, o.status, o.total_amount,
             o.discount_amount, s.tracking_number, s.carrier
    ORDER BY o.order_date DESC;
END;
$$ LANGUAGE plpgsql;

-- Вспомогательная функция: пересчёт суммы заказа
CREATE OR REPLACE FUNCTION sp_calculate_order_total(
    p_order_id BIGINT
)
RETURNS VOID AS $$
DECLARE
    v_total DECIMAL(10,2);
    v_promo_code_id BIGINT;
    v_discount DECIMAL(10,2) := 0;
    v_promo_record RECORD;
BEGIN
    -- Вычисляем сумму позиций заказа
    SELECT COALESCE(SUM(quantity * unit_price), 0)
    INTO v_total
    FROM order_items
    WHERE order_id = p_order_id;

    -- Получаем информацию о применённом промокоде
    SELECT promo_code_id INTO v_promo_code_id
    FROM orders
    WHERE order_id = p_order_id;

    -- Если есть промокод, пересчитываем скидку
    IF v_promo_code_id IS NOT NULL THEN
        SELECT * INTO v_promo_record
        FROM promocodes
        WHERE promo_code_id = v_promo_code_id;

        IF FOUND THEN
            IF v_promo_record.discount_type = 'PERCENT' THEN
                v_discount := v_total * v_promo_record.discount_value / 100;
            ELSE
                v_discount := v_promo_record.discount_value;
            END IF;

            IF v_discount > v_total THEN
                v_discount := v_total;
            END IF;
        END IF;
    END IF;

    -- Обновляем сумму и скидку
    UPDATE orders
    SET total_amount = v_total,
        discount_amount = v_discount
    WHERE order_id = p_order_id;

END;
$$ LANGUAGE plpgsql;

-- ============================================================
-- МОДУЛЬ 5: БОНУСНЫЕ БАЛЛЫ
-- ============================================================

-- 14. Начисление бонусных баллов
CREATE OR REPLACE FUNCTION sp_add_loyalty_points(
    p_customer_id BIGINT,
    p_points_amount INTEGER
)
RETURNS VOID AS $$
BEGIN
    -- Проверяем существование клиента
    IF NOT EXISTS (SELECT 1 FROM customers WHERE customer_id = p_customer_id) THEN
        RAISE EXCEPTION 'Клиент с ID % не найден', p_customer_id;
    END IF;

    -- Проверяем, что сумма положительная
    IF p_points_amount <= 0 THEN
        RAISE EXCEPTION 'Количество начисляемых баллов должно быть положительным. Передано: %',
            p_points_amount;
    END IF;

    -- Начисляем баллы
    UPDATE customers
    SET loyalty_points = loyalty_points + p_points_amount
    WHERE customer_id = p_customer_id;

    -- Логируем операцию
    INSERT INTO audit_log (table_name, record_id, operation, new_data)
    VALUES ('customers', p_customer_id, 'ADD_POINTS',
            jsonb_build_object('points_added', p_points_amount));

END;
$$ LANGUAGE plpgsql;

-- 15. Списание бонусных баллов
CREATE OR REPLACE FUNCTION sp_spend_loyalty_points(
    p_customer_id BIGINT,
    p_points_amount INTEGER
)
RETURNS VOID AS $$
DECLARE
    v_current_points INTEGER;
BEGIN
    -- Получаем текущий баланс баллов
    SELECT loyalty_points INTO v_current_points
    FROM customers
    WHERE customer_id = p_customer_id;

    IF NOT FOUND THEN
        RAISE EXCEPTION 'Клиент с ID % не найден', p_customer_id;
    END IF;

    -- Проверяем, что сумма положительная
    IF p_points_amount <= 0 THEN
        RAISE EXCEPTION 'Количество списываемых баллов должно быть положительным. Передано: %',
            p_points_amount;
    END IF;

    -- Проверяем достаточность баллов
    IF v_current_points < p_points_amount THEN
        RAISE EXCEPTION 'Недостаточно баллов для списания. Текущий баланс: %, запрошено: %',
            v_current_points, p_points_amount;
    END IF;

    -- Списываем баллы
    UPDATE customers
    SET loyalty_points = loyalty_points - p_points_amount
    WHERE customer_id = p_customer_id;

    -- Логируем операцию
    INSERT INTO audit_log (table_name, record_id, operation, new_data)
    VALUES ('customers', p_customer_id, 'SPEND_POINTS',
            jsonb_build_object('points_spent', p_points_amount));

END;
$$ LANGUAGE plpgsql;

-- ============================================================
-- МОДУЛЬ 6: РАБОТА С ПРОМОКОДАМИ
-- ============================================================

-- 16. Проверка валидности промокода
CREATE OR REPLACE FUNCTION sp_validate_promocode(
    p_code VARCHAR,
    p_order_amount DECIMAL(10,2)
)
RETURNS TABLE(
    is_valid BOOLEAN,
    discount_type VARCHAR,
    discount_value DECIMAL(10,2),
    calculated_discount DECIMAL(10,2),
    error_message TEXT
) AS $$
DECLARE
    v_promo_record RECORD;
    v_calculated_discount DECIMAL(10,2);
BEGIN
    -- Ищем промокод
    SELECT * INTO v_promo_record
    FROM promocodes
    WHERE code = p_code;

    IF NOT FOUND THEN
        RETURN QUERY SELECT false, NULL::VARCHAR, NULL::DECIMAL, NULL::DECIMAL,
            'Промокод не найден'::TEXT;
        RETURN;
    END IF;

    -- Проверяем срок действия
    IF CURRENT_DATE < v_promo_record.valid_from THEN
        RETURN QUERY SELECT false, NULL::VARCHAR, NULL::DECIMAL, NULL::DECIMAL,
            'Срок действия промокода ещё не начался'::TEXT;
        RETURN;
    END IF;

    IF CURRENT_DATE > v_promo_record.valid_to THEN
        RETURN QUERY SELECT false, NULL::VARCHAR, NULL::DECIMAL, NULL::DECIMAL,
            'Срок действия промокода истёк'::TEXT;
        RETURN;
    END IF;

    -- Проверяем лимит использований
    IF v_promo_record.max_uses IS NOT NULL AND v_promo_record.used_count >= v_promo_record.max_uses THEN
        RETURN QUERY SELECT false, NULL::VARCHAR, NULL::DECIMAL, NULL::DECIMAL,
            'Лимит использований промокода исчерпан'::TEXT;
        RETURN;
    END IF;

    -- Проверяем минимальную сумму заказа
    IF v_promo_record.min_order_amount > 0 AND p_order_amount < v_promo_record.min_order_amount THEN
        RETURN QUERY SELECT false, NULL::VARCHAR, NULL::DECIMAL, NULL::DECIMAL,
            'Минимальная сумма заказа для применения промокода: ' || v_promo_record.min_order_amount::TEXT;
        RETURN;
    END IF;

    -- Рассчитываем скидку
    IF v_promo_record.discount_type = 'PERCENT' THEN
        v_calculated_discount := p_order_amount * v_promo_record.discount_value / 100;
    ELSE
        v_calculated_discount := v_promo_record.discount_value;
    END IF;

    IF v_calculated_discount > p_order_amount THEN
        v_calculated_discount := p_order_amount;
    END IF;

    -- Возвращаем успешный результат
    RETURN QUERY SELECT
        true,
        v_promo_record.discount_type,
        v_promo_record.discount_value,
        v_calculated_discount,
        NULL::TEXT;

END;
$$ LANGUAGE plpgsql;

-- ============================================================
-- МОДУЛЬ 7: УПРАВЛЕНИЕ СКЛАДОМ
-- ============================================================

-- 17. Операция со складом (пополнение/списание)
CREATE OR REPLACE FUNCTION sp_update_inventory(
    p_book_id BIGINT,
    p_quantity_change INTEGER
)
RETURNS TABLE(
    quantity INTEGER,
    reserved_quantity INTEGER,
    available_quantity INTEGER,
    last_restocked DATE
) AS $$
DECLARE
    v_current_quantity INTEGER;
    v_current_reserved INTEGER;
BEGIN
    -- Получаем текущие остатки
    SELECT inv.quantity, inv.reserved_quantity
    INTO v_current_quantity, v_current_reserved
    FROM inventory inv
    WHERE inv.book_id = p_book_id;

    IF NOT FOUND THEN
        RAISE EXCEPTION 'Книга с ID % не найдена на складе', p_book_id;
    END IF;

    -- Проверяем, что после списания остаток не станет отрицательным
    IF p_quantity_change < 0 AND (v_current_quantity + p_quantity_change) < 0 THEN
        RAISE EXCEPTION 'Недостаточно книг на складе для списания. Текущий остаток: %, запрошено списание: %',
            v_current_quantity, ABS(p_quantity_change);
    END IF;

    -- Обновляем количество
    UPDATE inventory
    SET quantity = quantity + p_quantity_change,
        last_restocked = CASE
            WHEN p_quantity_change > 0 THEN CURRENT_DATE
            ELSE last_restocked
        END
    WHERE book_id = p_book_id;

    -- Возвращаем обновлённые остатки
    RETURN QUERY
    SELECT
        inv.quantity,
        inv.reserved_quantity,
        (inv.quantity - inv.reserved_quantity)::INTEGER,
        inv.last_restocked
    FROM inventory inv
    WHERE inv.book_id = p_book_id;

END;
$$ LANGUAGE plpgsql;

-- Получение книг с низким остатком
CREATE OR REPLACE FUNCTION sp_get_low_stock_books(
    p_threshold INTEGER DEFAULT 5
)
RETURNS TABLE(
    book_id BIGINT,
    title VARCHAR,
    isbn VARCHAR,
    quantity INTEGER,
    reserved_quantity INTEGER,
    available_quantity INTEGER,
    last_restocked DATE,
    price DECIMAL(10,2),
    genre_name VARCHAR
) AS $$
BEGIN
    RETURN QUERY
    SELECT
        b.book_id,
        b.title,
        b.isbn,
        i.quantity,
        i.reserved_quantity,
        (i.quantity - i.reserved_quantity)::INTEGER AS available_quantity,
        i.last_restocked,
        b.price,
        g.name AS genre_name
    FROM books b
    JOIN inventory i ON b.book_id = i.book_id
    JOIN genres g ON b.genre_id = g.genre_id
    WHERE (i.quantity - i.reserved_quantity) < p_threshold
    ORDER BY (i.quantity - i.reserved_quantity) ASC;
END;
$$ LANGUAGE plpgsql;

-- Добавление отзыва с автоматической проверкой покупки
CREATE OR REPLACE FUNCTION sp_add_review(
    p_customer_id BIGINT,
    p_book_id BIGINT,
    p_rating INTEGER,
    p_comment TEXT DEFAULT NULL
)
RETURNS BIGINT AS $$
DECLARE
    v_has_purchase BOOLEAN;
    v_review_id BIGINT;
    v_existing_count INTEGER;
BEGIN
    -- Проверка существования клиента
    IF NOT EXISTS (SELECT 1 FROM customers WHERE customer_id = p_customer_id) THEN
        RAISE EXCEPTION 'Клиент с ID % не найден', p_customer_id;
    END IF;

    -- Проверка существования книги
    IF NOT EXISTS (SELECT 1 FROM books WHERE book_id = p_book_id) THEN
        RAISE EXCEPTION 'Книга с ID % не найдена', p_book_id;
    END IF;

    -- Проверка, что клиент ещё не оставлял отзыв на эту книгу
    SELECT COUNT(*) INTO v_existing_count
    FROM reviews
    WHERE customer_id = p_customer_id AND book_id = p_book_id;

    IF v_existing_count > 0 THEN
        RAISE EXCEPTION 'Клиент уже оставил отзыв на эту книгу';
    END IF;

    -- Проверка, что рейтинг в допустимом диапазоне
    IF p_rating < 1 OR p_rating > 5 THEN
        RAISE EXCEPTION 'Рейтинг должен быть от 1 до 5. Передано: %', p_rating;
    END IF;

    -- Проверка, покупал ли клиент эту книгу
    SELECT EXISTS (
        SELECT 1
        FROM orders o
        JOIN order_items oi ON o.order_id = oi.order_id
        WHERE o.customer_id = p_customer_id
          AND oi.book_id = p_book_id
          AND o.status IN ('оплачен', 'обрабатывается', 'подтверждён', 'отправлен', 'доставлен', 'возвращён')
    ) INTO v_has_purchase;

    -- Создание отзыва
    INSERT INTO reviews (customer_id, book_id, rating, comment, is_verified_purchase)
    VALUES (p_customer_id, p_book_id, p_rating, p_comment, v_has_purchase)
    RETURNING review_id INTO v_review_id;

    RETURN v_review_id;
END;
$$ LANGUAGE plpgsql;

-- Получение рейтинга книги
CREATE OR REPLACE FUNCTION sp_get_book_rating(
    p_book_id BIGINT
)
RETURNS TABLE(
    avg_rating DECIMAL(3,2),
    total_reviews INTEGER,
    verified_avg_rating DECIMAL(3,2),
    verified_reviews INTEGER,
    rating_distribution JSONB
) AS $$
BEGIN
    RETURN QUERY
    SELECT
        ROUND(AVG(r.rating)::DECIMAL, 2) AS avg_rating,
        COUNT(*)::INTEGER AS total_reviews,
        ROUND(AVG(CASE WHEN r.is_verified_purchase THEN r.rating ELSE NULL END)::DECIMAL, 2) AS verified_avg_rating,
        COUNT(CASE WHEN r.is_verified_purchase THEN 1 ELSE NULL END)::INTEGER AS verified_reviews,
        jsonb_build_object(
            '1', COUNT(CASE WHEN r.rating = 1 THEN 1 ELSE NULL END),
            '2', COUNT(CASE WHEN r.rating = 2 THEN 1 ELSE NULL END),
            '3', COUNT(CASE WHEN r.rating = 3 THEN 1 ELSE NULL END),
            '4', COUNT(CASE WHEN r.rating = 4 THEN 1 ELSE NULL END),
            '5', COUNT(CASE WHEN r.rating = 5 THEN 1 ELSE NULL END)
        ) AS rating_distribution
    FROM reviews r
    WHERE r.book_id = p_book_id;
END;
$$ LANGUAGE plpgsql;

-- Создание отправления
CREATE OR REPLACE FUNCTION sp_create_shipment(
    p_order_id BIGINT,
    p_carrier VARCHAR,
    p_tracking_number VARCHAR DEFAULT NULL
)
RETURNS BIGINT AS $$
DECLARE
    v_order_status VARCHAR(50);
    v_shipment_id BIGINT;
BEGIN
    -- Проверка статуса заказа
    SELECT status INTO v_order_status
    FROM orders
    WHERE order_id = p_order_id;

    IF NOT FOUND THEN
        RAISE EXCEPTION 'Заказ с ID % не найден', p_order_id;
    END IF;

    IF v_order_status != 'подтверждён' THEN
        RAISE EXCEPTION 'Нельзя создать отправление для заказа в статусе "%". Ожидается статус "подтверждён"',
            v_order_status;
    END IF;

    -- Проверка, что отправление ещё не создано
    IF EXISTS (SELECT 1 FROM shipments WHERE order_id = p_order_id) THEN
        RAISE EXCEPTION 'Отправление для заказа (ID: %) уже существует', p_order_id;
    END IF;

    -- Создание отправления
    INSERT INTO shipments (order_id, carrier, tracking_number, status)
    VALUES (p_order_id, p_carrier, p_tracking_number, 'создано')
    RETURNING shipment_id INTO v_shipment_id;

    -- Изменение статуса заказа на «отправлен»
    UPDATE orders
    SET status = 'отправлен'
    WHERE order_id = p_order_id;

    RETURN v_shipment_id;
END;
$$ LANGUAGE plpgsql;

-- Обновление статуса доставки
CREATE OR REPLACE FUNCTION sp_update_shipment_status(
    p_shipment_id BIGINT,
    p_new_status VARCHAR,
    p_delivered_date TIMESTAMP DEFAULT NULL
)
RETURNS VOID AS $$
DECLARE
    v_order_id BIGINT;
BEGIN
    -- Получение ID заказа
    SELECT order_id INTO v_order_id
    FROM shipments
    WHERE shipment_id = p_shipment_id;

    IF NOT FOUND THEN
        RAISE EXCEPTION 'Отправление с ID % не найдено', p_shipment_id;
    END IF;

    -- Обновление статуса доставки
    UPDATE shipments
    SET status = p_new_status,
        delivered_date = COALESCE(p_delivered_date,
            CASE WHEN p_new_status = 'доставлено' THEN CURRENT_TIMESTAMP ELSE delivered_date END)
    WHERE shipment_id = p_shipment_id;

    -- Если статус «доставлено» или указана дата доставки, обновляем заказ
    IF p_new_status = 'доставлено' OR p_delivered_date IS NOT NULL THEN
        UPDATE orders
        SET status = 'доставлен'
        WHERE order_id = v_order_id AND status = 'отправлен';
    END IF;

END;
$$ LANGUAGE plpgsql;

-- Отслеживание заказа для клиента
CREATE OR REPLACE FUNCTION sp_track_order(
    p_order_id BIGINT
)
RETURNS TABLE(
    order_id BIGINT,
    order_status VARCHAR,
    order_date TIMESTAMP,
    shipping_address TEXT,
    carrier VARCHAR,
    tracking_number VARCHAR,
    shipment_status VARCHAR,
    shipped_date TIMESTAMP,
    delivered_date TIMESTAMP,
    estimated_delivery VARCHAR
) AS $$
BEGIN
    RETURN QUERY
    SELECT
        o.order_id,
        o.status AS order_status,
        o.order_date,
        o.shipping_address,
        s.carrier,
        s.tracking_number,
        s.status AS shipment_status,
        s.shipped_date,
        s.delivered_date,
        CASE
            WHEN s.delivered_date IS NOT NULL THEN 'Доставлен'
            WHEN s.shipped_date IS NOT NULL THEN 'В пути'
            WHEN s.shipment_id IS NOT NULL THEN 'Готовится к отправке'
            ELSE 'Обрабатывается'
        END AS estimated_delivery
    FROM orders o
    LEFT JOIN shipments s ON o.order_id = s.order_id
    WHERE o.order_id = p_order_id;
END;
$$ LANGUAGE plpgsql;

-- Отчёт о продажах
CREATE OR REPLACE FUNCTION sp_get_sales_report(
    p_date_from DATE,
    p_date_to DATE,
    p_genre_id BIGINT DEFAULT NULL
)
RETURNS TABLE(
    total_orders BIGINT,
    total_revenue DECIMAL(12,2),
    total_discount DECIMAL(12,2),
    avg_order_value DECIMAL(10,2),
    total_books_sold BIGINT,
    unique_customers BIGINT,
    top_books JSONB
) AS $$
BEGIN
    RETURN QUERY
    WITH sales_data AS (
        SELECT
            o.order_id,
            o.total_amount,
            o.discount_amount,
            o.customer_id,
            oi.book_id,
            oi.quantity,
            b.title,
            b.genre_id
        FROM orders o
        JOIN order_items oi ON o.order_id = oi.order_id
        JOIN books b ON oi.book_id = b.book_id
        WHERE o.order_date::DATE BETWEEN p_date_from AND p_date_to
          AND o.status NOT IN ('отменён', 'возвращён')
          AND (p_genre_id IS NULL OR b.genre_id = p_genre_id)
    ),
    top_10_books AS (
        SELECT
            sd.title,
            SUM(sd.quantity) AS sold_count,
            SUM(sd.quantity * oi.unit_price) AS revenue
        FROM sales_data sd
        JOIN order_items oi ON sd.order_id = oi.order_id AND sd.book_id = oi.book_id
        GROUP BY sd.title, sd.book_id
        ORDER BY sold_count DESC
        LIMIT 10
    )
    SELECT
        COUNT(DISTINCT sd.order_id)::BIGINT AS total_orders,
        COALESCE(SUM(sd.total_amount), 0) AS total_revenue,
        COALESCE(SUM(sd.discount_amount), 0) AS total_discount,
        CASE
            WHEN COUNT(DISTINCT sd.order_id) > 0
            THEN ROUND((SUM(sd.total_amount) / COUNT(DISTINCT sd.order_id))::DECIMAL, 2)
            ELSE 0
        END AS avg_order_value,
        COALESCE(SUM(sd.quantity), 0)::BIGINT AS total_books_sold,
        COUNT(DISTINCT sd.customer_id)::BIGINT AS unique_customers,
        COALESCE((
            SELECT jsonb_agg(jsonb_build_object(
                'title', title,
                'sold_count', sold_count,
                'revenue', revenue
            ))
            FROM top_10_books
        ), '[]'::JSONB) AS top_books
    FROM sales_data sd;
END;
$$ LANGUAGE plpgsql;

-- Топ клиентов по сумме покупок
CREATE OR REPLACE FUNCTION sp_get_top_customers(
    p_limit_count INTEGER DEFAULT 10
)
RETURNS TABLE(
    customer_id BIGINT,
    full_name TEXT,
    email VARCHAR,
    total_orders BIGINT,
    total_spent DECIMAL(12,2),
    loyalty_points INTEGER,
    last_order_date TIMESTAMP
) AS $$
BEGIN
    RETURN QUERY
    SELECT
        c.customer_id,
        (c.first_name || ' ' || c.last_name)::TEXT AS full_name,
        c.email,
        COUNT(o.order_id)::BIGINT AS total_orders,
        COALESCE(SUM(o.total_amount - o.discount_amount), 0) AS total_spent,
        c.loyalty_points,
        MAX(o.order_date) AS last_order_date
    FROM customers c
    LEFT JOIN orders o ON c.customer_id = o.customer_id
        AND o.status NOT IN ('отменён', 'возвращён')
    GROUP BY c.customer_id, c.first_name, c.last_name, c.email, c.loyalty_points
    ORDER BY total_spent DESC
    LIMIT p_limit_count;
END;
$$ LANGUAGE plpgsql;

-- Подсчёт выручки по жанрам
CREATE OR REPLACE FUNCTION sp_get_revenue_by_genre(
    p_date_from DATE,
    p_date_to DATE
)
RETURNS TABLE(
    genre_id BIGINT,
    genre_name VARCHAR,
    books_sold BIGINT,
    revenue DECIMAL(12,2),
    percentage_of_total DECIMAL(5,2)
) AS $$
DECLARE
    v_total_revenue DECIMAL(12,2);
BEGIN
    -- Вычисление общей выручки за период
    SELECT COALESCE(SUM(o.total_amount - o.discount_amount), 0)
    INTO v_total_revenue
    FROM orders o
    WHERE o.order_date::DATE BETWEEN p_date_from AND p_date_to
      AND o.status NOT IN ('отменён', 'возвращён');

    RETURN QUERY
    SELECT
        g.genre_id,
        g.name AS genre_name,
        COALESCE(SUM(oi.quantity), 0)::BIGINT AS books_sold,
        COALESCE(SUM(oi.quantity * oi.unit_price), 0) AS revenue,
        CASE
            WHEN v_total_revenue > 0
            THEN ROUND((SUM(oi.quantity * oi.unit_price) / v_total_revenue * 100)::DECIMAL, 2)
            ELSE 0
        END AS percentage_of_total
    FROM genres g
    LEFT JOIN books b ON g.genre_id = b.genre_id
    LEFT JOIN order_items oi ON b.book_id = oi.book_id
    LEFT JOIN orders o ON oi.order_id = o.order_id
        AND o.order_date::DATE BETWEEN p_date_from AND p_date_to
        AND o.status NOT IN ('отменён', 'возвращён')
    GROUP BY g.genre_id, g.name
    ORDER BY revenue DESC;
END;
$$ LANGUAGE plpgsql;

-- Ежедневная статистика
CREATE OR REPLACE FUNCTION sp_get_daily_stats(
    p_report_date DATE DEFAULT CURRENT_DATE
)
RETURNS TABLE(
    new_orders BIGINT,
    total_revenue DECIMAL(12,2),
    avg_order_value DECIMAL(10,2),
    new_customers BIGINT,
    new_reviews BIGINT,
    books_sold BIGINT,
    orders_by_status JSONB
) AS $$
BEGIN
    RETURN QUERY
    SELECT
        COUNT(DISTINCT o.order_id)::BIGINT AS new_orders,
        COALESCE(SUM(o.total_amount - o.discount_amount), 0) AS total_revenue,
        CASE
            WHEN COUNT(DISTINCT o.order_id) > 0
            THEN ROUND((SUM(o.total_amount - o.discount_amount) / COUNT(DISTINCT o.order_id))::DECIMAL, 2)
            ELSE 0
        END AS avg_order_value,
        COUNT(DISTINCT c.customer_id) FILTER (WHERE c.registration_date = p_report_date)::BIGINT AS new_customers,
        COUNT(DISTINCT r.review_id) FILTER (WHERE r.review_date::DATE = p_report_date)::BIGINT AS new_reviews,
        COALESCE(SUM(oi.quantity), 0)::BIGINT AS books_sold,
        (
            SELECT jsonb_object_agg(status, count)
            FROM (
                SELECT o2.status, COUNT(*)::BIGINT AS count
                FROM orders o2
                WHERE o2.order_date::DATE = p_report_date
                GROUP BY o2.status
            ) status_counts
        ) AS orders_by_status
    FROM orders o
    LEFT JOIN order_items oi ON o.order_id = oi.order_id
    LEFT JOIN customers c ON o.customer_id = c.customer_id
    LEFT JOIN reviews r ON r.review_date::DATE = p_report_date
    WHERE o.order_date::DATE = p_report_date;
END;
$$ LANGUAGE plpgsql;

-- Подсчёт стоимости складских запасов
CREATE OR REPLACE FUNCTION sp_get_inventory_value()
RETURNS TABLE(
    total_books_count BIGINT,
    total_value DECIMAL(14,2),
    avg_price DECIMAL(10,2),
    by_genre JSONB,
    low_stock_count BIGINT
) AS $$
BEGIN
    RETURN QUERY
    SELECT
        COALESCE(SUM(i.quantity), 0)::BIGINT AS total_books_count,
        COALESCE(SUM(i.quantity * b.price), 0) AS total_value,
        CASE
            WHEN SUM(i.quantity) > 0
            THEN ROUND((SUM(i.quantity * b.price) / SUM(i.quantity))::DECIMAL, 2)
            ELSE 0
        END AS avg_price,
        (
            SELECT jsonb_agg(jsonb_build_object(
                'genre', g.name,
                'books_count', SUM(i2.quantity),
                'value', SUM(i2.quantity * b2.price)
            ))
            FROM inventory i2
            JOIN books b2 ON i2.book_id = b2.book_id
            JOIN genres g ON b2.genre_id = g.genre_id
            GROUP BY g.genre_id, g.name
            ORDER BY SUM(i2.quantity * b2.price) DESC
        ) AS by_genre,
        COUNT(*) FILTER (WHERE (i.quantity - i.reserved_quantity) < 5)::BIGINT AS low_stock_count
    FROM inventory i
    JOIN books b ON i.book_id = b.book_id;
END;
$$ LANGUAGE plpgsql;

-- Обработка жизненного цикла заказа
CREATE OR REPLACE FUNCTION sp_process_order_workflow(
    p_order_id BIGINT
)
RETURNS TABLE(
    step TEXT,
    success BOOLEAN,
    message TEXT
) AS $$
DECLARE
    v_status VARCHAR(50);
    v_item RECORD;
    v_available INTEGER;
BEGIN
    -- Получение статуса заказа
    SELECT status INTO v_status
    FROM orders
    WHERE order_id = p_order_id;

    IF NOT FOUND THEN
        RETURN QUERY SELECT 'check_order'::TEXT, false, 'Заказ не найден'::TEXT;
        RETURN;
    END IF;

    -- Проверка оплаты
    IF v_status = 'ожидает оплаты' THEN
        RETURN QUERY SELECT 'payment_check'::TEXT, false,
            'Заказ ещё не оплачен'::TEXT;
        RETURN;
    END IF;

    -- Проверка книги наличия на складе
    FOR v_item IN
        SELECT oi.book_id, oi.quantity, b.title
        FROM order_items oi
        JOIN books b ON oi.book_id = b.book_id
        WHERE oi.order_id = p_order_id
    LOOP
        SELECT (quantity - reserved_quantity) INTO v_available
        FROM inventory
        WHERE book_id = v_item.book_id;

        IF v_available < v_item.quantity THEN
            RETURN QUERY SELECT ('stock_check: ' || v_item.title)::TEXT, false,
                ('Недостаточно книг "' || v_item.title || '". Доступно: ' || v_available || ', требуется: ' || v_item.quantity)::TEXT;
            RETURN;
        END IF;
    END LOOP;

    -- Резервирование книг
    BEGIN
        FOR v_item IN
            SELECT book_id, quantity
            FROM order_items
            WHERE order_id = p_order_id
        LOOP
            UPDATE inventory
            SET reserved_quantity = reserved_quantity + v_item.quantity
            WHERE book_id = v_item.book_id;
        END LOOP;

        RETURN QUERY SELECT 'reserve_stock'::TEXT, true, 'Книги зарезервированы'::TEXT;
    EXCEPTION WHEN OTHERS THEN
        RETURN QUERY SELECT 'reserve_stock'::TEXT, false,
            'Ошибка при резервировании: ' || SQLERRM::TEXT;
        RETURN;
    END;

    -- Подтверждение заказа
    BEGIN
        PERFORM sp_change_order_status(p_order_id, 'подтверждён');
        RETURN QUERY SELECT 'confirm_order'::TEXT, true, 'Заказ подтверждён'::TEXT;
    EXCEPTION WHEN OTHERS THEN
        RETURN QUERY SELECT 'confirm_order'::TEXT, false,
            'Ошибка при подтверждении: ' || SQLERRM::TEXT;
        RETURN;
    END;

END;
$$ LANGUAGE plpgsql;

-- Массовое изменение цен
CREATE OR REPLACE FUNCTION sp_bulk_update_prices(
    p_genre_id BIGINT DEFAULT NULL,
    p_publisher_id BIGINT DEFAULT NULL,
    p_percentage_change DECIMAL(5,2)
)
RETURNS TABLE(
    books_updated INTEGER,
    old_avg_price DECIMAL(10,2),
    new_avg_price DECIMAL(10,2)
) AS $$
DECLARE
    v_old_avg DECIMAL(10,2);
    v_new_avg DECIMAL(10,2);
    v_updated_count INTEGER;
BEGIN
    -- Проверка, что изменение не превышает 50%
    IF ABS(p_percentage_change) > 50 THEN
        RAISE EXCEPTION 'Нельзя изменить цены более чем на 50%% за одну операцию. Запрошено: %%%',
            p_percentage_change;
    END IF;

    -- Получение средней цены до изменения
    SELECT AVG(price) INTO v_old_avg
    FROM books
    WHERE (p_genre_id IS NULL OR genre_id = p_genre_id)
      AND (p_publisher_id IS NULL OR publisher_id = p_publisher_id);

    -- Обновление цены
    WITH updated AS (
        UPDATE books
        SET price = price * (1 + p_percentage_change / 100)
        WHERE (p_genre_id IS NULL OR genre_id = p_genre_id)
          AND (p_publisher_id IS NULL OR publisher_id = p_publisher_id)
        RETURNING book_id, price
    )
    SELECT COUNT(*), AVG(price) INTO v_updated_count, v_new_avg
    FROM updated;

    RETURN QUERY SELECT
        v_updated_count,
        COALESCE(v_old_avg, 0),
        COALESCE(v_new_avg, 0);
END;
$$ LANGUAGE plpgsql;

-- Объединение дубликатов авторов
CREATE OR REPLACE FUNCTION sp_merge_authors(
    p_source_author_id BIGINT,
    p_target_author_id BIGINT
)
RETURNS TABLE(
    merged_books INTEGER,
    source_author_name TEXT,
    target_author_name TEXT
) AS $$
DECLARE
    v_source_name TEXT;
    v_target_name TEXT;
    v_merged_count INTEGER;
BEGIN
    -- Проверка, что это разные авторы
    IF p_source_author_id = p_target_author_id THEN
        RAISE EXCEPTION 'Нельзя объединить автора с самим собой';
    END IF;

    -- Получение имён авторов
    SELECT (first_name || ' ' || last_name) INTO v_source_name
    FROM authors WHERE author_id = p_source_author_id;

    IF NOT FOUND THEN
        RAISE EXCEPTION 'Исходный автор с ID % не найден', p_source_author_id;
    END IF;

    SELECT (first_name || ' ' || last_name) INTO v_target_name
    FROM authors WHERE author_id = p_target_author_id;

    IF NOT FOUND THEN
        RAISE EXCEPTION 'Целевой автор с ID % не найден', p_target_author_id;
    END IF;

    -- Перенос связи с книгами
    WITH moved AS (
        UPDATE book_authors
        SET author_id = p_target_author_id
        WHERE author_id = p_source_author_id
          AND NOT EXISTS (
              SELECT 1 FROM book_authors ba2
              WHERE ba2.book_id = book_authors.book_id
                AND ba2.author_id = p_target_author_id
          )
        RETURNING book_id
    )
    SELECT COUNT(*) INTO v_merged_count FROM moved;

    -- Удаление оставшихся связей (дубликаты) и самого автора
    DELETE FROM book_authors WHERE author_id = p_source_author_id;
    DELETE FROM authors WHERE author_id = p_source_author_id;

    RETURN QUERY SELECT
        v_merged_count,
        v_source_name::TEXT,
        v_target_name::TEXT;
END;
$$ LANGUAGE plpgsql;
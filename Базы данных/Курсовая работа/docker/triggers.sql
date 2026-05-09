-- Проверка года публикации
CREATE OR REPLACE FUNCTION fn_books_validate_year()
RETURNS TRIGGER AS $$
BEGIN
    IF NEW.publication_year IS NOT NULL AND
       NEW.publication_year > EXTRACT(YEAR FROM CURRENT_DATE) THEN
        RAISE EXCEPTION 'Год публикации (%) не может превышать текущий год (%)',
            NEW.publication_year, EXTRACT(YEAR FROM CURRENT_DATE);
    END IF;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE TRIGGER trg_books_validate_year
    BEFORE INSERT OR UPDATE ON books
    FOR EACH ROW
    EXECUTE FUNCTION fn_books_validate_year();

-- Проверка цены
CREATE OR REPLACE FUNCTION fn_books_validate_price()
RETURNS TRIGGER AS $$
BEGIN
    -- Цена должна быть больше нуля
    IF NEW.price IS NOT NULL AND NEW.price <= 0 THEN
        RAISE EXCEPTION 'Цена книги должна быть больше нуля. Текущее значение: %', NEW.price;
    END IF;

    -- Защита от резкого снижения цены (более чем на 50%)
    IF TG_OP = 'UPDATE' AND OLD.price IS NOT NULL AND NEW.price IS NOT NULL THEN
        IF NEW.price < OLD.price * 0.5 THEN
            RAISE EXCEPTION 'Нельзя снизить цену более чем на 50%% за одно изменение. Старая цена: %, новая: %',
                OLD.price, NEW.price;
        END IF;
    END IF;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE TRIGGER trg_books_validate_price
    BEFORE INSERT OR UPDATE ON books
    FOR EACH ROW
    EXECUTE FUNCTION fn_books_validate_price();

-- Запрет удаления книги, если она есть в заказах
CREATE OR REPLACE FUNCTION fn_books_prevent_delete_if_in_orders()
RETURNS TRIGGER AS $$
DECLARE
    v_order_count INTEGER;
BEGIN
    SELECT COUNT(*) INTO v_order_count
    FROM order_items
    WHERE book_id = OLD.book_id;

    IF v_order_count > 0 THEN
        RAISE EXCEPTION 'Невозможно удалить книгу (ID: %), так как она присутствует в % заказах',
            OLD.book_id, v_order_count;
    END IF;

    RETURN OLD;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE TRIGGER trg_books_prevent_delete_if_in_orders
    BEFORE DELETE ON books
    FOR EACH ROW
    EXECUTE FUNCTION fn_books_prevent_delete_if_in_orders();

-- Проверка дат действия промокода
CREATE OR REPLACE FUNCTION fn_promocodes_validate_dates()
RETURNS TRIGGER AS $$
BEGIN
    -- Проверка, что дата начала не позже даты окончания
    IF NEW.valid_from > NEW.valid_to THEN
        RAISE EXCEPTION 'Дата начала действия промокода (%) не может быть позже даты окончания (%)',
            NEW.valid_from, NEW.valid_to;
    END IF;

    -- При создании нового промокода дата окончания не должна быть в прошлом
    IF TG_OP = 'INSERT' AND NEW.valid_to < CURRENT_DATE THEN
        RAISE EXCEPTION 'Нельзя создать промокод с истёкшим сроком действия. valid_to: %', NEW.valid_to;
    END IF;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE TRIGGER trg_promocodes_validate_dates
    BEFORE INSERT OR UPDATE ON promocodes
    FOR EACH ROW
    EXECUTE FUNCTION fn_promocodes_validate_dates();

-- Проверка лимита использований промокода
CREATE OR REPLACE FUNCTION fn_promocodes_check_usage_limit()
RETURNS TRIGGER AS $$
BEGIN
    -- Проверка, что счётчик не превышает максимальное количество
    IF NEW.max_uses IS NOT NULL AND NEW.used_count > NEW.max_uses THEN
        RAISE EXCEPTION 'Превышен лимит использований промокода. Использовано: %, лимит: %',
            NEW.used_count, NEW.max_uses;
    END IF;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE TRIGGER trg_promocodes_check_usage_limit
    BEFORE UPDATE OF used_count ON promocodes
    FOR EACH ROW
    WHEN (NEW.used_count > OLD.used_count)
    EXECUTE FUNCTION fn_promocodes_check_usage_limit();

-- Валидация значения скидки
CREATE OR REPLACE FUNCTION fn_promocodes_validate_discount()
RETURNS TRIGGER AS $$
BEGIN
    IF NEW.discount_type = 'PERCENT' THEN
        IF NEW.discount_value < 1 OR NEW.discount_value > 100 THEN
            RAISE EXCEPTION 'Процент скидки должен быть от 1 до 100. Текущее значение: %',
                NEW.discount_value;
        END IF;
    ELSIF NEW.discount_type = 'FIXED' THEN
        IF NEW.discount_value <= 0 THEN
            RAISE EXCEPTION 'Фиксированная скидка должна быть больше нуля. Текущее значение: %',
                NEW.discount_value;
        END IF;
    END IF;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE TRIGGER trg_promocodes_validate_discount
    BEFORE INSERT OR UPDATE ON promocodes
    FOR EACH ROW
    EXECUTE FUNCTION fn_promocodes_validate_discount();

-- Валидация email
CREATE OR REPLACE FUNCTION fn_customers_validate_email()
RETURNS TRIGGER AS $$
BEGIN
    -- Проверка формата email
    IF NEW.email IS NOT NULL AND NEW.email !~ '^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$' THEN
        RAISE EXCEPTION 'Некорректный формат email: %', NEW.email;
    END IF;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE TRIGGER trg_customers_validate_email
    BEFORE INSERT OR UPDATE ON customers
    FOR EACH ROW
    EXECUTE FUNCTION fn_customers_validate_email();

-- Запрет удаления клиента с заказами
CREATE OR REPLACE FUNCTION fn_customers_prevent_delete_with_orders()
RETURNS TRIGGER AS $$
DECLARE
    v_order_count INTEGER;
BEGIN
    SELECT COUNT(*) INTO v_order_count
    FROM orders
    WHERE customer_id = OLD.customer_id;

    IF v_order_count > 0 THEN
        RAISE EXCEPTION 'Невозможно удалить клиента (ID: %), так как у него есть % заказов',
            OLD.customer_id, v_order_count;
    END IF;

    RETURN OLD;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE TRIGGER trg_customers_prevent_delete_with_orders
    BEFORE DELETE ON customers
    FOR EACH ROW
    EXECUTE FUNCTION fn_customers_prevent_delete_with_orders();

-- Проверка бонусных баллов
CREATE OR REPLACE FUNCTION fn_customers_validate_loyalty_points()
RETURNS TRIGGER AS $$
BEGIN
    -- Баллы не могут быть отрицательными
    IF NEW.loyalty_points < 0 THEN
        RAISE EXCEPTION 'Бонусные баллы не могут быть отрицательными. Текущее значение: %',
            NEW.loyalty_points;
    END IF;

    -- Логирование значительных изменений (более 1000 баллов)
    IF TG_OP = 'UPDATE' AND ABS(NEW.loyalty_points - OLD.loyalty_points) > 1000 THEN
        INSERT INTO audit_log (table_name, record_id, operation, old_data, new_data)
        VALUES ('customers', NEW.customer_id, 'LOYALTY_CHANGE',
                jsonb_build_object('loyalty_points', OLD.loyalty_points),
                jsonb_build_object('loyalty_points', NEW.loyalty_points));
    END IF;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE TRIGGER trg_customers_validate_loyalty_points
    BEFORE UPDATE ON customers
    FOR EACH ROW
    EXECUTE FUNCTION fn_customers_validate_loyalty_points();

-- Валидация рейтинга
CREATE OR REPLACE FUNCTION fn_reviews_validate_rating()
RETURNS TRIGGER AS $$
BEGIN
    IF NEW.rating < 1 OR NEW.rating > 5 THEN
        RAISE EXCEPTION 'Рейтинг должен быть от 1 до 5. Текущее значение: %', NEW.rating;
    END IF;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE TRIGGER trg_reviews_validate_rating
    BEFORE INSERT OR UPDATE ON reviews
    FOR EACH ROW
    EXECUTE FUNCTION fn_reviews_validate_rating();

-- Автоматическая установка флага подтверждённой покупки
CREATE OR REPLACE FUNCTION fn_reviews_set_verified_purchase()
RETURNS TRIGGER AS $$
DECLARE
    v_has_purchase BOOLEAN;
BEGIN
    -- Проверяем, покупал ли клиент эту книгу
    SELECT EXISTS (
        SELECT 1
        FROM orders o
        JOIN order_items oi ON o.order_id = oi.order_id
        WHERE o.customer_id = NEW.customer_id
          AND oi.book_id = NEW.book_id
          AND o.status IN ('оплачен', 'обрабатывается', 'подтверждён', 'отправлен', 'доставлен', 'возвращён')
    ) INTO v_has_purchase;

    -- Устанавливаем флаг на основе проверки
    NEW.is_verified_purchase := v_has_purchase;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE TRIGGER trg_reviews_set_verified_purchase
    BEFORE INSERT ON reviews
    FOR EACH ROW
    EXECUTE FUNCTION fn_reviews_set_verified_purchase();

-- Запрет дублирования отзывов
CREATE OR REPLACE FUNCTION fn_reviews_prevent_duplicate()
RETURNS TRIGGER AS $$
DECLARE
    v_existing_count INTEGER;
BEGIN
    SELECT COUNT(*) INTO v_existing_count
    FROM reviews
    WHERE customer_id = NEW.customer_id AND book_id = NEW.book_id;

    IF v_existing_count > 0 THEN
        RAISE EXCEPTION 'Клиент (ID: %) уже оставил отзыв на книгу (ID: %)',
            NEW.customer_id, NEW.book_id;
    END IF;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE TRIGGER trg_reviews_prevent_duplicate
    BEFORE INSERT ON reviews
    FOR EACH ROW
    EXECUTE FUNCTION fn_reviews_prevent_duplicate();

-- Автоматический расчёт суммы заказа
CREATE OR REPLACE FUNCTION fn_orders_calculate_total()
RETURNS TRIGGER AS $$
DECLARE
    v_total DECIMAL(10,2);
BEGIN
    -- Вычисляем общую сумму на основе позиций заказа
    SELECT COALESCE(SUM(quantity * unit_price), 0) INTO v_total
    FROM order_items
    WHERE order_id = NEW.order_id;

    -- Устанавливаем общую сумму
    NEW.total_amount := v_total;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE TRIGGER trg_orders_calculate_total
    BEFORE INSERT OR UPDATE ON orders
    FOR EACH ROW
    EXECUTE FUNCTION fn_orders_calculate_total();

-- Контроль переходов статусов заказа
CREATE OR REPLACE FUNCTION fn_orders_validate_status_transition()
RETURNS TRIGGER AS $$
BEGIN
    -- Если статус не изменился, пропускаем
    IF OLD.status = NEW.status THEN
        RETURN NEW;
    END IF;

    -- Проверяем допустимость перехода
    CASE OLD.status
        WHEN 'ожидает оплаты' THEN
            IF NEW.status NOT IN ('оплачен', 'отменён') THEN
                RAISE EXCEPTION 'Недопустимый переход статуса: % -> %', OLD.status, NEW.status;
            END IF;
        WHEN 'оплачен' THEN
            IF NEW.status NOT IN ('обрабатывается', 'отменён') THEN
                RAISE EXCEPTION 'Недопустимый переход статуса: % -> %', OLD.status, NEW.status;
            END IF;
        WHEN 'обрабатывается' THEN
            IF NEW.status NOT IN ('подтверждён', 'отменён') THEN
                RAISE EXCEPTION 'Недопустимый переход статуса: % -> %', OLD.status, NEW.status;
            END IF;
        WHEN 'подтверждён' THEN
            IF NEW.status NOT IN ('отправлен', 'отменён') THEN
                RAISE EXCEPTION 'Недопустимый переход статуса: % -> %', OLD.status, NEW.status;
            END IF;
        WHEN 'отправлен' THEN
            IF NEW.status NOT IN ('доставлен', 'возвращён') THEN
                RAISE EXCEPTION 'Недопустимый переход статуса: % -> %', OLD.status, NEW.status;
            END IF;
        WHEN 'доставлен' THEN
            IF NEW.status != 'возвращён' THEN
                RAISE EXCEPTION 'Недопустимый переход статуса: % -> %', OLD.status, NEW.status;
            END IF;
        WHEN 'отменён', 'возвращён' THEN
            RAISE EXCEPTION 'Статус "%" является конечным и не может быть изменён', OLD.status;
        ELSE
            RAISE EXCEPTION 'Неизвестный статус заказа: %', OLD.status;
    END CASE;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE TRIGGER trg_orders_validate_status_transition
    BEFORE UPDATE OF status ON orders
    FOR EACH ROW
    EXECUTE FUNCTION fn_orders_validate_status_transition();

-- Применение промокода к заказу
CREATE OR REPLACE FUNCTION fn_orders_apply_promocode()
RETURNS TRIGGER AS $$
DECLARE
    v_discount DECIMAL(10,2) := 0;
    v_promo_record RECORD;
BEGIN
    -- Если промокод не указан или не изменился, выходим
    IF NEW.promo_code_id IS NULL OR
       (TG_OP = 'UPDATE' AND OLD.promo_code_id = NEW.promo_code_id) THEN
        RETURN NEW;
    END IF;

    -- Получаем данные промокода
    SELECT * INTO v_promo_record
    FROM promocodes
    WHERE promo_code_id = NEW.promo_code_id;

    IF NOT FOUND THEN
        RAISE EXCEPTION 'Промокод с ID % не найден', NEW.promo_code_id;
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
    IF v_promo_record.min_order_amount > 0 AND NEW.total_amount < v_promo_record.min_order_amount THEN
        RAISE EXCEPTION 'Сумма заказа (%) меньше минимальной для применения промокода (%)',
            NEW.total_amount, v_promo_record.min_order_amount;
    END IF;

    -- Рассчитываем скидку
    IF v_promo_record.discount_type = 'PERCENT' THEN
        v_discount := NEW.total_amount * v_promo_record.discount_value / 100;
    ELSIF v_promo_record.discount_type = 'FIXED' THEN
        v_discount := v_promo_record.discount_value;
    END IF;

    -- Скидка не может превышать сумму заказа
    IF v_discount > NEW.total_amount THEN
        v_discount := NEW.total_amount;
    END IF;

    -- Устанавливаем скидку
    NEW.discount_amount := v_discount;

    -- Увеличиваем счётчик использований промокода
    UPDATE promocodes
    SET used_count = used_count + 1
    WHERE promo_code_id = NEW.promo_code_id;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE TRIGGER trg_orders_apply_promocode
    BEFORE INSERT OR UPDATE OF promo_code_id ON orders
    FOR EACH ROW
    EXECUTE FUNCTION fn_orders_apply_promocode();

-- Проверка минимальной суммы для промокода
CREATE OR REPLACE FUNCTION fn_orders_check_min_amount_for_promo()
RETURNS TRIGGER AS $$
DECLARE
    v_min_amount DECIMAL(10,2);
BEGIN
    IF NEW.promo_code_id IS NOT NULL THEN
        SELECT min_order_amount INTO v_min_amount
        FROM promocodes
        WHERE promo_code_id = NEW.promo_code_id;

        IF v_min_amount > 0 AND NEW.total_amount < v_min_amount THEN
            RAISE EXCEPTION 'Сумма заказа (%) меньше минимальной для применения промокода (%)',
                NEW.total_amount, v_min_amount;
        END IF;
    END IF;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE TRIGGER trg_orders_check_min_amount_for_promo
    BEFORE INSERT OR UPDATE ON orders
    FOR EACH ROW
    EXECUTE FUNCTION fn_orders_check_min_amount_for_promo();

-- Автоматическая фиксация цены книги
CREATE OR REPLACE FUNCTION fn_order_items_fix_price()
RETURNS TRIGGER AS $$
DECLARE
    v_current_price DECIMAL(10,2);
BEGIN
    -- Получаем текущую цену книги из каталога
    SELECT price INTO v_current_price
    FROM books
    WHERE book_id = NEW.book_id;

    IF NOT FOUND THEN
        RAISE EXCEPTION 'Книга с ID % не найдена в каталоге', NEW.book_id;
    END IF;

    -- Фиксируем цену на момент добавления в заказ
    NEW.unit_price := v_current_price;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE TRIGGER trg_order_items_fix_price
    BEFORE INSERT ON order_items
    FOR EACH ROW
    EXECUTE FUNCTION fn_order_items_fix_price();

-- Проверка доступности книги на складе
CREATE OR REPLACE FUNCTION fn_order_items_check_availability()
RETURNS TRIGGER AS $$
DECLARE
    v_available INTEGER;
BEGIN
    -- Вычисляем доступное количество (общее - зарезервированное)
    SELECT (quantity - reserved_quantity) INTO v_available
    FROM inventory
    WHERE book_id = NEW.book_id;

    IF NOT FOUND THEN
        RAISE EXCEPTION 'Книга с ID % отсутствует на складе', NEW.book_id;
    END IF;

    IF NEW.quantity > v_available THEN
        RAISE EXCEPTION 'Недостаточно книг на складе. Запрошено: %, доступно: % (с учётом резерва)',
            NEW.quantity, v_available;
    END IF;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE TRIGGER trg_order_items_check_availability
    BEFORE INSERT ON order_items
    FOR EACH ROW
    EXECUTE FUNCTION fn_order_items_check_availability();

-- Обновление складских остатков
CREATE OR REPLACE FUNCTION fn_order_items_update_inventory()
RETURNS TRIGGER AS $$
BEGIN
    IF TG_OP = 'INSERT' THEN
        -- Увеличиваем зарезервированное количество
        UPDATE inventory
        SET reserved_quantity = reserved_quantity + NEW.quantity
        WHERE book_id = NEW.book_id;

    ELSIF TG_OP = 'DELETE' THEN
        -- Уменьшаем зарезервированное количество
        UPDATE inventory
        SET reserved_quantity = reserved_quantity - OLD.quantity
        WHERE book_id = OLD.book_id;

        -- Проверяем, что резерв не стал отрицательным
        IF EXISTS (SELECT 1 FROM inventory WHERE book_id = OLD.book_id AND reserved_quantity < 0) THEN
            UPDATE inventory SET reserved_quantity = 0 WHERE book_id = OLD.book_id;
        END IF;
    END IF;

    RETURN NULL;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE TRIGGER trg_order_items_update_inventory
    AFTER INSERT OR DELETE ON order_items
    FOR EACH ROW
    EXECUTE FUNCTION fn_order_items_update_inventory();

-- Проверка количества
CREATE OR REPLACE FUNCTION fn_order_items_validate_quantity()
RETURNS TRIGGER AS $$
BEGIN
    IF NEW.quantity <= 0 THEN
        RAISE EXCEPTION 'Количество экземпляров должно быть больше нуля. Текущее значение: %',
            NEW.quantity;
    END IF;

    IF NEW.quantity > 100 THEN
        RAISE EXCEPTION 'Нельзя заказать более 100 экземпляров одной книги в одном заказе. Запрошено: %',
            NEW.quantity;
    END IF;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE TRIGGER trg_order_items_validate_quantity
    BEFORE INSERT OR UPDATE ON order_items
    FOR EACH ROW
    EXECUTE FUNCTION fn_order_items_validate_quantity();

-- Обновление даты пополнения
CREATE OR REPLACE FUNCTION fn_inventory_update_timestamp()
RETURNS TRIGGER AS $$
BEGIN
    -- Если количество увеличилось (пополнение склада)
    IF NEW.quantity > OLD.quantity THEN
        NEW.last_restocked := CURRENT_DATE;
    END IF;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE TRIGGER trg_inventory_update_timestamp
    BEFORE UPDATE ON inventory
    FOR EACH ROW
    EXECUTE FUNCTION fn_inventory_update_timestamp();

-- Предотвращение отрицательных значений
CREATE OR REPLACE FUNCTION fn_inventory_prevent_negative()
RETURNS TRIGGER AS $$
BEGIN
    IF NEW.quantity < 0 THEN
        RAISE EXCEPTION 'Количество на складе не может быть отрицательным. Текущее значение: %',
            NEW.quantity;
    END IF;

    IF NEW.reserved_quantity < 0 THEN
        RAISE EXCEPTION 'Зарезервированное количество не может быть отрицательным. Текущее значение: %',
            NEW.reserved_quantity;
    END IF;

    IF NEW.reserved_quantity > NEW.quantity THEN
        RAISE EXCEPTION 'Зарезервированное количество (%) не может превышать общее количество (%)',
            NEW.reserved_quantity, NEW.quantity;
    END IF;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE TRIGGER trg_inventory_prevent_negative
    BEFORE UPDATE ON inventory
    FOR EACH ROW
    EXECUTE FUNCTION fn_inventory_prevent_negative();

-- Предотвращение циклических ссылок
CREATE OR REPLACE FUNCTION fn_genres_prevent_circular_reference()
RETURNS TRIGGER AS $$
DECLARE
    v_parent_id BIGINT;
BEGIN
    -- Проверка на ссылку самого на себя
    IF NEW.parent_genre_id = NEW.genre_id THEN
        RAISE EXCEPTION 'Жанр не может быть родительским для самого себя';
    END IF;

    -- Проверка на циклические зависимости
    v_parent_id := NEW.parent_genre_id;
    WHILE v_parent_id IS NOT NULL LOOP
        IF v_parent_id = NEW.genre_id THEN
            RAISE EXCEPTION 'Обнаружена циклическая зависимость в иерархии жанров';
        END IF;

        SELECT parent_genre_id INTO v_parent_id
        FROM genres
        WHERE genre_id = v_parent_id;
    END LOOP;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE TRIGGER trg_genres_prevent_circular_reference
    BEFORE INSERT OR UPDATE ON genres
    FOR EACH ROW
    EXECUTE FUNCTION fn_genres_prevent_circular_reference();

-- Запрет удаления жанра с книгами
CREATE OR REPLACE FUNCTION fn_genres_prevent_delete_with_books()
RETURNS TRIGGER AS $$
DECLARE
    v_book_count INTEGER;
    v_book_list TEXT;
BEGIN
    SELECT COUNT(*) INTO v_book_count
    FROM books
    WHERE genre_id = OLD.genre_id;

    IF v_book_count > 0 THEN
        -- Получаем список книг для информативного сообщения
        SELECT STRING_AGG(title, ', ' ORDER BY title) INTO v_book_list
        FROM books
        WHERE genre_id = OLD.genre_id
        LIMIT 5;

        IF v_book_count > 5 THEN
            v_book_list := v_book_list || ' и ещё ' || (v_book_count - 5) || ' книг(и)';
        END IF;

        RAISE EXCEPTION 'Невозможно удалить жанр "%", так как к нему привязаны книги: %',
            OLD.name, v_book_list;
    END IF;

    RETURN OLD;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE TRIGGER trg_genres_prevent_delete_with_books
    BEFORE DELETE ON genres
    FOR EACH ROW
    EXECUTE FUNCTION fn_genres_prevent_delete_with_books();

-- Проверка дат доставки
CREATE OR REPLACE FUNCTION fn_shipments_validate_dates()
RETURNS TRIGGER AS $$
DECLARE
    v_order_date TIMESTAMP;
BEGIN
    -- Проверка дат отправки и доставки
    IF NEW.shipped_date IS NOT NULL AND NEW.delivered_date IS NOT NULL THEN
        IF NEW.shipped_date > NEW.delivered_date THEN
            RAISE EXCEPTION 'Дата отправки (%) не может быть позже даты доставки (%)',
                NEW.shipped_date, NEW.delivered_date;
        END IF;
    END IF;

    -- Проверка, что дата доставки не раньше даты создания заказа
    SELECT order_date INTO v_order_date
    FROM orders
    WHERE order_id = NEW.order_id;

    IF NEW.delivered_date IS NOT NULL AND NEW.delivered_date < v_order_date THEN
        RAISE EXCEPTION 'Дата доставки (%) не может быть раньше даты создания заказа (%)',
            NEW.delivered_date, v_order_date;
    END IF;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE TRIGGER trg_shipments_validate_dates
    BEFORE INSERT OR UPDATE ON shipments
    FOR EACH ROW
    EXECUTE FUNCTION fn_shipments_validate_dates();

-- Обновление статуса заказа при изменении доставки
CREATE OR REPLACE FUNCTION fn_shipments_update_order_status()
RETURNS TRIGGER AS $$
BEGIN
    IF TG_OP = 'INSERT' THEN
        -- При создании записи о доставке меняем статус заказа на «отправлен»
        UPDATE orders
        SET status = 'отправлен'
        WHERE order_id = NEW.order_id AND status = 'подтверждён';

    ELSIF TG_OP = 'UPDATE' THEN
        -- При заполнении даты доставки меняем статус на «доставлен»
        IF NEW.delivered_date IS NOT NULL AND OLD.delivered_date IS NULL THEN
            UPDATE orders
            SET status = 'доставлен'
            WHERE order_id = NEW.order_id AND status = 'отправлен';
        END IF;
    END IF;

    RETURN NULL;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE TRIGGER trg_shipments_update_order_status
    AFTER INSERT OR UPDATE ON shipments
    FOR EACH ROW
    EXECUTE FUNCTION fn_shipments_update_order_status();

-- Проверка и автоназначение порядка авторов
CREATE OR REPLACE FUNCTION fn_book_authors_validate_order()
RETURNS TRIGGER AS $$
DECLARE
    v_max_order INTEGER;
BEGIN
    -- Если порядковый номер не указан, назначаем следующий
    IF NEW.author_order IS NULL THEN
        SELECT COALESCE(MAX(author_order), 0) + 1 INTO v_max_order
        FROM book_authors
        WHERE book_id = NEW.book_id;

        NEW.author_order := v_max_order;
    ELSE
        -- Проверяем, что нет другого автора с таким же порядковым номером для этой книги
        IF EXISTS (
            SELECT 1 FROM book_authors
            WHERE book_id = NEW.book_id
              AND author_order = NEW.author_order
              AND author_id != NEW.author_id
        ) THEN
            RAISE EXCEPTION 'У книги (ID: %) уже есть автор с порядковым номером %',
                NEW.book_id, NEW.author_order;
        END IF;
    END IF;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE TRIGGER trg_book_authors_validate_order
    BEFORE INSERT ON book_authors
    FOR EACH ROW
    EXECUTE FUNCTION fn_book_authors_validate_order();
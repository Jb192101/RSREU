package org.jedi_bachelor.course_paper_db.entity;

import lombok.Getter;
import lombok.RequiredArgsConstructor;

@RequiredArgsConstructor
@Getter
public enum OrderStatus {
    CONFIRMED("Подтверждён"),
    CANCELED("Отменён"),
    SENT("Отправлено"),
    DELIVERED("Доставлено"),
    UNDER_CONSIDERATION("Рассматривается");

    private final String text;
}

-- 1 задание

select * from projects;

-- 2 задание

select * from employees where id >= 4 and id <= 7;

-- 3 задание

select * from projects where название_проекта LIKE 'P%';

-- 4 задание

SELECT * FROM projects ORDER BY название_проекта;

-- 5 задание

select max(бюджет) from projects;

-- 6 задание

SELECT 
    p.название AS название_проекта,
    COUNT(DISTINCT a.id_сотрудника) AS количество_сотрудников
FROM projects p
LEFT JOIN assignments a ON p.id = a.id_проекта
GROUP BY p.id, p.название
ORDER BY количество_сотрудников DESC;

-- 7 задание

SELECT 
    a.id_назначения,
    e.ФИО as сотрудник,
    p.название_проекта as проект,
    a.отработано_часов,
    a.ставка_в_час,
    a.дата_назначения
FROM assingment a
JOIN employees e ON a.id_сотрудника = e.id
JOIN projects p ON a.id_проекта = p.id
ORDER BY a.дата_назначения DESC;

-- 8 задание 

SELECT 
    id_сотрудника,
    COUNT(id_проекта) as количество_проектов
FROM assingments
GROUP BY id_сотрудника
ORDER BY количество_проектов DESC, id_сотрудника;

-- 9 задание

create or replace view template_view3
as select employees.фио, COUNT(id_проекта) as количество_проектов
from assingments INNER JOIN employees ON employees.id = assingments.id_сотрудника
GROUP BY employees.фио
ORDER BY количество_проектов DESC limit 1;

select * from template_view3;

-- 10 задание

create or replace view template_view5
as select employees.id, employees.фио, employees.должность
from assingments INNER JOIN employees ON employees.id = assingments.id_сотрудника
GROUP BY employees.id
ORDER BY COUNT(id_проекта) DESC limit 1;

select * from template_view5;

-- 11 задание

create or replace view template_view6
as select projects.id, projects.название_проекта, projects.бюджет
from assingments INNER JOIN projects ON projects.id = assingments.id_проекта
GROUP BY projects.id
ORDER BY COUNT(assingments.отработано_часов) DESC limit 1;

select * from template_view6;

select * from assingments;
select * from projects;

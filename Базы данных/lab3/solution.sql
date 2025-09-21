select * from employees;
select * from projects;
select * from assignment;

-- 1.1
-- Вывести полную информацию обо всех сотрудниках
select * from employees;

-- 1.2
-- Вывести информацию о названии и бюджете всех проектов
select название_проекта, Бюджет 
from projects;

-- 1.3
-- Вывести информацию о пяти первых записях о назначениях
select * from assignment
limit 5;

-- 1.4
-- Вывести информацию обо всех назначениях, указав в поле «Статус» строку «Активно»
SELECT 
    a.id_назначения,
    a.id_сотрудника,
    a.id_проекта,
    a.отработано_часов,
    a.ставка_в_час,
    a.дата_назначения,
    CASE 
        WHEN a.дата_назначения > CURRENT_DATE - INTERVAL '30 days' THEN 'Активно'
        ELSE 'Неактивно'
    END AS Статус
FROM 
    assignment a;

-- 1.6
-- Вычислить среднюю ставку в час
select AVG(cast(ставка_в_час as numeric)) 
from assignment;

-- 1.7
-- Найти минимальное количество отработанных часов
select MIN(отработано_часов)
from assignment;

-- 1.8
-- Найти максимальный бюджет проекта
select MAX(Бюджет)
from projects;

-- 1.9
-- Найти количество активных назначений
SELECT 
    COUNT(*) AS количество_активных_назначений
FROM 
    assignment
WHERE 
    дата_назначения > CURRENT_DATE - INTERVAL '30 days';
	
-- 2.1
select * from assignments where id_назначения < 10;

-- 2.2
select * from projects where бюджет < '$1000000'::money and бюджет > '$100000'::money;

-- 2.3
select * from employees where фио like 'Сидоров' or фио like 'Иванов' or фио like 'Петров';

-- 2.4
select * from projects where название_проекта like 'А%' or название_проекта like 'Б%' название_проекта like 'В%';

-- 2.5
select * from assignments where ставка_в_час != NULL;

-- 2.6
select * from employees where должность = NULL;

-- 3.1
-- Вывести количество назначений по каждой должности.
create view viewTable1 as
select e.Должность, a.id_назначения
from employees e
join assignment a on a.id_сотрудника = e.id
order by e.Должность;

select Должность, count(Должность) 
from viewTable1
group by Должность;

select * from viewTable1

-- 3.2
-- Вывести среднее количество часов по каждому проекту
SELECT 
    p.id,
    p.название_проекта,
    AVG(a.отработано_часов) AS среднее_количество_часов
FROM 
    projects p
JOIN 
    assignment a ON p.id = a.id_проекта
GROUP BY 
    p.id, p.название_проекта
ORDER BY 
    p.id;

-- 3.3
-- Вывести минимальную ставку для должности "Разработчик"
select min(ставка_в_час) from assingments inner join employees as e on e.id = assingments.id_сотрудника and
e.должность = 'Разработчик';

-- 4.1
-- Вывести должности, которые участвуют более чем в 3 проектах.
SELECT 
    e.Должность,
    COUNT(DISTINCT a.id_проекта) AS количество_проектов
FROM 
    employees e
JOIN 
    assignment a ON e.id = a.id_сотрудника
GROUP BY 
    e.Должность
HAVING 
    COUNT(DISTINCT a.id_проекта) > 3;

-- 4.2
-- Вывести id проектов, где среднее количество часов превышает 100
SELECT 
    a.id_проекта,
    p.название_проекта,
    AVG(a.отработано_часов) AS среднее_количество_часов
FROM 
    assignment a
JOIN 
    projects p ON a.id_проекта = p.id
GROUP BY 
    a.id_проекта, p.название_проекта
HAVING 
    AVG(a.отработано_часов) > 100
ORDER BY 
    a.id_проекта;

-- 5.1
-- Вывести список сотрудников, отсортировав по ФИО.
select * from employees order by фио;

-- 5.2
-- Вывести список проектов и их бюджет, отсортировав по убыванию бюджета
select * from projects order by бюджет desc;

-- 5.3
-- Вывести информацию о трех самых дорогих проектах
select * from projects order by бюджет desc limit 3;

-- 6.1
-- Вывести единый список ФИО сотрудников и названий проектов
SELECT фио AS "Единый список"
FROM employees

UNION

SELECT название_проекта AS "Единый список"
FROM projects
ORDER BY "Единый список";

-- 6.2
-- Вывести полную информацию обо всех проектах. Если бюджет не указан, вывести «неизвестен»
select название_проекта, 
case when бюджет is null then 'Неизвестно' else бюджет::char(50) end 
from projects;

-- 6.3
-- Изменить ставку в час в зависимости от опыта: <1000 часов -> -10%, 1000-5000 -> 0%, >5000 -> +20%
select id_проекта, 
отработано_часов,
case when отработано_часов < 1000 then 0.9*ставка_в_час
when отработано_часов > 5000 then 1.2*ставка_в_час
else ставка_в_час end 
from assingments;

-- 7.1
-- Вывести id сотрудников, которые работали и над проектом "Альфа", и над проектом "Бета"
select Distinct(id_сотрудника) from assingments inner join projects as p on p.название_проекта = 'Альфа'
and p.id = 'Бета';

-- 7.2
-- Вывести id проектов, которые выполнялись и в I, и во II квартале 2023 года
select id_проекта from assingments where дата_назначения < '2023-30-06';

-- 8.1
-- Вывести id сотрудников, которые не работали над проектами в 2023 году
select id_сотрудника from assingments where дата_назначения < '2023-01-01' or дата_назначения > '2023-31-12;

-- 8.2
-- Вывести id проектов, которые не выполнялись ни одним сотрудником с должностью "Стажер"
select id_проекта from assingments inner join employees as e on 
e.должность != 'Стажер' and e.id = assingments.id_сотрудника;


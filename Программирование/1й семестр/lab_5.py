# Цель: создать программу, вычисляющую и выводящую число байт в выбранной единице измерения
# Переменные:
# i - параметр, отвечающий за вывод конкретной единицы измерения
# v - количество единицы измерения
# Программист: Барышев Г.А.
# Дата написания: 27.09.2023

# Блок меню
v = float(input("Введите цифру: "))
print("Выберите номер единицы измерения: ")
print("1. Килобайт")
print("2. Мегабайт")
print("3. Гигабайт")
print("4. Терабайт")
i = int(input("Номер: "))

#Блок с многоальтернативным ветвлением
if i == 1:
    kb = v*2**10
    print("Количество байт в килобайте равно", kb)
elif i == 2:
    mb = v*2**20
    print("Количество байт в мегабайте равно", mb)
elif i == 3:
    gb = v*2**30
    print("Количество байт в гигабайте равно", gb)
elif i == 4:
    tb = v*2**40
    print("Количество байт в терабайте равно", tb)
else:
    print("Неверный номер!")

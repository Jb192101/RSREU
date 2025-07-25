# Цель: создать программу, вычисляющую конечные суммы и произведения
# Переменные:
# x - первое вводимое значение
# a - второе вводимое значение
# k, m - параметры, отвечающие за количество итераций в циклах с заголовком
# b, c, d - промежуточные функции
# W - выводимое значение
# Программист: Барышев Г.А.
# Дата написания: 30.11.2023

from math import sin, pi
# Подпрограммы
def summ(x):
    W = 0
    d = 1
    for k in range(1, 11):
        d = d * x
        W += d * sin(k * pi / 4)
    return W

def product(x, a):
    W = 1
    b = 1
    c = 1
    for m in range(1, 6):
        b *= a
        c *= x
        W *= (b - c)
    return W
# Блок ввода
x = float(input("Введите число х: "))
a = float(input("Введите число а: "))

#Эхо-вывод
print("x =", x)
print("a =", a)

b = 1
c = 1

# Блок проверки
if (x < a):
    W = product(x, a)
else:
    W = summ(x)

# Вывод значения W
print("Значение W равно", round(W, 4))

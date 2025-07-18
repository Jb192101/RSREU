# Цель: создать программу, табулирующую функции f1(x) и f2(x) с помощью цикла с параметром
# Переменные:
# x0, xn - начальное и конечное значение x
# hx - шаг изменения
# N - количество шагов цикла
# y1, y2 - значение функций f1(x) и f2(x) соответственно
# Программист: Барышев Г.А.
# Дата написания: 08.10.2023
from math import sin, cos, floor

# Блок ввода
print("Введите исходные данные: ")
print("x0 = ", end = '')
x0 = float(input())
print("hx = ", end = '')
hx = float(input())
print("xn = ", end = '')
xn = float(input())
N = floor((xn - x0)/hx) + 1

# Проверка ОДЗ
if(N <= 0 or x0 > xn or hx <= 0):
    print("Неверно введённые данные!")
    exit()

# Вывод введённых данных
print("Вы ввели: ")
print("x0 = %.2f  hx = %.2f  xn = %.2f" % (x0, hx, xn))
print("")

#Начальная таблица
print("┌" + "─" * 21 + "┬" + "─" * 27 + "┬" + "─" * 27 + "┐")
print('│' + ' ' * 10 + 'x' + ' ' * 10 + '│' + ' ' * 13 + 'y1' + ' ' * 12 + '│' + ' ' * 13 + 'y2' + ' ' * 12 + '│')
print('└' + '─' * 21 + '┴' + '─' * 27 + '┴' + '─' + '─' * 26 + '┘')

x = x0
    
# Табулирование функций
for i in range(0, N):
    y1 = 2 + sin(3 * x)
    y2 = cos(x / 3)
    print("┌" + "─" * 21 + "┬" + "─" * 27 + "┬" + "─" * 27 + "┐")
    print(f'│{round(x, 8):11}' + ' ' * 10 + f'│{round(y1, 8):18}' + ' ' * 9 + '│' + ' ' * 5 + f'{round(y2, 8):13}' + ' ' * 9 + '│')
    print('└' + '─' * 21 + '┴' + '─' * 27 + '┴' + '─' + '─' * 26 + '┘')
    x += hx
    i += 1

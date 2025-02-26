# Цель: создать программу, обнуляющую все элементы, которые принадлежат строке и столбцу, в которых лежит максимальный элемент
# Переменные:
# M, N – количество столбцов и строк в многомерном массиве
# inp – переменная для выбора метода заполнения матрицы
# Z – исходная матрица
# Z1 - матрица для копирования
# Z2 - матрица, используемая во втором методе
# s - строка, используемая для заполнения матрицы
# max_elem – максимальный элемент в матрице
# elem_p - промежуточный элемент для нахождения максимального элемента матрицы
# m, n – индексы максимального элемента
# T1, T2 – начало и конец вычисления времени работы методов
# Time_result1, Time_result2 – время работы обоих методов
# Программист: Барышев Г.А.
# Дата написания: 10.02.2024
import copy
from random import uniform
from time import perf_counter_ns

# Ввод массива
M = int(input('Введите количество столбцов в массиве: '))
N = int(input('Введите количество строк в массиве: '))

print('Выберите, как вы желаете ввести массив: ')
print('1. Заполнение массива псевдослучайными числами')
print('2. Ручное заполнение массива')
inp = int(input('Введите число: ')) 
match inp:
    case 1:
    Z = [0] * N
    for i in range(N):
        s = [0] * M
        for j in range(M):
            s[j] = round(uniform(-100, 100), 2)
        Z[i] = s
    case 2:
        # Ввод матрицы построчно
        Z = [[0] * M] * N
        for i in range(N):
    s = [a for a in map(float, input(f"Введите строку {i}: ").split())]
    Z[i] = s 

else:
    case _:
        print('Ошибка: введён неверный номер выбора')
        exit()

print('Исходный массив: ')
for i in range(N):
    print(Z[i])
print()

# Процедурный метод
max_elem = -1e9
m = 0
n = 0
Z1 = copy.deepcopy(Z)
T1 = perf_counter_ns()
for f in range(1000):
    for i in range(N):
        for j in range(M):
            if Z[i][j] > max_elem:
                max_elem = Z[i][j]
                m = i
                n = j
    for i in range(N):
        for j in range(M):
            if i == m or j == n:
                Z[i][j] = 0
T2 = perf_counter_ns()

Time_result1 = round((T2 - T1)/1000, 4)
# Программа с использованием методов Python
T1 = perf_counter_ns()
for h in range(1000):
    Z2 = copy.deepcopy(Z1)
    max_elem = -1e9
    for i in range(N):
        elem_p = max(Z2[i])
        if elem_p > max_elem:
            max_elem = elem_p
            m = i
            n = j
    for i in range(N):
        for j in range(M):
            if i == m or j == n:
                Z2[i][j] = 0

T2 = perf_counter_ns()

Time_result2 = round((T2 - T1)/1000, 4)
print(f'Максимальный элемент в матрице равен {max_elem} и имеет координаты [{m+1};{n+1}]')
print('Полученный многомерный массив:')
for i in range(N):
    print(Z[i])
print()
print(f'Время работы первого метода равна {Time_result1} нс')
print(f'Время работы второго метода равна {Time_result2} нс')

if Time_result2 > Time_result1:
    print('Первый метод более быстродейственный.')
elif Time_result2 < Time_result1:
    print('Второй метод более быстродейственный.')
else:
    print('Оба метода равносильны по времени.')

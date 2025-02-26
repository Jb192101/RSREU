# Цель: создать программу, находящую индекс элемента, сумма которого с первым элементом максимальна.
# Переменные:
# N – количество элементов в массиве
# inp – переменная для выбора метода заполнения массива
# arrayEnt – исходный массив
# max_sum – максимальная сумма 1го элемента и k-го элемента
# k – индекс нужного элемента
# T1, T2 – начало и конец вычисления времени работы методов
# Time_result1, Time_result2 – время работы обоих методов
# arrayMed – промежуточный массив
# Программист: Барышев Г.А.
# Дата написания: 05.02.2024
from random import uniform
from time import perf_counter_ns

# Ввод массива
N = int(input('Введите количество элементов в массиве: '))

print('Выберите, как вы желаете ввести массив: ')
print('1. Заполнение массива псевдослучайными числами')
print('2. Ручное заполнение массива')
inp = int(input('Введите число: '))
match inp:
    case 1:
        arrayEnt = [0] * N
        for j in range(N):
            arrayEnt[j] = round(uniform(-100, 100), 2)
        print(arrayEnt)
    case 2:
        arrayEnt = [0] * N
        for j in range(N):
            print(f'arrayEnt[{j}] = ', sep = '', end = '')
            arrayEnt[j] = float(input())
        print(arrayEnt)
    case _:
        print('Ошибка: введён неверный номер выбора')
        exit()

print()
# Процедурный метод
max_sum = -1e9
k = 0
T1 = perf_counter_ns()
for f in range(1000):
    for i in range(1, len(arrayEnt) - 1):
        if (arrayEnt[0] + arrayEnt[i]) > max_sum:
            max_sum = arrayEnt[0] + arrayEnt[i]
            k = i
T2 = perf_counter_ns()

Time_result1 = round((T2 - T1)/1000, 4)
print(f'Время работы первого метода равна {Time_result1} нс')

# Программа с использованием методов Python
T1 = perf_counter_ns()
for h in range(1000):
    k = 0
    arrayMed = [0] * N
    arrayMed = sorted(arrayEnt)
    k = arrayEnt.index(arrayMed[N - 1])
T2 = perf_counter_ns()

Time_result2 = round((T2 - T1)/1000, 4)
print(f'Время работы второго метода равна {Time_result2} нс')

print(f'Элемент, сумма которого с первым элементом максимальна, обладает индексом {k}')
if Time_result2 > Time_result1:
    print('Первый метод более быстродейственный.')
elif Time_result2 < Time_result1:
    print('Второй метод более быстродейственный.')
else:
    print('Оба метода равносильны по времени.')

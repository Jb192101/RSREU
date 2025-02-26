# Переменные:
# inp – переменная для выбора метода заполнения матрицы
# B – вводимая матрица
# s - строка, используемая для заполнения матрицы
# max_elem – максимальный элемент в матрице
# elem_p - промежуточный элемент для нахождения максимального элемента матрицы
# BP1, BP2 - два произведения матриц
# B_sq - квадрат матрицы В
# B_tr - транспонирования матрица В
# m1, m2, n1, n2 – индексы максимального и минмального элементов
# T1, T2 – начало и конец вычисления времени работы методов
# Time_result1, Time_result2 – время работы обоих методов
# Программист: Барышев Г.А.
# Дата написания: 11.02.2024
from random import uniform
from time import perf_counter_ns
import numpy as np

def matrix_sq(matrix):
    result_matrix = [[0] * 5] * 5
    for i in range(5):
        for l in range(5):
            s = 0
            for j in range(5):
                s += matrix[i][j] * matrix[j][l]
            result_matrix[i][l] += s

    return result_matrix

def product_matrix(m1, m2):
    result_matrix = [[0] * 5 ] * 5
    for i in range(5):
        for j in range(5):
            for k in range(5):
                result_matrix[i][j] += m1[i][k] * m2[k][j]
    return result_matrix

def transpon_matrix(matrix):
  result_matrix = [[0] * 5] * 5
  for i in range(5):
      for j in range(5):
         result_matrix[j][i] = matrix[i][j]
  return result_matrix

def sum_matrix(m1, m2):
    result_matrix = [[0] * 5] * 5
    for i in range(5):
        for j in range(5):
            result_matrix[i][j] = m1[i][j] + m2[i][j]
    return result_matrix

# Ввод массива
print('Выберите, как вы желаете ввести массив: ')
print('1. Заполнение массива псевдослучайными числами')
print('2. Ручное заполнение массива')
inp = int(input('Введите число: ')) 
match inp:
    case 1:
    	B = [0] * 5
    	for i in range(5):
        	s = [0] * 5
        	for j in range(5):
            		s[j] = round(uniform(-100, 100), 2)
        	B[i] = s
    case 2:
        # Ввод матрицы построчно
        B = [[0] * 5] * 5
        for i in range(5):
            s = [a for a in map(float, input(f"Введите строку {i}: ").split())]
            B[i] = s
else:
    case _:
        print('Ошибка: введён неверный номер выбора')
        exit()

print('Исходный массив: ')
for i in range(5):
    print(B[i])
print()

# Процедурный метод
T1 = perf_counter_ns()
B_sq = matrix_sq(B)
B_tr = transpon_matrix(B)
BP1 = product_matrix(B_tr, B)
BP2 = product_matrix(B, B_tr)
A = sum_matrix(sum_matrix(B_sq, BP1), sum_matrix(BP2, B))
max_elem = A[0][1]
m1 = 0
n1 = 1
for i in range(4):
  for j in range(i + 1, 5):
      if A[i][j] > max_elem:
         max_elem = A[i][j]
         m1 = i
         n1 = j

min_elem = A[1][0]
m2 = 1
n2 = 0
for i in range(2, 5):
  for j in range(1, i):
     if A[i][j] < min_elem:
         min_elem = A[i][j]
         m2 = i
         n2 = j
# Замена
A[m1][n1] = min_elem
A[m2][n2] = max_elem

T2 = perf_counter_ns()
Time_result1 = round((T2 - T1), 4)

# Решение с помощью numpy
T1 = perf_counter_ns()
B_t = np.transpose(B)
max_elem = -1e9
m1 = 0
n1 = 0
A = np.dot(B, B) + np.dot(B, B_t) + np.dot(B_t, B) + B
for i in range(4):
  for j in range(i + 1, 5):
      if A[i][j] > max_elem:
         max_elem = A[i][j]
         m1 = i
         n1 = j

min_elem = A[1][0]
m2 = 0
n2 = 0
for i in range(2, 5):
  for j in range(1, i):
     if A[i][j] < min_elem:
         min_elem = A[i][j]
         m2 = i
         n2 = j
# Замена
A[m1][n1] = min_elem
A[m2][n2] = max_elem
T2 = perf_counter_ns()
Time_result2 = round((T2 - T1), 4)
for i in range(5):
    print(A[i])
print()

print(f'Максимальное значение, лежащее выше главной диагонали, равно {max_elem} и имеет координаты [{m1+1};{n1+1}]')
print(f'Минимальное значение, лежащее ниже главной диагонали, равно {min_elem} и имеет координаты [{m2+1};{n2+1}]')
print()
print(f'Время работы первого метода равна {Time_result1} нс')
print(f'Время работы второго метода равна {Time_result2} нс')

if Time_result2 > Time_result1:
  print('Первый метод более быстродейственный.')
elif Time_result2 < Time_result1:
  print('Второй метод более быстродейственный.')
else:
  print('Оба метода равносильны по времени.')

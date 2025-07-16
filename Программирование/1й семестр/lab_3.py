import math
# Цель: вычисление углов бета и гамма
# Переменные:
# x1, x2, x3 - координаты векторов по оси ОХ
# y1, y2, y3 - координаты векторов по оси ОY
# beta_ugol2, gamma_ugol2 - значение углов бета и гамма в градусах
# Метод: вычисление через тригонометрические формулы
# Программист: Барышев Г.А.
# Дата написания: 17.09.2023

# Определение косинуса
def X(a, b, c, d):
    s = (a*b + c*d)/(math.sqrt(a*a + c*c)*math.sqrt(b*b + d*d))
    return s

# Определение угла в радианах
def ugol(e):
    f = math.atanh(math.sqrt(1 - e*e)/e)
    return f
# Определение угла в градусах
def ugol_g(g):
    q = 180*g/math.pi
    return q

# Значения переменных
# Иксы
x1 = 2
x2 = 7
x3 = 9
# Игрики
y1 = 5
y2 = 6
y3 = 3

# Определение значения угла бета
betaX = X(x1, x2, y1, y2)
beta_ugol = ugol(betaX)
beta_ugol2 = round(ugol_g(beta_ugol), 2)

# Определение значения угла гамма
gammaX = X(x2, x3, y2, y3)
gamma_ugol = ugol(gammaX)
gamma_ugol2 = round(ugol_g(gamma_ugol), 2)

print("Угол бета равен ", beta_ugol2, " градусов, угол гамма ", gamma_ugol2, "градусов.")

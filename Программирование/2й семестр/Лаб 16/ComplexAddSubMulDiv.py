# Модуль для сложения, вычитания, умножения и деления двух комплексных чисел

# Сложение
def cadd(c1, c2):
    realPart1, imagPart1 = c1.real, c1.imag
    realPart2, imagPart2 = c2.real, c2.imag
    RPart = realPart1 + realPart2
    IPart = imagPart1 + imagPart2
    return complex(RPart, IPart)

# Вычитание
def csub(c1, c2):
    realPart1, imagPart1 = c1.real, c1.imag
    realPart2, imagPart2 = c2.real, c2.imag
    RPart = realPart1 - realPart2
    IPart = imagPart1 - imagPart2
    return complex(RPart, IPart)

# Умножение
def cmul(c1, c2):
    realPart1, imagPart1 = c1.real, c1.imag
    realPart2, imagPart2 = c2.real, c2.imag
    RPart = (realPart1 * realPart2) - (imagPart1 * imagPart2)
    IPart = (imagPart1 * realPart2) + (realPart1 * imagPart2)
    return complex(RPart, IPart)

# Деление
def cdiv(c1, c2):
    realPart1, imagPart1 = c1.real, c1.imag
    realPart2, imagPart2 = c2.real, c2.imag
    k = realPart2 * realPart2 + imagPart2 * imagPart2
    RPart = ((realPart1 * realPart2) + (imagPart1 * imagPart2))/k
    IPart = ((imagPart1 * realPart2) - (realPart1 * imagPart2))/k
    return complex(RPart, IPart)

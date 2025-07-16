# Модуль для определения модуля комплексного числа

# Модуль числа
def cmodule(c):
    realPart, imagPart = c.real, c.imag
    module_n = (realPart * realPart + imagPart * imagPart) ** 0.5
    return module_n

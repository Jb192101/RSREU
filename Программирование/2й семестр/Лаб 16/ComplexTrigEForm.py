# Модуль для представления комплексного числа в тригоном. и показ. формах
from math import acos
from ComplexPy import ComplexMod

# Тригонометрическая форма записи
def ctrig(c):
    realPart, imagPart = c.real, c.imag
    r = ComplexMod.cmodule(c)
    ccos = realPart / r
    csin = imagPart / r
    print(f"Тригонометрическая форма записи комплексного числа: {round(r, 3)} * ({round(ccos, 3)} + i * {round(csin, 3)})")

# Показательная форма записи
def ce(c):
    realPart= c.real
    r = ComplexMod.cmodule(c)
    u = acos(realPart / r)
    print(f'Показательная форма записи комплексного числа: {round(r, 3)} * e^(i*{round(u, 3)})')

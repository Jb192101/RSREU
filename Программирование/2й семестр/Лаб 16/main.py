# Основной файл лабораторной работы номер 16
from ComplexPy import *

def menu():
    try:
        print("____МЕНЮ____")
        print("1. Сложение комплексных чисел")
        print("2. Вычитание комплексных чисел")
        print("3. Умножение комплексных чисел")
        print("4. Деление комплексных чисел")
        print("5. Модуль комплексного числа")
        print("6. Тригоном. форма записи комплексного числа")
        print("7. Показательная форма записи комплексного числа")
        print("8. Выход из программы")
        choice = int(input("Введите номер операции: "))
    except ValueError:
        print("Неверно введённые данные!")
    return choice

choice = 0
while choice != 8:
    choice = menu()
    match choice:
        case 1:
            c1 = complex(input("Введите 1-е комплексное число: "))
            c2 = complex(input("Введите 2-е комплексное число: "))
            sum = ComplexAddSubMulDiv.cadd(c1, c2)
            print(f'Сложение двух комплексных чисел равно {sum}')
        case 2:
            c1 = complex(input("Введите 1-е комплексное число: "))
            c2 = complex(input("Введите 2-е комплексное число: "))
            sub = ComplexAddSubMulDiv.csub(c1, c2)
            print(f'Вычитание двух комплексных чисел равно {sub}')
        case 3:
            c1 = complex(input("Введите 1-е комплексное число: "))
            c2 = complex(input("Введите 2-е комплексное число: "))
            mult = ComplexAddSubMulDiv.cmul(c1, c2)
            print(f'Умножение двух комплексных чисел равно {mult}')
        case 4:
            c1 = complex(input("Введите 1-е комплексное число: "))
            c2 = complex(input("Введите 2-е комплексное число: "))
            div = ComplexAddSubMulDiv.cdiv(c1, c2)
            print(f'Деление двух комплексных чисел равно {div}')
        case 5:
            c = complex(input("Введите комплексное число: "))
            module_n = ComplexMod.cmodule(c)
            print(f'Модуль комплексного числа равен {module_n}')
        case 6:
            c = complex(input("Введите комплексное число: "))
            ComplexTrigEForm.ctrig(c)
        case 7:
            c = complex(input("Введите комплексное число: "))
            ComplexTrigEForm.ce(c)

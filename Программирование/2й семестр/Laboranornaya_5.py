# Цель: создать программу, определяющая самое длинное и самое короткое слово в тексте
# Переменные:
# inpStr - вводимая строка
# length_s - длина строки
# words1, words2 - массивы слов в строке
# new_word - текущее исследуемое в цикле слово
# s - символ
# k - длина массива слов
# max_size, min_size - макс. и мин. размеры слов
# long_word, short_word - самые длинное и короткое слова
# Программист: Барышев Г.А.
# Дата написания: 29.02.2024
import re

inpStr = input('Введите строку: ')
# Эхо-вывод
print(f'Исходная строка: {inpStr} \n')
length_s = len(inpStr)
# Программа без использования регулярных выражений
words1 = []
new_word = ''
for j in range(length_s):
    s = inpStr[j]
    if s != ' ':
        new_word += s
        if j == length_s - 1:
            words1.append(new_word)
            new_word = ''
    else:
        words1.append(new_word)
        new_word = ''

k = len(words1)
max_size = 0
long_word = ''
min_size = 1000
short_word = ''
for g in range(k):
    if len(words1[g]) > max_size:
        long_word = words1[g]
        max_size = len(words1[g])

    if len(words1[g]) < min_size:
        short_word = words1[g]
        min_size = len(words1[g])

print(f'Самое длинное слово в данной строке: {long_word}')
print(f'Самое короткое слово в данной строке: {short_word} \n')
# Программа с регулярными выражениями
words2 = re.split(r' ', inpStr)
max_size = 0
long_word = ''
min_size = 1000
short_word = ''
for i in range(len(words2)):
    if len(words2[i]) > max_size:
        long_word = words2[i]
        max_size = len(words2[i])

    if len(words2[i]) < min_size:
        short_word = words2[i]
        min_size = len(words2[i])

print(f'Самое длинное слово в данной строке: {long_word}')
print(f'Самое короткое слово в данной строке: {short_word}')

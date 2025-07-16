from random import randrange
import copy

n = int(input("Введите количество строк: "))
f = open("Data.txt", "w")

for _ in range(n):
    cnt = randrange(10, 21)
    s = "".join([chr(randrange(ord("A"), ord("G") + 1)) for _ in range(cnt)]) + "".join([str(randrange(0, 10)) for _ in range(cnt)])
    f.write(s + "\n")
f.close()

# Проверка
f = open("Data.txt", "r")
# Нахождение самого большого нечётного числа
last_1number = 0
strs = []
max_non2 = []
for s in f:
    d = []
    for i in range(len(s)):
        d.append(s[i])
    for j in range(len(d)//2, len(d) - 1):
        d[j] = int(d[j])
        if d[j] % 2 != 0:
            last_1number = j
    max_non2.append(int(s[len(d)//2:last_1number+1]))
    strs.append(s)

max_el = 0
max_el_ind = 0
for i in range(len(max_non2)):
    if max_non2[i] > max_el:
        max_el = max_non2[i]
        max_el_ind = i
#Наибольшая последовательность
l = len(strs[max_el_ind])
string_s = strs[max_el_ind][:l//2]
liters = []
prop = 0
for i in range(len(string_s)-1):
    if ord(string_s[i]) <= ord(string_s[i+1]) and prop == 0:
        liters.append(string_s[i])
    elif ord(string_s[i]) <= ord(string_s[i+1]) and prop > 0:
        liters = []
        liters.append(string_s[i])
    else: 
        prop += 1


f.close()
# Для ответа/
f = open("Result.txt", "w", encoding="utf-8")
count_max = len(liters)
f.write("Искомая строка - {}.\n".format(max_el_ind))
f.write("Искомое число - {}.\n".format(max_el))
f.write("Самая длинная последовательность из букв: \n")
for fg in range(count_max):
    f.write(liters[fg])
f.close()

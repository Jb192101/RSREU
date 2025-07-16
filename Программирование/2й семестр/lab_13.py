import turtle

# Основная программа
# Создание класса дорожного знака
RoadSign = turtle.Turtle()
RoadSign.speed(0)
RoadSign.shape("classic")
RoadSign.pencolor("red")
RoadSign.fillcolor("white")
RoadSign.pensize(20)

# Перемещение на точку
RoadSign.penup()
RoadSign.goto(0, -200)
RoadSign.pendown()

#Рисование обода знака
RoadSign.circle(200)
RoadSign.penup()
RoadSign.goto(0, -220)
RoadSign.pendown()
RoadSign.pensize(5)
RoadSign.color("black")
RoadSign.circle(220)
RoadSign.penup()

# Внутренняя часть круга
RoadSign.goto(75, -110)
RoadSign.pendown()
RoadSign.pencolor("black")
RoadSign.fillcolor("black")
RoadSign.begin_fill()
RoadSign.circle(20)
RoadSign.end_fill()
RoadSign.penup()
RoadSign.goto(95, -90)
RoadSign.pendown()
RoadSign.begin_fill()
for j in range(4):
    RoadSign.lt(90)
    RoadSign.fd(40)
RoadSign.end_fill()
RoadSign.penup()

RoadSign.goto(-75, -110)
RoadSign.pendown()
RoadSign.begin_fill()
RoadSign.circle(20)
RoadSign.end_fill()
RoadSign.penup()
RoadSign.goto(-55, -90)
RoadSign.pendown()
RoadSign.begin_fill()
for j in range(4):
    RoadSign.left(90)
    RoadSign.fd(40)
RoadSign.end_fill()
RoadSign.penup()

# Корпус авто
RoadSign.goto(-100, -40)
RoadSign.pendown()
RoadSign.begin_fill()
for k in range(4):
    if k % 2 == 0:
        RoadSign.fd(200)
    else:
        RoadSign.fd(75)
    RoadSign.lt(90)
RoadSign.end_fill()
RoadSign.penup()

# Крыша
RoadSign.pensize(10)
RoadSign.pencolor("black")
RoadSign.goto(-90, 35)
RoadSign.pendown()
RoadSign.lt(60)
RoadSign.fd(80)
RoadSign.rt(60)
RoadSign.fd(100)
RoadSign.rt(60)
RoadSign.fd(80)
RoadSign.lt(60)
RoadSign.penup()

# Фары
RoadSign.pencolor("white")
RoadSign.fillcolor("white")
RoadSign.goto(65, -15)
RoadSign.pendown()
RoadSign.begin_fill()
RoadSign.circle(15)
RoadSign.end_fill()
RoadSign.penup()

RoadSign.goto(-65, -15)
RoadSign.pendown()
RoadSign.begin_fill()
RoadSign.circle(15)
RoadSign.end_fill()
RoadSign.penup()

# Удержание изображения на экране
turtle.done()

FROM ubuntu:22.04

# Установка необходимых пакетов
RUN apt-get update && apt-get install -y \
    golang-go \
    libgl1-mesa-dev \
    xorg-dev \
    libgtk-3-dev \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

# Установка переменных окружения
ENV GO111MODULE=on
ENV DISPLAY=:0
ENV XAUTHORITY=/tmp/.Xauthority

# Создание рабочей директории
WORKDIR /app

# Копирование исходного кода
COPY main.go .

# Скачивание зависимостей
RUN go mod init proc-viewer
RUN go mod tidy

# Компиляция приложения
RUN go build -o proc-viewer .

# Команда для запуска
CMD ["./proc-viewer"]
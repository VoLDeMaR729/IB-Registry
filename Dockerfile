FROM ubuntu:22.04

# Отключаем вопросы при установке
ENV DEBIAN_FRONTEND=noninteractive

# 1. Установка зависимостей
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    qt6-base-dev \
    libqt6sql6-psql \
    libpq-dev \
    libspdlog-dev \
    nlohmann-json3-dev \
    libgl1-mesa-dev \
    libxkbcommon-x11-0 \
    && rm -rf /var/lib/apt/lists/*

# 2. Рабочая директория
WORKDIR /app

# 3. Копируем файлы проекта
COPY CMakeLists.txt .
COPY src ./src
COPY config ./config

# 4. Сборка проекта
RUN mkdir build && cd build && \
    cmake .. && \
    make -j$(nproc)

# 5. Команда запуска
CMD ["./build/ib_registry_app"]

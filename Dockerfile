# Используем базовый образ Python
FROM python:3.12.3

# Устанавливаем зависимости системы
RUN apt-get update && apt-get install -y \
    libpq-dev \
    build-essential

# Устанавливаем рабочую директорию
WORKDIR /app

# Копируем файлы requirements.txt и устанавливаем зависимости Python
COPY requirements.txt /app/
RUN pip install --no-cache-dir -r requirements.txt

# Копируем остальные файлы проекта
COPY . /app/

# Устанавливаем переменную окружения для Django
ENV DJANGO_SETTINGS_MODULE=system_info_from_bike.settings

# Выполняем миграции, собираем статику и запускаем приложение
CMD ["sh", "-c", "python manage.py migrate && python manage.py collectstatic --noinput && python manage.py runserver 0.0.0.0:8000"]

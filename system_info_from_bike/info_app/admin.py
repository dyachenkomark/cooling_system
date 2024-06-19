from django.contrib import admin
from .models import DataPoint

# Зарегистрируйте вашу модель в панели администратора, чтобы её было видно в админке
admin.site.register(DataPoint)

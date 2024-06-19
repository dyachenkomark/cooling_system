from django.urls import path
from .views import plot_view

urlpatterns = [
    path('plot/', plot_view, name='plot'),
]

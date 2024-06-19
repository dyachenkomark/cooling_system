from django.shortcuts import render
from .models import DataPoint
import matplotlib
matplotlib.use('Agg')  # Использование бэкэнда, который не требует GUI
import matplotlib.pyplot as plt
import io
import urllib, base64

def plot_view(request):
    data = DataPoint.objects.all()
    temp_in = [d.temp_in for d in data]
    temp_out = [d.temp_out for d in data]
    delta_temp = [tin - tout for tin, tout in zip(temp_in, temp_out)]

    # Первый график - Температура на входе и выходе
    plt.figure(figsize=(12, 8))  # Увеличение размера графика
    plt.plot(temp_in, label='Температура на входе')
    plt.plot(temp_out, label='Температура на выходе')
    plt.xlabel('Пункт данных')
    plt.ylabel('Температура')
    plt.legend()
    plt.title('График температур')
    buf1 = io.BytesIO()
    plt.savefig(buf1, format='png')
    buf1.seek(0)
    string1 = base64.b64encode(buf1.read())
    uri1 = urllib.parse.quote(string1)
    plt.close()

    # Второй график - Δ температур
    plt.figure(figsize=(10, 8))  # Увеличение размера графика
    plt.plot(delta_temp, label='Δ температур', color='r')
    plt.xlabel('Пункт данных')
    plt.ylabel('Δ Температур')
    plt.legend()
    plt.title('График Δ температур')
    buf2 = io.BytesIO()
    plt.savefig(buf2, format='png')
    buf2.seek(0)
    string2 = base64.b64encode(buf2.read())
    uri2 = urllib.parse.quote(string2)
    plt.close()

    return render(request, 'info_app/plot.html', {'data1': uri1, 'data2': uri2})

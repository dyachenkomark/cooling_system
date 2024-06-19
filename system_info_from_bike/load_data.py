import pandas as pd
import os
os.environ.setdefault("DJANGO_SETTINGS_MODULE", "system_info_from_bike.settings")
import django
django.setup()
from info_app.models import DataPoint

def load_data_from_excel(file_path):
    data = pd.read_excel(file_path)
    for _, row in data.iterrows():
        DataPoint.objects.create(
            number=row['номер'],
            temp_in=row['температура на входе'],
            temp_out=row['температура выходе'],
            flow=row['поток']
        )

if __name__ == "__main__":
    load_data_from_excel('data.xlsx')

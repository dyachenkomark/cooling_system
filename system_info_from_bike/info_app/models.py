from django.db import models

class DataPoint(models.Model):
    number = models.IntegerField()
    temp_in = models.FloatField()
    temp_out = models.FloatField()
    flow = models.FloatField()

    def __str__(self):
        return f"DataPoint {self.number}"
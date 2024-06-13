#define THERMISTOR_PIN0 A0  // К какому аналоговому пину подключены
#define THERMISTOR_PIN1 A1
#define THERMISTOR_NOMINAL 10000  // Сопротивление при 25 C°
#define TEMPERATURE_NOMINAL 25  // Температура для номинального сопротивления (обычно 25 C°)
#define NUM_SAMPLES 25  // Сколько показаний используем для определения среднего значения
#define B_COEFFICIENT 4300  // Бета коэффициент термистора (обычно 3000-4000)
#define SERIES_RESISTOR 10000  // Сопротивление второго резистора

float n;  // Поправочный коэффициент (искомый жи ес)
float m;  // Температура датчика 1
float a;  // Температура датчика 2
float r;  // Коэффициент для температуры 1
float k;  // Коэффициент для температуры 2

int samples[NUM_SAMPLES];
int row_excel = 0;  // Количество строк

volatile int flow_frequency;  // Частота импульсов датчика расхода
float l_hour;  // Рассчитанный расход в литрах/час
unsigned char flow_sensor = 2;  // Вход датчика
unsigned long current_time;
unsigned long cloop_time;

// Функция прерывания для датчика расхода
void flow() {
    flow_frequency++;
}

void setup() {  
    m = 12;
    a = 40;
    r = 1.51;
    k = 1.4;

    Serial.begin(9600);
    
    Serial.println("CLEARDATA");  // Очистка листа Excel
    Serial.println("LABEL,Time,C1,C2,Flow,Nomer");  // Заголовки столбцов

    pinMode(flow_sensor, INPUT);
    digitalWrite(flow_sensor, HIGH);  // Внутренний подтягивающий резистор
    attachInterrupt(0, flow, RISING);  // Настройка прерывания
    sei();  // Включение прерываний
    current_time = millis();
    cloop_time = current_time;
    analogReference(EXTERNAL);

    delay(500);
}

void loop() {
    current_time = millis();

    // Каждые 5 секунд рассчитываем и выводим литры в час
    if (current_time >= (cloop_time + 5000)) {
        cli();
        cloop_time = current_time;  // Обновляем cloop_time

        // Частота импульсов (Гц) = 7.5Q, где Q - расход в л/мин
        l_hour = (flow_frequency * (60.0f / 7.5f) / 5 * n);  // Вычислим расход, л/ч
        flow_frequency = 0;  // Сбрасываем счётчик
        sei();
    } else {
        return;
    }

    uint8_t i;
    float average;

    // Считываем показания с первого термистора
    for (i = 0; i < NUM_SAMPLES; i++) {
        samples[i] = analogRead(THERMISTOR_PIN0);
        delay(10);
    }

    // Расчёт среднего значения
    average = 0;
    for (i = 0; i < NUM_SAMPLES; i++) {
        average += samples[i];
    }
    average /= NUM_SAMPLES;

    // Преобразование значения в сопротивление
    average = 1023 / average - 1;
    average = SERIES_RESISTOR / average;

    // Применение формулы Штейнгарта-Харта для вычисления температуры
    float steinhart;
    steinhart = average / THERMISTOR_NOMINAL;  // (R/Ro)
    steinhart = log(steinhart);  // ln(R/Ro)
    steinhart /= B_COEFFICIENT;  // 1/B * ln(R/Ro)
    steinhart += 1.0 / (TEMPERATURE_NOMINAL + 273.15);  // + (1/To)
    steinhart = 1.0 / steinhart;  // Инвертируем
    steinhart -= 273.15;  // Конвертируем в градусы по Цельсию

    // Считываем показания со второго термистора
    for (i = 0; i < NUM_SAMPLES; i++) {
        samples[i] = analogRead(THERMISTOR_PIN1);
        delay(10);
    }

    // Расчёт среднего значения
    average = 0;
    for (i = 0; i < NUM_SAMPLES; i++) {
        average += samples[i];
    }
    average /= NUM_SAMPLES;

    // Преобразование значения в сопротивление
    average = 1023 / average - 1;
    average = SERIES_RESISTOR / average;

    // Применение формулы Штейнгарта-Харта для вычисления температуры второго датчика
    float steinhart1;
    steinhart1 = average / THERMISTOR_NOMINAL;  // (R/Ro)
    steinhart1 = log(steinhart1);  // ln(R/Ro)
    steinhart1 /= B_COEFFICIENT;  // 1/B * ln(R/Ro)
    steinhart1 += 1.0 / (TEMPERATURE_NOMINAL + 273.15);  // + (1/To)
    steinhart1 = 1.0 / steinhart1;  // Инвертируем
    steinhart1 -= 273.15;  // Конвертируем в градусы по Цельсию

    // Расчёт поправочного коэффициента
    n = r + (steinhart - m) * ((k - r) / (a - m));

    row_excel++;
    Serial.print("DATA,TIME,");
    Serial.print(steinhart);
    Serial.print(",");
    Serial.print(steinhart1);
    Serial.print(",");
    Serial.print(l_hour);
    Serial.print(",");
    Serial.println(row_excel);
    Serial.println(" ");
}

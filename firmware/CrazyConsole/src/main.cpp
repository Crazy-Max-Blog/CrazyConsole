#include "tft.h"

void setup()
{
    Serial.begin(115200);
    pinMode(4, INPUT_PULLUP); // Инициализация первой кнопки
    pinMode(5, INPUT_PULLUP); // Инициализация второй кнопки
    tft_init(); // Инициализируем дисплей
}

bool run = true;
void loop()
{
    if (run) {
        tft_tick(); // Обновляем экран
        // Обработка второй кнопки
        static bool b1s = false;
        bool b1 = !digitalRead(4);
        if (b1s != b1) {
            b1s = b1;
            if (b1) jump(); // Делаем прыжок
        }
    }
    // Обработка второй кнопки
    static bool b2s = false;
    bool b2 = !digitalRead(5);
    if (b2s != b2) {
        b2s = b2;
        if (b2) run=!run; // Ставим на паузу / запускаем опратно
    }
}
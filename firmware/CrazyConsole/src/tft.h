#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

#include "images.h"

// SCL->D5
// SDA->D7
// BLK->3V3
#define TFT_CS D3
#define TFT_DC D8
#define TFT_RST D6

int lives = 3;

Adafruit_ST7735 tft(TFT_CS, TFT_DC, TFT_RST);

struct {
    int w = 23; // Ширина Чебурашки
    int h = 20; // Высота Чебурашки
    int x = 0;  // Позиция Чебурашки по X
    int y = 128 - h - 16; // Позиция Чебурашки по Y
    bool n = true;  // Направление движения Чебурашки (true - вверх, false - вниз)
    void restart() {y = 128 - h - 16; n = true;}
} ch_data;

struct {
    int w = 20;  // Ширина Лариски
    int h = 20;  // Высота Лариски
    int x = 160; // Позиция Лариски по X
    int y = 128 - h - 16; // Позиция Лариски по Y
    bool bonus = false; // Является ли эта Лариска скрытым бонусом
    void restart() {x = 160;bonus = (random(5) == 0);}
} la_data;

void tft_print_lives() {
    for (int i=0; i<3; i++) {
        int x = 160 - 20 * (i + 1);
        if (i < lives) {
            tft.drawRGBBitmap(x, 0, orange, 20, 20);
        } else {
            tft.fillRect(x, 0, 20, 20, 0xFFFF);
        }
    }
}

void tft_start() {
    tft.drawRGBBitmap(0, 0, background, 160, 128);
    for(int i=0; i<15; i++) tft.drawRGBBitmap(i*32, 128-16, brick, 32, 16);
    ch_data.restart();
    la_data.restart();
    tft_print_lives();
}

void game_ower() {
    tft.fillScreen(0);
    tft.setCursor(0, 0);
    tft.print("Game Ower!");
    delay(2500);
    lives = 3;
    tft_start();
}

void tft_init()
{
    SPI.setFrequency(8000000ul);
    tft.initR(INITR_BLACKTAB);
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextColor(ST77XX_GREEN);
    tft.setRotation(1);
    tft.fillScreen(0xFFFF);

    // Рисуем заставку
    int w = 120;
    int h = 65;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
          tft.drawFastVLine(x + 20, y + 32, 128 - y - 1, pgm_read_word(&label[y * w + x]));
        }
    }
    delay(500);
    tft_start();
}

bool _jump = false;
void jump() { _jump = true; }

bool point_in_square(int xp, int yp, int x, int y, int w, int h) {
    return x <= xp and xp <= (x+w) and y <= yp and yp <= (y+h);
}

void game_ower();

void tft_tick() {
    // Движение Чебурашки
    tft.drawRGBBitmap(ch_data.x, ch_data.y, cheburashka, ch_data.w, ch_data.h);
    if (_jump) {
        ch_data.y += ch_data.n ? -1 : 1;
        
        tft.drawFastHLine(ch_data.x, ch_data.y + (ch_data.n ? ch_data.h:-1), ch_data.w, 0xFFFF);
        if (ch_data.y == 128 - 3.5 * ch_data.h - 16 || ch_data.y == 128 - ch_data.h - 16) { // Если вышел из границ
            ch_data.n = !ch_data.n;
            if (ch_data.n) _jump = false;
        }
    }

    // Движение Лариски
    la_data.x--;
    if (la_data.x == -20) la_data.restart();
    tft.drawRGBBitmap(la_data.x, la_data.y, (la_data.x < 64 and la_data.bonus) ? orange : lariska, la_data.w, la_data.h);
    tft.drawFastVLine(la_data.x + la_data.w, la_data.y, la_data.h, 0xFFFF);

    // Проверка на столкновение
    bool b = point_in_square(la_data.x, la_data.y, ch_data.x, ch_data.y, ch_data.w, ch_data.h) or
        point_in_square(la_data.x+la_data.w, la_data.y, ch_data.x, ch_data.y, ch_data.w, ch_data.h) or
        point_in_square(la_data.x, la_data.y+la_data.h, ch_data.x, ch_data.y, ch_data.w, ch_data.h) or
        point_in_square(la_data.x+la_data.w, la_data.y+la_data.h, ch_data.x, ch_data.y, ch_data.w, ch_data.h);
    static bool s = false;
    if (b and !s) {
        s = true;
        if (la_data.bonus) {
            if (lives < 3) lives++;
        } else lives--;
        tft_print_lives();
        if (lives == 0) game_ower();
    } else if (!b and s) {
        s = false;
    }
}
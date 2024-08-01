/*
フラッシュザウルスもどき
https://github.com/ohguma/mock_flash_zaurus
2024-07-22 ohguma
*/

#ifndef __EFFECT_H
#define __EFFECT_H


#include "led.h"
#include "music.h"

#define PIN_TONE 11  // パッシブブザー接続ピン

//花火パターン
void effect_firework(int num_loop) {
  int num_tane1 = 8;
  int num_tane2 = 20;
  int top;
  int tail;
  unsigned long tm = millis();
  unsigned long tm_s, blow1, blow2;  //音用
  unsigned long freq = 0;
  unsigned long tane1[num_tane1] = {};
  unsigned long tane2[num_tane2] = {};
  long t, p, k, x, y;
  for (int h = 0; h < num_loop; h++) {
    int firsthue = 32 * random(8) + random(32);
    //打ち上げ
    for (int i = 0; i < num_tane1; i++) {
      int v = 224 * pow(0.7, i) + 31;
      //tane1[i] = pixels.Color(v, v, v);
      tane1[i] = pixels.ColorHSV((firsthue + i * 3) * 255, 160, v);
    }
    tm = millis();
    top = 70 + random(4) * 10;
    t = 300 + random(3) * 200;  //周期[ms]
    p = top + num_tane1;        //0.5t 時の頂点  x=0.5t時に -x*(x-t) / k = p
    k = t * t / 4 / p;
    x = 0;
    tm_s = 0;
    while (x <= t / 2) {
      if (millis() > tm_s) {
        //打ち上げ音
        tm_s = millis() + 10;
        freq = 200 + x * 3;  //map(x, 0, t / 2, 200, 500);
        tone(PIN_TONE, 1000000 / freq);
        delay(5);
        noTone(PIN_TONE);
      }
      pixels.clear();
      x = millis() - tm;
      y = -x * (x - t) / k;  //2次曲線。放物線
      for (int i = 0; i < num_tane1; i++) {
        if (y - i > 0 && y - i < top) {
          pixels.setPixelColor(y - i, tane1[i]);
        }
      }
      pixels.show();
    }
    delay(100 * random(3) + 100);
    //爆発
    for (int i = 0; i < num_tane2; i++) {
      int v = 224 * pow(0.9, i) + 31;
      tane2[i] = pixels.ColorHSV((firsthue + i * 3) * 255, 128, v);
    }
    tm = millis();
    tail = 30 + random(3) * 5;
    t = 400 + random(3) * 100;  //周期[ms]
    p = tail + num_tane2;       //0.5t 時の頂点  x=0.5t時に -x*(x-t) / k = p
    k = 3 / 2 * t * t / 4 / p;
    x = 0;
    tm_s = 0;
    while (x <= t / 2) {
      if (millis() > tm_s) {
        //爆発音
        blow1 = random(200, 500);
        blow2 = random(1, 5);
        tm_s = millis() + blow2;
        tone(PIN_TONE, blow1);
        delay(blow2);
        noTone(PIN_TONE);
      }
      pixels.clear();
      x = millis() - tm;
      y = -x * (x - t) / k;  //2次曲線。放物線
      for (int i = 0; i < num_tane2; i++) {
        if (y - i > 0 && y - i < tail) {
          if (i % 2 == 0) {
            pixels.setPixelColor(top + (y - i) / 2, tane2[i]);
          }
          pixels.setPixelColor(top - (y - i), tane2[i]);
        }
      }
      pixels.show();
    }
    delay(100 * random(3) + 200);
  }
  pixels.clear();
  pixels.show();
  delay(1000);
}

// スタートシグナル
void sound_start() {
  int w1 = 4;
  int w2 = 1;
  for (int i = 0; i < 3; i++) {
    tone(PIN_TONE, 440);
    for (int j = 0; j < w1; j++) {
      pixels.setPixelColor(i * (w1 + w2) + j, color_ng);
    }
    pixels.show();
    delay(100);
    noTone(PIN_TONE);  //音止める
    delay(900);
  }
  tone(PIN_TONE, 880);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < w1; j++) {
      pixels.setPixelColor(i * (w1 + w2) + j, 0);
    }
  }
  pixels.show();
  delay(1400);
  noTone(PIN_TONE);  //音止める
  delay(100);
}

//当たり音
void sound_ok() {
  tone(PIN_TONE, 659);  //NOTE_E5
  delay(100);
  tone(PIN_TONE, 554);  //NOTE_CS5
  delay(100);
  noTone(PIN_TONE);  //音止める
}

//ハズレ音
void sound_ng() {
  tone(PIN_TONE, 110);
  delay(100);
  noTone(PIN_TONE);  //音止める
  delay(100);
  tone(PIN_TONE, 110);
  delay(200);
  noTone(PIN_TONE);  //音止める
}

#endif

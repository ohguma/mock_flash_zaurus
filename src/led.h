/*
フラッシュザウルスもどき
https://github.com/ohguma/mock_flash_zaurus
2024-07-22 ohguma
*/

#ifndef __LED_H
#define __LED_H

#include <Adafruit_NeoPixel.h>

#define LED_NUM_PIXELS 144  // LED数
#define PIN_LED 6           // LEDテープ接続ピン

Adafruit_NeoPixel pixels(LED_NUM_PIXELS, PIN_LED, NEO_GRB + NEO_KHZ800);

unsigned long color_fall;   // 落下ドット色
unsigned long color_obi;    // 正解ゾーンの色
unsigned long color_ok;     // 当たり色
unsigned long color_ng;     // ハズレ色
unsigned long color_none;   // 未回答部色
unsigned long color_hi_ok;  // ハイスコア表示用当たり色
unsigned long color_hi_ng;  // ハイスコア表示用ハズレ色

//規定の色設定
void set_color() {
  color_fall = pixels.Color(127, 255, 255);  // 落下ドット色
  color_obi = pixels.Color(255, 255, 0);     // 正解ゾーンの色
  color_ok = pixels.Color(0, 255, 0);        // 当たり色
  color_ng = pixels.Color(255, 0, 0);        // ハズレ色
  color_none = pixels.Color(255, 255, 255);  // 未回答部色
  color_hi_ok = pixels.Color(64, 160, 32);   // ハイスコア表示用当たり色
  color_hi_ng = pixels.Color(160, 64, 32);   // ハイスコア表示用ハズレ色
}

#endif

/*
フラッシュザウルスもどき
https://github.com/ohguma/mock_flash_zaurus
2024-07-03 ohguma

■必要パーツ
・Arduino Uno等
・押ボタンスイッチ
・LEDテープ144連
・パッシブブザー
・ケース、電源等

LEDテープはAdafruit_NeoPixelで利用できるもの。144個を想定
ALITOVE WS2812B LEDテープ1m 144連 NeoPixel RGB TAPE LED 5050 SMD LEDテープライトピクセル 防水 黒いベース 5V DC
https://www.amazon.co.jp/dp/B01MYT6JH9/

パッシブブザー
tone()関数で制御できるもの。電源につなぐだけでは鳴らない。
KKHMF 20個パッシブブザー 電磁 ユニバーサル インピーダンス 16R 12*8.5mm ピッチ 6.5mm
https://www.amazon.co.jp/gp/product/B08LB2GYD1/

押ボタンスイッチは丈夫なものを使う
押しボタンスイッチ AR22シリーズ 突形(Φ24) 非照光式 取付穴径22.3/25.5mm モメンタリ 1a
https://www.monotaro.com/p/3492/4644/

演奏には下記を利用する。
https://github.com/lalaso2000/Music/

*/

#include "music.h"

#include <Adafruit_NeoPixel.h>

#define PIN_SW 4       // スタート・回答スイッチピン
#define PIN_TONE 11    // パッシブブザー接続ピン
#define PIN_LED 6      // LEDテープ接続ピン
#define NUMPIXELS 144  // LED数
Adafruit_NeoPixel pixels(NUMPIXELS, PIN_LED, NEO_GRB + NEO_KHZ800);

#define NUM_RANGE_DEFAULT 15
int pos_ok_begin = 20;                 // 正解ゾーン開始位置。実際はランダム配置
int num_ok_range = NUM_RANGE_DEFAULT;  // 正解ゾーン個数。だんだん減る。

int score = -1;                 // 得点 0～10。-1はまだ誰も挑戦していないとき
int hi_score = -1;              // ハイスコア
unsigned long delay_fall = 1;   // ドット落下時待機時間
unsigned long delay_move = 25;  // ドット移動時待機時間
int fall_hosei = 0;             //落下速度補正。低得点時の高速落下回避用

int light = 1;              // LED明るさ基準値
unsigned long color_fall;   // 落下ドット色
unsigned long color_obi;    // 帯部分の色
unsigned long color_ok;     // 当たり色
unsigned long color_ng;     // ハズレ色
unsigned long color_none;   // 未回答部色
unsigned long color_hi_ok;  // ハイスコア表示用当たり色
unsigned long color_hi_ng;  // ハイスコア表示用ハズレ色

#define NUM_ATARI 10      // 回答数
int atari[NUM_ATARI];     // 回答状態 0:未回答 1:OK 2:NG
int hi_atari[NUM_ATARI];  // ハイスコア回答状態 0:未回答 1:OK 2:NG

//規定の色設定
void set_color() {
  color_fall = pixels.Color(127, 255, 255);   // 落下ドット色
  color_obi = pixels.Color(255, 255, 0);    // 帯部分の色
  color_ok = pixels.Color(0, 255, 0);     // 当たり色
  color_ng = pixels.Color(255, 0, 0);     // ハズレ色
  color_none = pixels.Color(255, 255, 255);   // 未回答部色
  color_hi_ok = pixels.Color(0, 160, 0);  // ハイスコア表示用当たり色
  color_hi_ng = pixels.Color(160, 0, 0);  // ハイスコア表示用ハズレ色
}

void setup() {
  Serial.begin(9600);
  pinMode(PIN_SW, INPUT_PULLUP);
  pixels.begin();
  pixels.show();
  pixels.setBrightness(light * 4);  // 輝度設定 x/255 (max = 255)
  for (int i = 0; i < NUM_ATARI; i++) {
    hi_atari[i] = 0;  //0:未回答,1:OK,2:NG
  }
  //
  while (digitalRead(PIN_SW) == LOW) {
    //スイッチONで明るさ調整
    light = (millis() / 1000) % 5 + 1;
    pixels.setBrightness(light * 4);  // 輝度調整 x/255 (max = 255)
    pixels.clear();
    pixels.setPixelColor(5, pixels.Color(255, 0, 0));
    for (int i = 0; i < light; i++) {
      pixels.setPixelColor(i, pixels.Color(255, 255, 255));
    }
    pixels.show();
    delay(10);
  }
  delay(500);
  set_color();
  pixels.clear();
  pixels.show();
}

//帯位置決定、表示
void disp_obi() {
  //帯オン
  pos_ok_begin = 20 + random(8) * 2;
  for (int i = pos_ok_begin; i < pos_ok_begin + num_ok_range; i++) {
    pixels.setPixelColor(i, color_obi);
  }
}

// 結果表示
void disp_atari(int start = 0) {
  for (int i = 0; i < NUM_ATARI; i++) {
    if (atari[i] == 1) {
      // 1:OK
      pixels.setPixelColor(i + start, color_ok);
    } else if (atari[i] == 2) {
      // 2:NG
      pixels.setPixelColor(i + start, color_ng);
    } else {
      // 0:未回答
      pixels.setPixelColor(i + start, color_none);
    }
  }
}

// スタート音
void sound_start() {
  // 時報風
  for (int i = 0; i < 3; i++) {
    tone(PIN_TONE, 440);
    pixels.setPixelColor(i, color_ng);
    pixels.show();
    delay(100);
    noTone(PIN_TONE);  //音止める
    delay(900);
  }
  tone(PIN_TONE, 880);
  for (int i = 0; i < 3; i++) {
    pixels.setPixelColor(i, 0);
    pixels.show();
  }
  delay(1400);
  noTone(PIN_TONE);  //音止める
  delay(100);
}

//当たり音
void sound_ok() {
  tone(PIN_TONE, 440);  //ラ～(440Hz)
  delay(200);           //1秒間まつ
  noTone(PIN_TONE);     //音止める
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

// Rainbow-enhanced theater marquee.
void theaterChaseRainbow(int period) {
  int wait = 50;
  int repeat_a = period / wait / 12;
  int firstPixelHue = 64;               // First pixel starts at red (hue 0)
  for (int a = 0; a < repeat_a; a++) {  // 
    for (int b = 0; b < 12; b++) {      //  
      pixels.clear();                   //   Set all pixels in RAM to 0 (off)
      for (int c = b; c < pixels.numPixels(); c += 12) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int hue = firstPixelHue + c * 65536L / pixels.numPixels();
        uint32_t color = pixels.gamma32(pixels.ColorHSV(hue));  // hue -> RGB
        pixels.setPixelColor(c, color);                         // Set pixel 'c' to value 'color'
      }
      pixels.show();                // Update strip with new contents
      delay(wait);                  // Pause for a moment
      firstPixelHue += 65536 / 30;  // One cycle of color wheel over 90 frames
    }
  }
  pixels.clear();
  pixels.show();
  delay(1000);
}

void loop() {
  //theaterChaseRainbow(1000);
  int h, i, j;
  // スタート待機
  unsigned long tm = millis();
  unsigned long tm2 = millis();
  long t = 250;   //周期[ms]
  long p = 100;   //0.5t 時の頂点  x=0.5t時に -x*(x-t) / k = p
  long t2 = 333;  //ハイスコア用周期[ms]
  long p2 = 100;  //ハイスコア用0.5t 時の頂点  x=0.5t時に -x*(x-t) / k = p
  if (score == -1) {
    p = (random(10) + 1) * 10;
  } else {
    p = (score + 1) * 10;
    p2 = (hi_score + 1) * 10;
  }
  t = p + 250;
  t2 = p2 + 333;
  long k = t * t / 4 / p;
  long k2 = t2 * t2 / 4 / p2;
  while (digitalRead(PIN_SW) == HIGH) {
    pixels.clear();
    long x = millis() - tm;
    long x2 = millis() - tm2;
    long y = -x * (x - t) / k;       //2次曲線。放物線
    long y2 = -x2 * (x2 - t2) / k2;  //2次曲線。放物線
    if (y < 0) {
      if (score == -1) {
        p = (random(10) + 1) * 10;
        t = p + 250;
        k = t * t / 4 / p;
      }
      tm = millis();
      x = 0;
      y = 0;
    }
    if (y2 < 0) {
      tm2 = millis();
      x2 = 0;
      y2 = 0;
    }
    if (score != -1 && hi_score > 1 && score != hi_score) {
      for (int i = 0; i < NUM_ATARI; i++) {
        if (hi_atari[i] == 1) {
          // 1:OK
          pixels.setPixelColor(i + y2, color_hi_ok);
        } else {
          // 2:NG
          pixels.setPixelColor(i + y2, color_hi_ng);
        }
      }
    }
    disp_atari(y);
    pixels.show();
  }
  // 開始
  score = 0;
  fall_hosei = 0;
  num_ok_range = NUM_RANGE_DEFAULT;
  for (int i = 0; i < NUM_ATARI; i++) {
    atari[i] = 0;  //0:未回答,1:OK,2:NG
  }
  pixels.clear();
  disp_obi();  //初回は帯表示して、開始音
  pixels.show();
  sound_start();
  for (h = 0; h < NUM_ATARI; h++) {
    if (h > 0) {
      pixels.clear();
      disp_obi();
    }
    disp_atari();
    pixels.show();
    if (h > 0) {
      //ランダム待機
      delay(random(3000) + 500);
    }
    //落下速度決定
    delay_fall = pow(random(4) + fall_hosei, 2);
    //落下開始位置決定
    int pos_start = NUMPIXELS - 1 - random(8) * 3;
    //流星開始
    for (i = pos_start; i >= NUM_ATARI; i--) {
      if (i < pos_start) {
        //ドット消し
        if (i + 1 >= pos_ok_begin && i + 1 < pos_ok_begin + num_ok_range) {
          pixels.setPixelColor(i + 1, color_obi);
        } else {
          pixels.setPixelColor(i + 1, 0);
        }
      }
      //新ドット
      pixels.setPixelColor(i, color_fall);
      pixels.show();
      if (digitalRead(PIN_SW) == LOW) {
        break;
      }
      delay(delay_fall);
    }
    // あたり判定
    if (i == NUM_ATARI - 1) {
      // 未回答時のドット位置調整
      i = NUM_ATARI;
    }
    // ドット色変更
    unsigned long c;
    if (i >= pos_ok_begin && i < pos_ok_begin + num_ok_range) {
      //当たり
      score++;
      atari[h] = 1;  //OK
      c = color_ok;
      pixels.setPixelColor(i, c);
      pixels.show();
      sound_ok();
    } else {
      //ハズレ
      atari[h] = 2;  //NG
      c = color_ng;
      pixels.setPixelColor(i, c);
      pixels.show();
      sound_ng();
    }
    //ドット移動
    int k;
    //ドットより奥に帯が残っていれば削除
    if (i < pos_ok_begin + num_ok_range) {
      if (i >= pos_ok_begin) {
        k = i + 1;
      } else {
        k = pos_ok_begin;
      }
      for (j = pos_ok_begin + num_ok_range - 1; j >= k; j--) {
        pixels.setPixelColor(j, 0);
        pixels.show();
        delay(delay_move);
      }
    }
    for (j = i; j > h; j--) {
      //ドット消し
      if (j < NUM_ATARI) {
        pixels.setPixelColor(j, color_none);
      } else {
        pixels.setPixelColor(j, 0);
      }
      //新ドット
      pixels.setPixelColor(j - 1, c);
      pixels.show();
      delay(delay_move);
    }
    num_ok_range--;
    if (h > NUM_ATARI / 2 && score == 0) {
      //提督展示の高速回落下避設定
      fall_hosei++;
    }
    delay(2000);
  }
  //ハイスコア
  if (score > 0 && score >= hi_score) {
    hi_score = score;
    for (int i = 0; i < NUM_ATARI; i++) {
      hi_atari[i] = atari[i];  //0:未回答,1:OK,2:NG
    }
  }
  //終了音
  if (score == NUM_ATARI) {
    playMusic(PIN_TONE, marioClear, 150);
    theaterChaseRainbow(3000);
  } else {
    playMusic(PIN_TONE, marioOver, 150);
  }
}
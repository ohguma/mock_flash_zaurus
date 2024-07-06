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
unsigned long delay_fall = 1;   // ドット落下時待機時間
unsigned long delay_move = 25;  // ドット移動時待機時間

int light = 1;             // LED明るさ基準値
unsigned long color_fall;  // 落下ドット色
unsigned long color_obi;   // 帯部分の色
unsigned long color_ok;    // 当たり色
unsigned long color_ng;    // ハズレ色
unsigned long color_none;  // 未回答部色

#define NUM_ATARI 10   // 回答数
int atari[NUM_ATARI];  // 回答状態 0:未回答 1:OK 2:NG

//明るさ基準値による色を返す
unsigned long get_color_sub(int r, int g, int b) {
  return pixels.Color(r * light, g * light, b * light);
}

//規定の色設定
void set_color() {
  color_fall = get_color_sub(4, 4, 8);  // 落下ドット色
  color_obi = get_color_sub(4, 4, 0);   // 帯部分の色
  color_ok = get_color_sub(0, 4, 0);    // 当たり色
  color_ng = get_color_sub(4, 0, 0);    // ハズレ色
  color_none = get_color_sub(4, 4, 4);  // 未回答部色
}

void setup() {
  Serial.begin(9600);
  pinMode(PIN_SW, INPUT_PULLUP);
  pixels.begin();
  //
  while (digitalRead(PIN_SW) == LOW) {
    //スイッチONで明るさ調整
    light = (millis() / 1000) % 5 + 1;
    pixels.clear();
    pixels.setPixelColor(5, get_color_sub(4, 0, 0));
    for (int i = 0; i < light; i++) {
      pixels.setPixelColor(i, get_color_sub(4, 4, 4));
    }
    pixels.show();
    delay(10);
  }
  delay(500);
  set_color();
  pixels.clear();
  pixels.show();
}

//帯表示
void disp_obi(int dot = -1) {
  for (int i = pos_ok_begin; i < pos_ok_begin + num_ok_range; i++) {
    if (i != dot) {
      pixels.setPixelColor(i, color_obi);
    }
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
    delay(100);
    noTone(PIN_TONE);  //音止める
    delay(900);
  }
  tone(PIN_TONE, 880);
  delay(1400);
  noTone(PIN_TONE);  //音止める
  delay(100);
}

// 終了音
void sound_over() {
  playMusic(PIN_TONE, marioOver, 180);
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

void loop() {
  int h, i, j;
  // スタート待機
  unsigned long tm = millis();
  long t = 250;  //周期[ms]
  long p = 100;  //0.5t 時の頂点  x=0.5t時に -x*(x-t) / k = p
  if (score == -1) {
    p = (random(10) + 1) * 10;
  } else {
    p = (score + 1) * 10;
  }
  t = p + 250;
  long k = t * t / 4 / p;
  while (digitalRead(PIN_SW) == HIGH) {
    pixels.clear();
    long x = millis() - tm;
    long y = -x * (x - t) / k;  //2次曲線。放物線
    //long y = p * sin(3.14 * x /t); //sinカーブ
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
    disp_atari(y);
    pixels.show();
  }
  // 開始
  score = 0;
  for (h = 0; h < NUM_ATARI; h++) {
    atari[h] = 0;  //0:未回答,1:OK,2:NG
  }
  pixels.clear();
  disp_atari();
  pixels.show();
  sound_start();
  num_ok_range = NUM_RANGE_DEFAULT;
  for (h = 0; h < NUM_ATARI; h++) {
    set_color();
    pos_ok_begin = 20 + random(8) * 2;
    //帯オン
    pixels.clear();
    disp_atari();
    disp_obi();
    pixels.show();
    //ランダム待機
    delay(random(3000) + 500);
    //落下速度決定
    delay_fall = pow(random(3), 2);
    //流星開始
    int pos_start = NUMPIXELS - 1 - random(8) * 3;
    for (i = pos_start; i >= NUM_ATARI; i--) {
      if (i < pos_start) {
        //ドット消し
        if (i + 1 >= pos_ok_begin && i + 1 < pos_ok_begin + num_ok_range) {
          pixels.setPixelColor(i + 1, color_obi);
        } else if (i + 1 < NUM_ATARI) {
          pixels.setPixelColor(i + 1, color_none);
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
    //帯消し
    if (i > pos_ok_begin && i < pos_ok_begin + num_ok_range) {
      //ドットが帯の上。
      k = i;
    } else if (i <= pos_ok_begin) {
      //ドットが帯より下。
      k = pos_ok_begin - 1;
    }
    for (j = pos_ok_begin + num_ok_range - 1; j > k; j--) {
      pixels.setPixelColor(j, 0);
      pixels.show();
      delay(delay_move);
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
    delay(2000);
  }

  if (score == NUM_ATARI) {
    playMusic(PIN_TONE, marioClear, 150);
  } else {
    playMusic(PIN_TONE, marioOver, 150);
  }
}
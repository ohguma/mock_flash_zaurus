// Arduino用　Music.h
// https://github.com/lalaso2000/Music/

// 曲の最後は必ずNOTE_ENDを指定

#include "music.h"
#include <Arduino.h>

//マリオがゲームオーバーをしたときに鳴る(音)
//ド ソ ミ ラシラソ#ラ#ソ#ソ (スーパーマリオブラザーズ・2)
// https://www.ninsheetmusic.org/download/pdf/1940
// https://www.mymusicsheet.com/ja/javintham/108370
// https://www.youtube.com/watch?v=YQpurENQcno
note marioOver[] = {
  { NOTE_C5, 4 },
  { NOTE_NON, 8 },
  { NOTE_G4, 8 },
  { NOTE_NON, 4 },
  { NOTE_E4, 4 },
  { NOTE_A4, 6 },
  { NOTE_B4, 6 },
  { NOTE_A4, 6 },
  { NOTE_GS4, 6 },
  { NOTE_AS4, 6 },
  { NOTE_GS4, 6 },
  { NOTE_G4, 2 },
  { NOTE_END, 1 }
};


//マリオがステージクリアしたときに鳴る(音)
// https://livedoor.blogimg.jp/masato_guitar/imgs/b/d/bd47aeba.jpg
note marioClear[] = {
  { NOTE_G4, 12 },   //ソ
  { NOTE_C5, 12 },   //ド
  { NOTE_E5, 12 },   //ミ
  { NOTE_G5, 12 },   //ソ
  { NOTE_C6, 12 },   //ド
  { NOTE_E6, 12 },   //ミ
  { NOTE_G6, 4 },    //ソ
  { NOTE_E6, 4 },    //ミ
  { NOTE_GS4, 12 },  //ソ#(ラ♭)
  { NOTE_C5, 12 },   //ド
  { NOTE_DS5, 12 },  //レ#(ミ♭)
  { NOTE_GS5, 12 },  //ソ#(レ♭)
  { NOTE_C6, 12 },   //ド
  { NOTE_DS6, 12 },  //レ#(ミ♭)
  { NOTE_GS6, 4 },   //ソ#(ラ♭)
  { NOTE_E6, 4 },    //ミ
  { NOTE_AS4, 12 },  //ラ#(シ♭)
  { NOTE_D5, 12 },   //レ
  { NOTE_F5, 12 },   //ファ
  { NOTE_AS5, 12 },  //ラ#(シ♭)
  { NOTE_D6, 12 },   //レ
  { NOTE_F6, 12 },   //ファ
  { NOTE_AS6, 4 },   //ラ#(シ♭)
  { NOTE_AS6, 12 },  //ラ#(シ♭)
  { NOTE_AS6, 12 },  //ラ#(シ♭)
  { NOTE_AS6, 12 },  //ラ#(シ♭)
  { NOTE_C6, 1 },    //ド
  { NOTE_END, 1 }
};


// marioStart	100	「スーパーマリオブラザーズ」より、ゲームスタート
note marioStart[] = {
  { NOTE_E4, 16 },
  { NOTE_E4, 16 },
  { NOTE_NON, 16 },
  { NOTE_E4, 16 },
  { NOTE_NON, 16 },
  { NOTE_C4, 16 },
  { NOTE_E4, 16 },
  { NOTE_NON, 16 },
  { NOTE_G4, 16 },
  { NOTE_NON, 8 / 1.5 },
  { NOTE_G3, 16 },
  { NOTE_NON, 8 / 1.5 },
  { NOTE_END, 1 }
};

// marioDead	100	「スーパーマリオブラザーズ」より、ミス
note marioDead[] = {
  { NOTE_C5, 64 },
  { NOTE_CS5, 64 },
  { NOTE_D5, 64 },
  { NOTE_NON, 64 },
  { NOTE_NON, 8 },
  { NOTE_NON, 4 },
  { NOTE_B4, 16 },
  { NOTE_F5, 16 },
  { NOTE_NON, 16 },
  { NOTE_F5, 16 },
  { NOTE_F5, 12 },
  { NOTE_E5, 12 },
  { NOTE_D5, 12 },
  { NOTE_C5, 8 },
  { NOTE_END, 1 }
};


void playMusic(int PIN, note notes[], int bpm){
  int i;
  int beat = 240000 / bpm;
  while(notes[i].pitch != NOTE_END){
    double noteDuration = beat / notes[i].duration / 1.30;
    tone(PIN, notes[i].pitch, noteDuration);
    double pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(PIN);
    i++;
  }
}

#include <FastLED.h>
#include "ble.h"
#include "max3010x.h"

#define NUM_LEDS 1
#define DATA_PIN 21

#define NUM_REACTIONS 10  // 定義10次數據的數量
#define REPORTING_PERIOD_MS 2000

CRGB leds[NUM_LEDS];

const int buttonPin = 2;  // 按鈕引腳
const int ledPin = 13;    // LED燈引腳

bool gameStarted = false;                   // 遊戲是否已經開始
bool timing = false;                        // 是否正在計時
unsigned long startTime;                    // 開始時間
unsigned long reactionTime[NUM_REACTIONS];  // 反應時間陣列
int count = 0;                              // 反應時間計數

uint32_t tsLastReport = 0;

extern BLECharacteristic *pReactionCh;
extern BLECharacteristic *pMax3010xCh;

void setup() {
  Serial.begin(115200);

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed

  Serial.println("[INFO] Initializing BLE work!");
  setupBLE();
  Serial.println("[INFO] BLE work!");

  Serial.println("[INFO] Initializing PulseOximeter.");
  setupMax3010x();
  Serial.println("[INFO] PulseOximeter success.");

  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  Serial.println("Long press the button to start.");
  leds[0] = CRGB::Blue;
  FastLED.show();

  // for (int i = 0; i < 10; i++) {
  //   reactionTime[i] = random(1000, 5000);
  //   pCharacteristic->setValue((uint8_t *)&reactionTime[i], sizeof(reactionTime[i]));
  //   pCharacteristic->notify();
  //   delay(3);
  // }
  // reactionTime[0] = 279;
  // reactionTime[1] = 65535;
  // // spo2*65535 + heartrate
  // reactionTime[2] = 85+90*65536;
}

void loop() {
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    // for (int i = 0; i < NUM_REACTIONS; i++) {
    //   pReactionCh->setValue((uint8_t *)&reactionTime[i], 4);
    //   pReactionCh->notify();
    //   delay(3);
    // }

    int max3010xData[2];
    getMax3010x(max3010xData);
    // Serial.print("Heart rate:");
    // Serial.print(max3010xData[0]);
    // Serial.print("bpm / SpO2:");
    // Serial.print(max3010xData[1]);
    // Serial.println("%");
    // transmit data
    int data = (max3010xData[1] << 16) + max3010xData[0];

    pMax3010xCh->setValue((uint8_t *)&data, 4);
    pMax3010xCh->notify();
    tsLastReport = millis();
  }


  if (!gameStarted && digitalRead(buttonPin) == HIGH) {
    gameStarted = true;
    delay(500);  // 避免按鈕彈跳
    startGame();

    delay(1000);
  }

  if (gameStarted && !timing) {
    timing = true;
    int randomTime = random(1000, 5000);  // 在1秒到5秒之間隨機生成時間
    delay(randomTime);                    // 等待隨機時間後亮起LED
    startTime = millis();

    digitalWrite(ledPin, HIGH);
    leds[0] = CRGB::Green;
    FastLED.show();
  }

  if (timing) {
    unsigned long reaction = millis() - startTime;
  
    if (digitalRead(buttonPin) == HIGH) {
      timing = false;
      reactionTime[count] = reaction; // 記錄反應時間
      pReactionCh->setValue((uint8_t *)&reaction, sizeof(reaction));
      pReactionCh->notify();

      digitalWrite(ledPin, LOW);
      leds[0] = CRGB::Black;
      FastLED.show();
  
      count++;
    } else if (reaction >= 6000) {
      // handle overflow: not press the button
      reaction = 6000;

      timing = false;
      reactionTime[count] = reaction; // 記錄反應時間
      pReactionCh->setValue((uint8_t *)&reaction, sizeof(reaction));
      pReactionCh->notify();

      digitalWrite(ledPin, LOW);
      leds[0] = CRGB::Black;
      FastLED.show();

      count++;
    }

    delay(1000);  // 停止一秒鐘以避免錯誤的按鈕觸發
  }

  if (count == NUM_REACTIONS) {
    endGame();
  }
}

void startGame() {
  Serial.println("Game started! Press the button when the LED lights up.");
  leds[0] = CRGB::Black;
  FastLED.show();
}

void endGame() {
  Serial.println("Game ended! Here are the reaction times:");
  for (int i = 0; i < NUM_REACTIONS; i++) {
    Serial.print("Reaction time ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(reactionTime[i]);
    Serial.println(" ms");

    // pCharacteristic->setValue((uint8_t *)&reactionTime[i], sizeof(reactionTime[i]));
    // pCharacteristic->notify();
    delay(3);
  }

  while (true) {}  // 結束程式
}

// char *concatenateReactionTimes() {
//   char *concatenatedData = (char *)malloc(NUM_REACTIONS * 20);  // 假設每個數字最多佔20個字元，分配足夠大的記憶體空間
//   if (concatenatedData == NULL) {
//     // 記憶體分配失敗
//     return NULL;
//   }

//   strcpy(concatenatedData, "");  // 清空字串

//   for (int i = 0; i < NUM_REACTIONS; ++i) {
//     char tempBuffer[20];                          // 暫存緩衝區，用於將數字轉換為字串
//     sprintf(tempBuffer, "%lu", reactionTime[i]);  // 將反應時間轉換為字串
//     strcat(concatenatedData, tempBuffer);         // 將字串附加到結果字串上
//     if (i != NUM_REACTIONS - 1) {
//       strcat(concatenatedData, ",");  // 在數字之間加上逗號
//     }
//   }

//   return concatenatedData;
// }
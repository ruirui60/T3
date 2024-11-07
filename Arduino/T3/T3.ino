#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>

#define RGB 8
Adafruit_NeoPixel strip(8, RGB, NEO_GRB + NEO_KHZ800);

#define SCREEN_WIDTH 128  // OLED 寬度像素
#define SCREEN_HEIGHT 64  // OLED 高度像素
#define OLED_RESET -1     // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

SoftwareSerial BT(0, 1);  // 藍牙通訊引腳（RX, TX）
String bs;                // 藍牙接收到的訊息

const int CPUledPin = 9;
unsigned long ledMillis = 0;   // LED閃爍時間
const long ledInterval = 500;  // LED閃爍間隔（毫秒）
bool led = false;              // LED狀態
int led2 = 0;

const int buttonUp = A3;     // 向上按鈕引腳
const int buttonDown = A2;   // 向下按鈕引腳
const int buttonEnter = A1;  // 確認按鈕引腳
const int buttonReturn = A0;
unsigned long btMillis = 0;
const long btInterval = 150;

int page = 1;

unsigned long startMillis = 0;
const long introDisplayDuration = 3000;
bool introDisplayDone = false;


const long YRDuration = 1000;  // 黃燈持續時間（2秒）

const long BR = 500;


unsigned long previousMillis = 0;  // 上一次變更時間
int state = 0;



const long interval2 = 100;  // 延遲時間

int position = 0;
bool forward = true;



void setup() {
  Serial.begin(9600);
  BT.begin(9600);
  pinMode(buttonUp, INPUT);     // 設定向上按鈕引腳為輸入
  pinMode(buttonEnter, INPUT);  // 設定確認按鈕引腳為輸入
  pinMode(buttonDown, INPUT);   // 設定向下按鈕引腳為輸入
  pinMode(CPUledPin, OUTPUT);
  strip.begin();
  strip.show();

  // 偵測是否安裝好OLED了
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // 一般1306 OLED的位址都是0x3C
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }
  display.clearDisplay();     // 清除顯示畫面
  display.setTextSize(2);     // 設定文字大小
  display.setTextColor(1);    // 設定文字顏色（1 表示預設顏色）
  display.setCursor(36, 5);   // 設定文字起始座標
  display.print("CYIVS");     // 顯示字串 "NKHS"
  display.setCursor(43, 25);  // 設定文字起始座標
  display.print("C220");      // 顯示字串 "C201"
  display.drawLine(62, 50, 63, 53, 1);
  display.drawLine(60, 53, 65, 53, 1);
  display.drawLine(61, 55, 64, 55, 1);
  display.drawLine(61, 57, 64, 57, 1);
  display.drawRect(60, 59, 6, 3, 1);
  display.drawLine(67, 51, 67, 54, 1);
  display.drawLine(67, 53, 72, 53, 1);
  display.drawLine(67, 56, 72, 56, 1);
  display.drawLine(70, 53, 70, 61, 1);
  display.display();  // 更新顯示內容
}
int Down = false;
bool BlueBreath = false;
bool Rainbow = false;
bool RYGB = false;
bool RYG = false;
bool BLE = false;
bool l1 = false, l2 = false, l3 = false, l4 = false, l5 = false, l6 = false, l7 = false, l8 = false;
String output = "";
void loop() {
  unsigned long starttime = millis();
  unsigned long currentMillis = millis();
  unsigned long interval;
  if (currentMillis - ledMillis >= ledInterval && led2 <= 4) {  // 根據間隔時間來閃爍 LED
    ledMillis = currentMillis;
    led = !led;
    digitalWrite(CPUledPin, led);
    led2++;
  }
  if (led2 > 4)
    digitalWrite(CPUledPin, HIGH);

  if (starttime - startMillis >= introDisplayDuration && !introDisplayDone) {
    int buttonUpState = digitalRead(buttonUp);        // 讀取向上按鈕狀態
    int buttonEnterState = digitalRead(buttonEnter);  // 讀取確認按鈕狀態
    int buttonDownState = digitalRead(buttonDown);    // 讀取向下按鈕狀態
    int buttonReturnState = digitalRead(buttonReturn);

    switch (page) {
      case 1:
        display.clearDisplay();
        display.setTextSize(1);    // 設定文字大小
        display.setTextColor(1);   // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
        display.setCursor(54, 1);  // 設定起始座標
        display.print("menu");     // 要顯示的字串
        display.fillRect(0, 9, 128, 1, 1);
        display.setCursor(1, 12);  // 設定起始座標
        display.print(">");        // 要顯示的字串
        display.setCursor(8, 12);  // 設定起始座標
        display.print("Color");    // 要顯示的字串
        display.setCursor(8, 25);  // 設定起始座標
        display.print("BLE");      // 要顯示的字串
        display.setCursor(8, 38);  // 設定起始座標
        display.print("EEPROM");   // 要顯示的字串
        display.setCursor(8, 51);  // 設定起始座標
        display.print("Light");    // 要顯示的字串
        display.display();         // 要有這行才會把文字顯示出來
        if (currentMillis - btMillis >= btInterval) {
          if (buttonDownState == LOW) {
            btMillis = currentMillis;
            page = 2;
          }
          if (buttonEnterState == LOW) {
            btMillis = currentMillis;
            display.clearDisplay();
            page = 10;
          }
        }
        break;
      case 2:
        display.clearDisplay();
        display.setTextSize(1);    // 設定文字大小
        display.setTextColor(1);   // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
        display.setCursor(54, 1);  // 設定起始座標
        display.print("menu");     // 要顯示的字串
        display.fillRect(0, 9, 128, 1, 1);
        display.setCursor(1, 25);  // 設定起始座標
        display.print(">");        // 要顯示的字串
        display.setCursor(8, 12);  // 設定起始座標
        display.print("Color");    // 要顯示的字串
        display.setCursor(8, 25);  // 設定起始座標
        display.print("BLE");      // 要顯示的字串
        display.setCursor(8, 38);  // 設定起始座標
        display.print("EEPROM");   // 要顯示的字串
        display.setCursor(8, 51);  // 設定起始座標
        display.print("Light");    // 要顯示的字串
        display.display();         // 要有這行才會把文字顯示出來
        if (currentMillis - btMillis >= btInterval) {
          if (buttonUpState == LOW) {
            btMillis = currentMillis;
            page = 1;
          }
          if (buttonDownState == LOW) {
            btMillis = currentMillis;
            page = 3;
          }
          if (buttonEnterState == LOW) {
            btMillis = currentMillis;
            display.clearDisplay();
            page = 20;
          }
        }
        break;
      case 3:
        display.clearDisplay();
        display.setTextSize(1);    // 設定文字大小
        display.setTextColor(1);   // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
        display.setCursor(54, 1);  // 設定起始座標
        display.print("menu");     // 要顯示的字串
        display.fillRect(0, 9, 128, 1, 1);
        display.setCursor(1, 38);  // 設定起始座標
        display.print(">");        // 要顯示的字串
        display.setCursor(8, 12);  // 設定起始座標
        display.print("Color");    // 要顯示的字串
        display.setCursor(8, 25);  // 設定起始座標
        display.print("BLE");      // 要顯示的字串
        display.setCursor(8, 38);  // 設定起始座標
        display.print("EEPROM");   // 要顯示的字串
        display.setCursor(8, 51);  // 設定起始座標
        display.print("Light");    // 要顯示的字串
        display.display();         // 要有這行才會把文字顯示出來
        if (currentMillis - btMillis >= btInterval) {
          if (buttonUpState == LOW) {
            btMillis = currentMillis;
            page = 2;
          }
          if (buttonDownState == LOW) {
            btMillis = currentMillis;
            page = 4;
          }
          if (buttonEnterState == LOW) {
            btMillis = currentMillis;
            page = 30;
          }
        }
        break;
      case 4:
        display.clearDisplay();
        display.setTextSize(1);    // 設定文字大小
        display.setTextColor(1);   // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
        display.setCursor(54, 1);  // 設定起始座標
        display.print("menu");     // 要顯示的字串
        display.fillRect(0, 9, 128, 1, 1);
        display.setCursor(1, 51);  // 設定起始座標
        display.print(">");        // 要顯示的字串
        display.setCursor(8, 12);  // 設定起始座標
        display.print("Color");    // 要顯示的字串
        display.setCursor(8, 25);  // 設定起始座標
        display.print("BLE");      // 要顯示的字串
        display.setCursor(8, 38);  // 設定起始座標
        display.print("EEPROM");   // 要顯示的字串
        display.setCursor(8, 51);  // 設定起始座標
        display.print("Light");    // 要顯示的字串
        display.display();         // 要有這行才會把文字顯示出來
        if (currentMillis - btMillis >= btInterval) {
          if (buttonUpState == LOW) {
            btMillis = currentMillis;
            page = 3;
          }
          if (buttonEnterState == LOW) {
            btMillis = currentMillis;
            display.clearDisplay();
            Lightpage();
            display.display();
            page = 40;
          }
        }
        break;


      case 10:
        display.setTextSize(1);    // 設定文字大小
        display.setTextColor(1);   // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
        display.setCursor(50, 1);  // 設定起始座標
        display.print("Color");    // 要顯示的字串
        display.setTextSize(1);    // 設定文字小大
        for (int i = 1, x = 5; x <= 124; x += 15) {
          display.setCursor(x, 9);  // 設定起始座標
          display.print("L");       // 要顯示的字串
          display.print(i);
          i++;
        }
        display.setTextSize(2);
        display.setCursor(1, 44);   // 設定起始座標
        display.print("2");         // 要顯示的字串
        display.setCursor(66, 26);  // 設定起始座標
        display.print("3");         // 要顯示的字串
        display.setCursor(66, 44);  // 設定起始座標
        display.print("4");         // 要顯示的字串
        display.fillRect(0, 25, 64, 18, 1);
        display.setTextColor(0);
        display.setCursor(1, 26);  // 設定起始座標
        display.print("1");        // 要顯示的字串
        display.display();         // 要有這行才會把文字顯示出來
        if (currentMillis - btMillis >= btInterval) {
          if (buttonDownState == LOW) {
            btMillis = currentMillis;
            display.clearDisplay();
            page = 11;
          }

          if (buttonEnterState == LOW) {
            btMillis = currentMillis;
            display.fillRect(0, 17, 128, 7, 0);
            display.setTextSize(1);
            display.setTextColor(1);
            display.setTextSize(1);
            display.setCursor(5, 17);
            display.print("Y");
            display.setCursor(20, 17);
            display.print("Y");
            display.setCursor(35, 17);
            display.print("Y");
            display.setCursor(80, 17);
            display.print("R");
            display.setCursor(95, 17);
            display.print("R");
            display.setCursor(110, 17);
            display.print("R");
            display.display();
            Rainbow = false;
            RYGB = false;
            RYG = true;
          }
          if (buttonReturnState == LOW) {
            btMillis = currentMillis;
            page = 1;
          }
        }
        break;
      case 11:
        display.setTextSize(1);    // 設定文字大小
        display.setTextColor(1);   // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
        display.setCursor(50, 1);  // 設定起始座標
        display.print("Color");    // 要顯示的字串
        display.setTextSize(1);    // 設定文字小大
        for (int i = 1, x = 5; x <= 124; x += 15) {
          display.setCursor(x, 9);  // 設定起始座標
          display.print("L");       // 要顯示的字串
          display.print(i);
          i++;
        }
        display.setTextSize(2);
        display.setCursor(1, 26);   // 設定起始座標
        display.print("1");         // 要顯示的字串
        display.setCursor(66, 26);  // 設定起始座標
        display.print("3");         // 要顯示的字串
        display.setCursor(66, 44);  // 設定起始座標
        display.print("4");         // 要顯示的字串
        display.fillRect(0, 43, 64, 18, 1);
        display.setTextColor(0);
        display.setCursor(1, 44);  // 設定起始座標
        display.print("2");        // 要顯示的字串
        display.display();         // 要有這行才會把文字顯示出來
        if (currentMillis - btMillis >= btInterval) {
          if (buttonUpState == LOW) {
            btMillis = currentMillis;
            display.clearDisplay();
            page = 10;
          }
          if (buttonDownState == LOW) {
            btMillis = currentMillis;
            display.clearDisplay();
            page = 12;
          }
          if (buttonEnterState == LOW) {
            btMillis = currentMillis;
            display.fillRect(0, 17, 128, 7, 0);
            display.setTextSize(1);
            display.setTextColor(1);
            display.setTextSize(1);
            for (int x = 5; x <= 124; x += 15) {
              display.setCursor(x, 17);  // 設定起始座標
              display.print("R");        // 要顯示的字串
            }
            for (int i = 0; i <= 7; i++)
              strip.setPixelColor(i, strip.Color(0, 0, 0));
            display.display();
            strip.show();
            Rainbow = true;
            RYGB = false;
            RYG = false;
          }
          if (buttonReturnState == LOW) {
            btMillis = currentMillis;
            page = 1;
          }
        }
        break;
      case 12:
        display.setTextSize(1);    // 設定文字大小
        display.setTextColor(1);   // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
        display.setCursor(50, 1);  // 設定起始座標
        display.print("Color");    // 要顯示的字串
        display.setTextSize(1);    // 設定文字小大
        for (int i = 1, x = 5; x <= 124; x += 15) {
          display.setCursor(x, 9);  // 設定起始座標
          display.print("L");       // 要顯示的字串
          display.print(i);
          i++;
        }
        display.setTextSize(2);
        display.setCursor(1, 26);   // 設定起始座標
        display.print("1");         // 要顯示的字串
        display.setCursor(1, 44);   // 設定起始座標
        display.print("2");         // 要顯示的字串
        display.setCursor(66, 44);  // 設定起始座標
        display.print("4");         // 要顯示的字串
        display.fillRect(65, 25, 128, 18, 1);
        display.setTextColor(0);
        display.setCursor(66, 26);  // 設定起始座標
        display.print("3");         // 要顯示的字串
        display.display();          // 要有這行才會把文字顯示出來
        if (currentMillis - btMillis >= btInterval) {
          if (buttonUpState == LOW) {
            btMillis = currentMillis;
            display.clearDisplay();
            page = 11;
          }
          if (buttonDownState == LOW) {
            btMillis = currentMillis;
            display.clearDisplay();
            page = 13;
          }
          if (buttonEnterState == LOW) {
            btMillis = currentMillis;
            display.fillRect(0, 17, 128, 7, 0);
            display.setTextColor(1);
            display.setTextSize(1);
            for (int x = 5; x <= 124; x += 15) {
              display.setCursor(x, 17);  // 設定起始座標
              display.print("B");        // 要顯示的字串
            }
            display.display();
            for (int i = 0; i <= 7; i++) {
              strip.setPixelColor(i, strip.Color(0, 0, 0));
            }
            strip.show();
            Rainbow = false;
            RYG = false;
            RYGB = true;
            position = 0;
          }
          if (buttonReturnState == LOW) {
            btMillis = currentMillis;
            page = 1;
          }
        }
        break;
      case 13:
        display.setTextSize(1);    // 設定文字大小
        display.setTextColor(1);   // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
        display.setCursor(50, 1);  // 設定起始座標
        display.print("Color");    // 要顯示的字串
        display.setTextSize(1);    // 設定文字小大
        for (int i = 1, x = 5; x <= 124; x += 15) {
          display.setCursor(x, 9);  // 設定起始座標
          display.print("L");       // 要顯示的字串
          display.print(i);
          i++;
        }
        display.setTextSize(2);
        display.setCursor(1, 26);   // 設定起始座標
        display.print("1");         // 要顯示的字串
        display.setCursor(1, 44);   // 設定起始座標
        display.print("2");         // 要顯示的字串
        display.setCursor(66, 26);  // 設定起始座標
        display.print("3");         // 要顯示的字串
        display.fillRect(65, 43, 128, 18, 1);
        display.setTextColor(0);
        display.setCursor(66, 44);  // 設定起始座標
        display.print("4");         // 要顯示的字串
        display.display();          // 要有這行才會把文字顯示出來
        if (currentMillis - btMillis >= btInterval) {
          if (buttonUpState == LOW) {
            btMillis = currentMillis;
            display.clearDisplay();
            page = 12;
          }
          if (buttonEnterState == LOW) {
            btMillis = currentMillis;
            display.fillRect(0, 17, 128, 7, 0);
            display.display();
            Rainbow = false;
            RYG = false;
            RYGB = false;
            BlueBreath = false;
            for (int i = 0; i <= 7; i++)
              strip.setPixelColor(i, strip.Color(0, 0, 0));
            strip.show();
          }
          if (buttonReturnState == LOW) {
            btMillis = currentMillis;
            page = 1;
          }
        }
        break;
      case 20:
        display.setTextSize(2);    // 設定文字大小
        display.setTextColor(1);   // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
        display.setCursor(47, 1);  // 設定起始座標
        display.print("BLE");      // 要顯示的字串
        display.setCursor(1, 17);  // 設定起始座標
        display.print("Connect");  // 要顯示的字串
        if (BLE == true) {
          display.setCursor(1, 34);        // 設定起始座標
          display.print("Change EEPROM");  // 要顯示的字串
          display.display();               // 要有這行才會把文字顯示出來
          if (currentMillis - btMillis >= btInterval) {
            if (buttonReturnState == LOW) {
              btMillis = currentMillis;
              page = 2;
            }
          }
        } else {
          display.display();  // 要有這行才會把文字顯示出來
          if (currentMillis - btMillis >= btInterval) {
            if (buttonReturnState == LOW) {
              btMillis = currentMillis;
              page = 2;
            }
          }
        }
        break;
      case 30:
        display.clearDisplay();
        display.setTextSize(2);    // 設定文字大小
        display.setTextColor(1);   // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
        display.setCursor(29, 1);  // 設定起始座標
        display.print("EEPROM");   // 要顯示的字串
        display.setCursor(42, 34);
        for (int i = 1; i <= 4; i++) {
          char value = EEPROM.read(i);  // 讀取 EEPROM 中的字符
          display.print(value);         // 顯示字符
        }
        display.display();  // 要有這行才會把文字顯示出來
        if (currentMillis - btMillis >= btInterval) {
          if (buttonReturnState == LOW) {
            btMillis = currentMillis;
            page = 3;
          }
        }
        break;
      case 40:
        Rainbow = false;
        RYGB = false;
        RYG = false;
        display.setTextSize(1);   // 設定文字大小
        display.setTextColor(1);  // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
        display.setCursor(8, 20);
        display.print(">");
        Light();
        if (l1 == true)
          strip.setPixelColor(0, strip.Color(255, 0, 0));
        else
          strip.setPixelColor(0, strip.Color(0, 0, 0));
        strip.show();
        display.display();
        if (currentMillis - btMillis >= btInterval) {
          if (buttonDownState == LOW) {
            btMillis = currentMillis;
            display.fillRect(8, 20, 5, 7, 0);
            page = 41;
          }
          if (buttonEnterState == LOW) {
            btMillis = currentMillis;
            l1 = !l1;
          }
          if (buttonReturnState == LOW) {
            btMillis = currentMillis;
            page = 4;
          }
        }
        break;
      case 41:
        display.setTextSize(1);
        display.setTextColor(1);
        display.setCursor(37, 20);
        display.print(">");
        Light();
        if (l2 == true)
          strip.setPixelColor(1, strip.Color(255, 100, 0));
        else
          strip.setPixelColor(1, strip.Color(0, 0, 0));
        strip.show();
        display.display();
        if (currentMillis - btMillis >= btInterval) {
          if (buttonUpState == LOW) {
            btMillis = currentMillis;
            display.fillRect(37, 20, 5, 7, 0);
            page = 40;
          }
          if (buttonDownState == LOW) {
            btMillis = currentMillis;
            display.fillRect(37, 20, 5, 7, 0);
            page = 42;
          }
          if (buttonEnterState == LOW) {
            btMillis = currentMillis;
            l2 = !l2;
          }
          if (buttonReturnState == LOW) {
            btMillis = currentMillis;
            page = 4;
          }
        }
        break;
      case 42:
        display.setTextSize(1);   // 設定文字大小
        display.setTextColor(1);  // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
        display.setCursor(66, 20);
        display.print(">");
        Light();
        if (l3 == true)
          strip.setPixelColor(2, strip.Color(255, 255, 0));
        else
          strip.setPixelColor(2, strip.Color(0, 0, 0));
        strip.show();
        display.display();  // 要有這行才會把文字顯示出來
        if (currentMillis - btMillis >= btInterval) {
          if (buttonUpState == LOW) {
            btMillis = currentMillis;
            display.fillRect(66, 20, 5, 7, 0);
            page = 41;
          }
          if (buttonDownState == LOW) {
            btMillis = currentMillis;
            display.fillRect(66, 20, 5, 7, 0);
            page = 43;
          }
          if (buttonEnterState == LOW) {
            btMillis = currentMillis;
            l3 = !l3;
          }
          if (buttonReturnState == LOW) {
            btMillis = currentMillis;
            page = 4;
          }
        }
        break;
      case 43:
        display.setTextSize(1);   // 設定文字大小
        display.setTextColor(1);  // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
        display.setCursor(95, 20);
        display.print(">");
        Light();
        if (l4 == true)
          strip.setPixelColor(3, strip.Color(0, 255, 0));
        else
          strip.setPixelColor(3, strip.Color(0, 0, 0));
        strip.show();
        display.display();  // 要有這行才會把文字顯示出來
        if (currentMillis - btMillis >= btInterval) {
          if (buttonUpState == LOW) {
            btMillis = currentMillis;
            display.fillRect(95, 20, 5, 7, 0);
            page = 42;
          }
          if (buttonDownState == LOW) {
            btMillis = currentMillis;
            display.fillRect(95, 20, 5, 7, 0);
            page = 44;
          }
          if (buttonEnterState == LOW) {
            btMillis = currentMillis;
            l4 = !l4;
          }
          if (buttonReturnState == LOW) {
            btMillis = currentMillis;
            page = 4;
          }
        }
        break;
      case 44:
        display.setTextSize(1);   // 設定文字大小
        display.setTextColor(1);  // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
        display.setCursor(8, 40);
        display.print(">");
        Light();
        if (l5 == true)
          strip.setPixelColor(4, strip.Color(0, 0, 255));
        else
          strip.setPixelColor(4, strip.Color(0, 0, 0));
        strip.show();
        display.display();  // 要有這行才會把文字顯示出來
        if (currentMillis - btMillis >= btInterval) {
          if (buttonUpState == LOW) {
            btMillis = currentMillis;
            display.fillRect(8, 40, 5, 7, 0);
            page = 43;
          }
          if (buttonDownState == LOW) {
            btMillis = currentMillis;
            display.fillRect(8, 40, 5, 7, 0);
            page = 45;
          }
          if (buttonEnterState == LOW) {
            btMillis = currentMillis;
            l5 = !l5;
          }
          if (buttonReturnState == LOW) {
            btMillis = currentMillis;
            page = 4;
          }
        }
        break;
      case 45:
        display.setTextSize(1);   // 設定文字大小
        display.setTextColor(1);  // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
        display.setCursor(37, 40);
        display.print(">");
        Light();
        if (l6 == true)
          strip.setPixelColor(5, strip.Color(255, 0, 255));
        else
          strip.setPixelColor(5, strip.Color(0, 0, 0));
        strip.show();
        display.display();  // 要有這行才會把文字顯示出來
        if (currentMillis - btMillis >= btInterval) {
          if (buttonUpState == LOW) {
            btMillis = currentMillis;
            display.fillRect(37, 40, 5, 7, 0);
            page = 44;
          }
          if (buttonDownState == LOW) {
            btMillis = currentMillis;
            display.fillRect(37, 40, 5, 7, 0);
            page = 46;
          }
          if (buttonEnterState == LOW) {
            btMillis = currentMillis;
            l6 = !l6;
          }
          if (buttonReturnState == LOW) {
            btMillis = currentMillis;
            page = 4;
          }
        }
        break;
      case 46:
        display.setTextSize(1);   // 設定文字大小
        display.setTextColor(1);  // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
        display.setCursor(66, 40);
        display.print(">");
        Light();
        if (l7 == true) {
          strip.setPixelColor(6, strip.Color(10, 10, 10));
        } else
          strip.setPixelColor(6, strip.Color(0, 0, 0));
        strip.show();
        display.display();
        if (currentMillis - btMillis >= btInterval) {
          if (buttonUpState == LOW) {
            btMillis = currentMillis;
            display.fillRect(66, 40, 5, 7, 0);
            page = 45;
          }
          if (buttonDownState == LOW) {
            btMillis = currentMillis;
            display.fillRect(66, 40, 5, 7, 0);
            page = 47;
          }
          if (buttonEnterState == LOW) {
            btMillis = currentMillis;
            l7 = !l7;
          }
          if (buttonReturnState == LOW) {
            btMillis = currentMillis;
            page = 4;
          }
        }
        break;
      case 47:
        display.setTextSize(1);   // 設定文字大小
        display.setTextColor(1);  // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
        display.setCursor(95, 40);
        display.print(">");
        Light();
        if (l8 == true)
          strip.setPixelColor(7, strip.Color(255, 255, 255));
        else
          strip.setPixelColor(7, strip.Color(0, 0, 0));
        strip.show();
        display.display();  // 要有這行才會把文字顯示出來
        if (currentMillis - btMillis >= btInterval) {
          if (buttonUpState == LOW) {
            btMillis = currentMillis;
            display.fillRect(95, 40, 5, 7, 0);
            page = 46;
          }
          if (buttonEnterState == LOW) {
            btMillis = currentMillis;
            l8 = !l8;
          }
          if (buttonReturnState == LOW) {
            btMillis = currentMillis;
            page = 4;
          }
        }
        break;
    }
  }

  if (RYGB == true) {
    if (currentMillis - previousMillis >= BR) {
      previousMillis = currentMillis;
      // 設定顏色
      if (RYGB) {
        if (position < 8)
          for (int i = 0; i <= 7; i++) {
            if (i == position) strip.setPixelColor(i, strip.Color(0, 0, 255));
            strip.show();
          }
        if (position == 8)
          for (int i = 0; i <= 7; i++) {
            strip.setPixelColor(i, strip.Color(0, 0, 0));
          }
        if (position == 9)
          for (int i = 0; i <= 7; i++) {
            strip.setPixelColor(i, strip.Color(0, 0, 255));
          }
        if (position == 10)
          for (int i = 0; i <= 7; i++) {
            strip.setPixelColor(i, strip.Color(0, 0, 0));
          }
        if (position == 11)
          for (int i = 0; i <= 7; i++) {
            strip.setPixelColor(i, strip.Color(0, 0, 255));
          }
        if (position > 11)
          for (int i = 0; i <= 7; i++) {
            if (i == position - 12) strip.setPixelColor(i, strip.Color(0, 0, 0));
            strip.show();
          }
        strip.show();
        position++;
        if (position > 19) {
          RYGB == false;
        }
      }
    }
  }



  if (RYG == true) {
    switch (state) {
      case 0:  // 南北向綠燈
        interval = YRDuration;
        break;
      case 1:  // 南北向黃燈
        interval = YRDuration;
        break;
    }

    // 根據時間來變更燈的狀態
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      state = (state + 1) % 2;  // 共有6種狀態
      switch (state) {
        case 0:
          strip.clear();
          for (int i = 0; i <= 2; i++)
            strip.setPixelColor(i, strip.Color(255, 255, 0));
          strip.show();
          break;
        case 1:
          strip.clear();
          for (int i = 5; i <= 7; i++)
            strip.setPixelColor(i, strip.Color(255, 0, 0));
          strip.show();
          break;
      }
    }
  }


  if (Rainbow == true) {
    if (currentMillis - previousMillis >= interval2) {
      previousMillis = currentMillis;


      // 設定顏色
      if (Rainbow) {
        for (int i = 0; i <= 7; i++) {
          if (i == position) strip.setPixelColor(i, strip.Color(255, 0, 0));
          strip.show();
        }
        for (int i = 0; i <= 7; i++) {
          if (i == position - 8) strip.setPixelColor(i, strip.Color(255, 125, 0));
          strip.show();
        }
        for (int i = 0; i <= 7; i++) {
          if (i == position - 16) strip.setPixelColor(i, strip.Color(255, 255, 0));
          strip.show();
        }
        for (int i = 0; i <= 7; i++) {
          if (i == position - 24) strip.setPixelColor(i, strip.Color(0, 255, 0));
          strip.show();
        }
        for (int i = 0; i <= 7; i++) {
          if (i == position - 32) strip.setPixelColor(i, strip.Color(0, 0, 255));
          strip.show();
        }
        for (int i = 0; i <= 7; i++) {
          if (i == position - 40) strip.setPixelColor(i, strip.Color(0, 255, 255));
          strip.show();
        }
        for (int i = 0; i <= 7; i++) {
          if (i == position - 48) strip.setPixelColor(i, strip.Color(255, 0, 255));
          strip.show();
        }
        for (int i = 0; i <= 7; i++) {
          if (i == position - 56) strip.setPixelColor(i, strip.Color(0, 0, 0));
          strip.show();
        }

        // 更新顯示
        strip.show();

        // 更新位置

        position++;
        if (position > 64) {
          position = 0;
        }
      }
    }
  }

  if (BT.available() > 0) {                    // 如果有藍牙資料可讀
    String bs = BT.readString();               // 讀取藍牙資料
    Serial.println(bs);                        // 輸出藍牙資料
    if (bs[0] == 'E') {                        // 如果資料以 'E' 開頭，啟動交通燈
      for (int i = 1; i < bs.length(); i++) {  // 從第1個字元開始儲存
        // 確保不會超過 EEPROM 容量
        if (i < EEPROM.length()) {
          EEPROM.write(i, bs[i]);  // 寫入字元
        }
      }
    }
  }
}

void setAllPixels(uint32_t color) {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
  }
}

void Lightpage() {
  display.setTextSize(2);
  display.setTextColor(1);
  display.setCursor(29, 1);
  display.print("Light");
  for (int i = 0; i <= 7; i++)
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  display.setTextSize(1);
  display.setCursor(19, 20);
  display.print("L1");
  display.setCursor(48, 20);
  display.print("L2");
  display.setCursor(77, 20);
  display.print("L3");
  display.setCursor(106, 20);
  display.print("L4");
  display.setCursor(19, 40);
  display.print("L5");
  display.setCursor(48, 40);
  display.print("L6");
  display.setCursor(77, 40);
  display.print("L7");
  display.setCursor(106, 40);
  display.print("L8");
}
void Light() {
  display.setTextColor(1);
  display.setTextSize(1);
  if (l1 == true)
    strip.setPixelColor(0, strip.Color(255, 0, 0));
  display.fillRect(15, 22, 2, 2, l1);
  if (l2 == true)
    strip.setPixelColor(1, strip.Color(255, 100, 0));
  display.fillRect(44, 22, 2, 2, l2);
  if (l3 == true)
    strip.setPixelColor(2, strip.Color(255, 255, 0));
  display.fillRect(73, 22, 2, 2, l3);
  if (l4 == true)
    strip.setPixelColor(3, strip.Color(0, 255, 0));
  display.fillRect(102, 22, 2, 2, l4);
  if (l5 == true)
    strip.setPixelColor(4, strip.Color(0, 0, 255));
  display.fillRect(15, 42, 2, 2, l5);
  if (l6 == true)
    strip.setPixelColor(5, strip.Color(255, 0, 255));
  display.fillRect(44, 42, 2, 2, l6);
  if (l7 == true)
    strip.setPixelColor(6, strip.Color(10, 10, 10));
  display.fillRect(73, 42, 2, 2, l7);
  if (l8 == true)
    strip.setPixelColor(7, strip.Color(255, 255, 255));
  display.fillRect(102, 42, 2, 2, l8);
}
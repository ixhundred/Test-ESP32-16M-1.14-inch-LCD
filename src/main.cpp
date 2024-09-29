#include <Arduino.h>
#include <EEPROM.h>
#include <SPI.h>
#include <esp_task_wdt.h>

#define MOVE_PERIOD 750
#define MOVE2_PERIOD 250
#define TEST_SPEEDX  4
#define TEST_SPEEDY  4
#define RX2_PIN 27 
#define TX2_PIN 26
#define T2_SPACE 11

#include <LiquidCrystal_I2C.h>
#define I2C_SDA 21
#define I2C_SCL 22
LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

#include "ixhundred_Move.h"
ixhundred_Move titles[13];
ixhundred_Move titles2[18];
uint32_t tmove = 0;
uint32_t tmove2 = 0;
uint8_t mark[4][20];
int speed1 = MOVE_PERIOD;
int speed2 = MOVE2_PERIOD;

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#define TFT_CS         5
#define TFT_RST        23
#define TFT_DC         16
//Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
#define TFT_MOSI 19  // Data out
#define TFT_SCLK 18  // Clock out
#define TFT_BL   4
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

#define BT_L  0
#define BT_R  35

float p = 3.1415926;

#define WDT_TIMEOUT WDTO_8S
TaskHandle_t Task1;
void Task1code( void * pvParameters );

void testlines(uint16_t color);
void testdrawtext(char *text, uint16_t color);
void testfastlines(uint16_t color1, uint16_t color2);
void tftPrintTest();

void setup() {
  Serial.begin(115200);
  //Serial1.begin(115200,SERIAL_8N1,RX2_PIN, TX2_PIN);
  EEPROM.begin(4096);
  randomSeed(analogRead(A0));
  pinMode(TFT_BL,OUTPUT);
  pinMode(BT_L,INPUT_PULLUP);
  pinMode(BT_R,INPUT_PULLUP);
  digitalWrite(TFT_BL,1);

	lcd.init(I2C_SDA, I2C_SCL); // initialize the lcd to use user defined I2C pins
	lcd.backlight();
	lcd.setCursor(3,0);
	lcd.print("Hello, world!");
	lcd.setCursor(2,1);
	lcd.print("V.I. Integration");
	lcd.setCursor(0,2);
	lcd.print("900 !! ixhundred");
	lcd.setCursor(0,3);
	lcd.print("#Ready!");

  Serial.println(F("#Initialized Screen..."));
  tft.init(135, 240);           // Init ST7789 240x135, 1.14 inch
  uint16_t time = millis();
  tft.fillScreen(ST77XX_BLACK);
  time = millis() - time;
  Serial.printf("Frame time = %d ms\r\n",time);
  tft.setRotation(2);

  // line draw test
  //testlines(ST77XX_YELLOW);
  //delay(500);

  // optimized lines
  //testfastlines(ST77XX_RED, ST77XX_BLUE);
  //delay(500);

  // optimized lines
  //tftPrintTest();

  tft.setTextSize(2);
  testdrawtext((char *)"\n\nixhundred",ST77XX_BLUE);
  delay(1000);
  tft.fillScreen(ST77XX_BLACK);

  titles[0].init('V',3,1);
  titles[1].init('A',4,1);
  titles[2].init('N',5,1);
  titles[3].init('L',6,1);
  titles[4].init('O',7,1);
  titles[5].init('P',8,1);
  titles[6].init(' ',9,1);
  titles[7].init('I',10,1);
  titles[8].init('N',11,1);
  titles[9].init('C',12,1);
  titles[10].init('H',13,1);
  titles[11].init('A',14,1);
  titles[12].init('M',15,1);

  xTaskCreatePinnedToCore(
                      Task1code,   // Task function. 
                      "TFTTask",     // name of task. 
                      10000,       // Stack size of task 
                      NULL,        // parameter of the task 
                      1,           // priority of the task 
                      &Task1,      // Task handle to keep track of created task 
                      0);          // pin task to core 0
  tmove = millis();  
  esp_task_wdt_init(WDTO_8S,true);
  esp_task_wdt_add(Task1);  
  Serial.println("#Ready");
}

void loop() {
  esp_task_wdt_reset();
  if(digitalRead(BT_L)==LOW) {
    delay(20);
    if(digitalRead(BT_L)==LOW) {
      tft.invertDisplay(false);
      Serial.println("#Left pressed");
      while(digitalRead(BT_L)==LOW);
      Serial.println("#Left released");
    }
  }
  if(digitalRead(BT_R)==LOW) {
    delay(20);
    if(digitalRead(BT_R)==LOW) {
      tft.invertDisplay(true);
      Serial.println("#Right pressed");
      while(digitalRead(BT_R)==LOW);
      Serial.println("#Right released");
    }
  }

  if ((uint32_t)(millis()-tmove) >= speed1) {
    tmove = millis();
    //Serial1.println("#tick");
    uint8_t fin = 1;
    for(int i = 0; i < sizeof titles/sizeof titles[0]; ++i) {
      if (!titles[i].move_finished()) {
        fin = 0;
        break;
      }
    }
    if (fin == 1) {
      //finished reset
      Serial.println("#titles move finished");
      speed1 = random(25,101)*10;
      Serial.printf("#Next Speed 1 = %d\r\n",speed1);
      lcd.clear();
      for(int i = 0; i < sizeof titles/sizeof titles[0]; ++i) {
        titles[i].show(lcd);
        if(titles[i]._cy+1 < titles[i]._maxy) {
          lcd.setCursor(titles[i]._cx,titles[i]._cy+1);
          lcd.print('=');
        }
      }
      
      delay(3000);
      lcd.clear();
      for(int i = 0; i < 4; ++i)
        for(int j = 0; j < 20; ++j)
          mark[i][j] = 0;
      for(int i = 0; i < sizeof titles/sizeof titles[0]; ++i) {
        int to = millis();
        do {
          titles[i].randompos();
        } while(mark[(int)titles[i]._cy][(int)titles[i]._cx] == 1 && millis()-to <= 1000);
        mark[(int)titles[i]._cy][(int)titles[i]._cx] = 1;
        titles[i].show(lcd);
        Serial.printf("#titles random = %c\r\n",titles[i]._char);
        delay(speed1);
      }
      delay(3000);
    }
    else {
      //try to move next;
      Serial.println("#titles move next");
      //lcd.clear();
      for(int i = 0; i < sizeof titles/sizeof titles[0]; ++i) {
        titles[i].move_next(lcd, titles);
      }
      //for(int j = 0; j < sizeof titles/sizeof titles[0]; ++j) {
      //  titles[j].show(lcd);
      //}
    }

  }
 
}

void Task1code( void * pvParameters ){
  //Serial.print("Task1 (TFT) running on core ");
  //Serial.println(xPortGetCoreID());
  titles2[0].init('i',T2_SPACE,64,tft.width(),tft.height(),TEST_SPEEDX,TEST_SPEEDY);
  titles2[1].init('x',T2_SPACE*2,64,tft.width(),tft.height(),TEST_SPEEDX,TEST_SPEEDY);
  titles2[2].init('h',T2_SPACE*3,64,tft.width(),tft.height(),TEST_SPEEDX,TEST_SPEEDY);
  titles2[3].init('u',T2_SPACE*4,64,tft.width(),tft.height(),TEST_SPEEDX,TEST_SPEEDY);
  titles2[4].init('n',T2_SPACE*5,64,tft.width(),tft.height(),TEST_SPEEDX,TEST_SPEEDY);
  titles2[5].init('d',T2_SPACE*6,64,tft.width(),tft.height(),TEST_SPEEDX,TEST_SPEEDY);
  titles2[6].init('r',T2_SPACE*7,64,tft.width(),tft.height(),TEST_SPEEDX,TEST_SPEEDY);
  titles2[7].init('e',T2_SPACE*8,64,tft.width(),tft.height(),TEST_SPEEDX,TEST_SPEEDY);
  titles2[8].init('d',T2_SPACE*9,64,tft.width(),tft.height(),TEST_SPEEDX,TEST_SPEEDY);
  titles2[9].init('p',T2_SPACE,100,tft.width(),tft.height(),TEST_SPEEDX,TEST_SPEEDY);
  titles2[10].init('i',T2_SPACE*2,96,tft.width(),tft.height(),TEST_SPEEDX,TEST_SPEEDY);
  titles2[11].init('n',T2_SPACE*3,100,tft.width(),tft.height(),TEST_SPEEDX,TEST_SPEEDY);
  titles2[12].init('k',T2_SPACE*4,96,tft.width(),tft.height(),TEST_SPEEDX,TEST_SPEEDY);
  titles2[13].init('j',T2_SPACE*5,100,tft.width(),tft.height(),TEST_SPEEDX,TEST_SPEEDY);
  titles2[14].init('a',T2_SPACE*6,96,tft.width(),tft.height(),TEST_SPEEDX,TEST_SPEEDY);
  titles2[15].init('4',T2_SPACE*7,100,tft.width(),tft.height(),TEST_SPEEDX,TEST_SPEEDY);
  titles2[16].init('9',T2_SPACE*8,96,tft.width(),tft.height(),TEST_SPEEDX,TEST_SPEEDY);
  titles2[17].init('8',T2_SPACE*9,100,tft.width(),tft.height(),TEST_SPEEDX,TEST_SPEEDY);

  tmove2 = millis();

  for(;;) {
    esp_task_wdt_reset();
    if ((uint32_t)(millis()-tmove2) >= speed2) {
      tmove2 = millis();
      uint8_t fin2 = 1;
      for(int i = 0; i < sizeof titles2/sizeof titles2[0]; ++i) {
        if (!titles2[i].move_finished()) {
          fin2 = 0;
          break;
        }
      }
      if (fin2 == 1) {
        //finished reset
        Serial.println("#titles2 move finished");
        speed2 = random(5,50)*10;
        Serial.printf("#Next Speed 2 = %d\r\n",speed2);
        //tft.fillScreen(ST77XX_BLACK);
        for(int i = 0; i < sizeof titles2/sizeof titles2[0]; ++i) {
          titles2[i].show(tft);
        }
        tft.fillRect(titles2[0]._cx,titles2[0]._cy+16,titles2[sizeof titles2/sizeof titles2[0]-1]._cx-titles2[0]._cx+T2_SPACE,1,ST77XX_BLUE);
        delay(3000);
        tft.fillRect(0,64,tft.width(),20,ST77XX_BLACK);
        tft.fillRect(0,96,tft.width(),20,ST77XX_BLACK);
        for(int i = 0; i < sizeof titles2/sizeof titles2[0]; ++i) {
          titles2[i].randompos();
          titles2[i].show(tft);
          Serial.printf("#titles2 random = %c\r\n",titles2[i]._char);
          delay(speed2);
        }
        delay(3000);
      }
      else {
        //try to move next;
        Serial.println("#titles2 move next");
        for(int i = 0; i < sizeof titles2/sizeof titles2[0]; ++i) {
          titles2[i].move_next(tft);
        }
      }
    }
  } 
}

void testlines(uint16_t color) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(0, 0, x, tft.height()-1, color);
    delay(0);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(0, 0, tft.width()-1, y, color);
    delay(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(tft.width()-1, 0, x, tft.height()-1, color);
    delay(0);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(tft.width()-1, 0, 0, y, color);
    delay(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(0, tft.height()-1, x, 0, color);
    delay(0);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(0, tft.height()-1, tft.width()-1, y, color);
    delay(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(tft.width()-1, tft.height()-1, x, 0, color);
    delay(0);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(tft.width()-1, tft.height()-1, 0, y, color);
    delay(0);
  }
}

void testdrawtext(char *text, uint16_t color) {
  //tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

void testfastlines(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t y=0; y < tft.height(); y+=5) {
    tft.drawFastHLine(0, y, tft.width(), color1);
  }
  for (int16_t x=0; x < tft.width(); x+=5) {
    tft.drawFastVLine(x, 0, tft.height(), color2);
  }
}

void tftPrintTest() {
  tft.setTextWrap(false);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 30);
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(2);
  tft.println("Hello World!");
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(3);
  tft.println("Hello World!");
  tft.setTextColor(ST77XX_BLUE);
  tft.setTextSize(4);
  tft.print(1234.567);
  delay(1500);
  tft.setCursor(0, 0);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(0);
  tft.println("Hello World!");
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_GREEN);
  tft.print(p, 6);
  tft.println(" Want pi?");
  tft.println(" ");
  tft.print(8675309, HEX); // print 8,675,309 out in HEX!
  tft.println(" Print HEX!");
  tft.println(" ");
  tft.setTextColor(ST77XX_WHITE);
  tft.println("Sketch has been");
  tft.println("running for: ");
  tft.setTextColor(ST77XX_MAGENTA);
  tft.print(millis() / 1000);
  tft.setTextColor(ST77XX_WHITE);
  tft.print(" seconds.");
}

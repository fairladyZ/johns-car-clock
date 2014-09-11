/***************************************************
  This sketch is a replacement clock for a 1987 Nissan 300zx
  It uses the Adafruit 1.8" TFT LCD, and DS1302 Real Time CLock
 ****************************************************/
//#define sclk 13
//#define mosi 11
#define cs   10
#define dc   9
#define rst  0  // you can also connect this to the Arduino reset

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <virtuabotixRTC.h>   
#include <math.h>

// Creation of the Real Time Clock Object\
//SCLK -> 2, I/O -> 3, CE -> 4
virtuabotixRTC myRTC(2, 3, 4);

const int IndicatorLightPin = A0;     
const int engineSensor = A1;
const int resetButton = A3;
const int mmButton = 5;
const int Backlight = 6; 
const int hhButton = 7;
const int selectButton = 8;

// variables will change:
int IndicatorLight = 0;         // variable for reading the pushbutton status
double engineTempSensor = 0;
int mode = 0;
int hh = 0;
int mm = 0;
int select = 0;
double oldTempSensor = 0;
int engineTempReadout = 0;

double Thermistor(int RawADC) {
 double Temp;
 //Temp = log(10000.0*((1024.0/RawADC-1))); 
 Temp = log(10000.0/(1024.0/RawADC-1)); // for pull-up configuration
 Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
 Temp = Temp - 273.15;            // Convert Kelvin to Celcius
 Temp = (Temp * 9.0)/ 5.0 + 32.0; // Convert Celcius to Fahrenheit
 return Temp;
}

// Option 2: must use the hardware SPI pins
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, rst);

void setup(void) {
  
  analogWrite(Backlight, 255);
  
  //Serial.begin(9600);  // for debug
  
  // initialize the pushbutton pin as an input:
  //pinMode(IndicatorLightPin, INPUT); 
  pinMode(hhButton, INPUT); 
  pinMode(mmButton, INPUT);
  pinMode(selectButton, INPUT);  
  
  // Virtual Box Library: Set the current date, and time in the following format:
  // seconds, minutes, hours, day of the week, day of the month, month, year
  //myRTC.setDS1302Time(00, 28, 13, 30, 8, 1, 2014);
  
  // Our supplier changed the 1.8" display slightly after Jan 10, 2012
  // so that the alignment of the TFT had to be shifted by a few pixels
  // this just means the init code is slightly different. Check the
  // color of the tab to see which init code to try. If the display is
  // cut off or has extra 'random' pixels on the top & left, try the
  // other option!
  // If you are seeing red and green color inversion, use Black Tab

  // If your TFT's plastic wrap has a Black Tab, use the following:
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip,Black tab
  // If your TFT's plastic wrap has a Red Tab, use the following:
  //tft.initR(INITR_REDTAB);   // initialize a ST7735R chip, red tab
  // If your TFT's plastic wrap has a Green Tab, use the following:
  //tft.initR(INITR_GREENTAB); // initialize a ST7735R chip, green tab

  //Serial.println("init");

  tft.fillScreen(ST7735_BLACK);    // Clear the display to black 
  tft.setRotation(3);              // Place the display in landscape 270° Mode
  
  //tft.drawRect(50, 45 , 5, 24, 0xFFFF);
  //tft.drawRect(60, 45 , 5, 24, 0xFFFF);
  //tft.drawTriangle(10, 45, 15, 50, 20, 45, 0xFFFF);
  // tft print function!
  //tftPrintDayTime();
  //delay(2000);
  //tftdrawCoolant(10, 10);
  //analogWrite(Backlight, 255);
  tftdrawNissan(0,60,0xFFFF,0x0000);
  tftFadeUp(250, 25, 5, 30);
  tftFadeDown(25,250, 5, 30);
  tftdrawNissan(0,60,0x0000,0x0000);
  tft.setCursor(20, 70);  //Set position
  tft.setTextColor(0xFFFF,0x0000);  //Set font to white, fill negative space
  tft.setTextSize(2);
  tft.print("BY");
  tftdrawJPD(55,60,0xFFFF,0x0000);
  tftFadeUp(250, 25, 5, 30);
  tftFadeDown(25,250, 5, 30);
  tft.setTextColor(0x0000,0x0000);
  tft.setCursor(20, 70);
  tft.print("BY");
  tftdrawJPD(55,60,0x0000,0x0000);
  tftPrintTime(0xFFFF, 0x0000);
  tftFadeUp(250, 25, 5, 30);
}

void loop() {
  
  myRTC.updateTime();   // Update Time from RAM
  //Serial.print(myRTC.hours);
  //Serial.print(" : ");
  //Serial.println(myRTC.minutes);
  //delay(500);
  
  if(mode == 0)
  {
    IndicatorLight = analogRead(IndicatorLightPin);
    //Serial.println(IndicatorLight);
    //IndicatorLight = 0;
    hh = digitalRead(hhButton);
    mm = digitalRead(mmButton);
  
    if(IndicatorLight < 1){
      tftPrintTime(0xFFFF, 0x0000);
      delay(100);
    }
    else {
      tftPrintTime(0xF800, 0x0000);
      delay(100);
    }
    select = digitalRead(selectButton);
    if (select == 1){
      mode++;
      tft.drawRect(10, 60, 25, 160, 0x0000);
      //tftPrintTime(0x0000, 0x0000);
    }
  }
 
  else if(mode == 1)
  {
    tftdrawNissan(0,60,0xFFFF,0x0000);
    select = digitalRead(selectButton);
    if (select == 1){
      mode++;
      tftdrawNissan(0,60,0x0000,0x0000);
      //tft.drawRect(0, 60, 160, 30, 0x0000);
    }
    //mode = mode += digitalRead(selectButton);
  }
  else
  {
    //tftdrawJPD(0,60,0xFFFF,0x0000);
    tftdrawCoolant(20, 55, 0xFFFF, 0x0000);
    tftPrintTemp(45, 60, 0xFFFF, 0x0000, 3);
    select = digitalRead(selectButton);
    if (select == 1){
      mode=0;
      tft.fillRect(20, 50, 16, 40, 0x0000);
    }
    delay(50);
    //mode = mode += digitalRead(selectButton);
  }
  
  // increment hours if button has been pressed
  if(hh == 1){                         //Check hh button
    myRTC.updateTime();                //Update RTC values
    int hourUpdate = myRTC.hours;      
    if (hourUpdate < 24){              //When less than 24 increment
      hourUpdate++;
    }
    else{
      hourUpdate = 1;                  //When 24 rollover (RAM permits erronious values)
    }
    myRTC.setDS1302Time(myRTC.seconds, myRTC.minutes, hourUpdate, myRTC.dayofweek, myRTC.dayofmonth, myRTC.month, myRTC.year);  //Set Time with "hourUpdate
  }
  
  // increment minutes if button has been pressed
  if(mm == 1){                         //Check mm button
    myRTC.updateTime();                //Update RTC values
    int minuteUpdate = myRTC.minutes; 
    if (minuteUpdate < 59){            //When less than 59 minutes increment
      minuteUpdate++;
    }
    else{
      minuteUpdate = 1;                //When 59 rollover (RAM permits erronious values)
    }
    //delay(250);
    myRTC.setDS1302Time(myRTC.seconds, minuteUpdate, myRTC.hours, myRTC.dayofweek, myRTC.dayofmonth, myRTC.month, myRTC.year);
    //delay(400);
  }
  
  delay(100);
}


void tftPrintTemp(int x, int y, int fontColor, int backColor, int point) {   
  
  tft.setTextWrap(false);
  //tft.setCursor(x, y);  //Set position
  tft.setTextSize(point);
  
  engineTempSensor = analogRead(engineSensor);
  engineTempSensor = log(10000.0*((1024.0/engineTempSensor-1))); // for pull-up configuration
  engineTempSensor = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * engineTempSensor * engineTempSensor ))* engineTempSensor );
  engineTempSensor = engineTempSensor - 273.15;            // Convert Kelvin to Celcius
  engineTempSensor = (engineTempSensor * 9.0)/ 5.0 + 32.0; // Convert Celcius to Fahrenheit
  
  engineTempReadout = engineTempSensor / 5;
  engineTempReadout = engineTempReadout * 5;
  
  tft.setTextColor(fontColor,backColor);  //Set font to white, fill negative space
        tft.setCursor(x, y);
        tft.print(engineTempReadout);
        tft.print("<F ");
  //Serial.println(oldTempSensor);
  /*
      if (oldTempSensor != engineTempSensor){
        //tft.setCursor(x, y);  //Set position
        //tft.setTextColor(backColor,backColor);
        //tft.print(oldTempSensor);
        //tft.drawRect(10, 60, 160, 25, 0x0000);
        tft.setTextColor(fontColor,backColor);  //Set font to white, fill negative space
        tft.setCursor(x, y);
        tft.print(engineTempSensor);
        oldTempSensor = engineTempSensor;
        //Serial.println(engineTempSensor);
      } */
}

// Display the time with chosen font color
void tftPrintTime(int fontColor, int backColor) {   
  myRTC.updateTime();   // Update Time from RAM
  tft.setTextWrap(false);
  tft.setCursor(10, 60);  //Set position
  tft.setTextColor(fontColor,backColor);  //Set font to white, fill negative space
  tft.setTextSize(3);
  if (myRTC.hours <= 12){  //Display hours when the time is 12 or less
    if (myRTC.hours <= 9){ //Add a space if the hour is less than 10
      tft.print(" ");
      tft.print(myRTC.hours);
      tft.print(":");
    }
    else{
        tft.print(myRTC.hours);  //Display hours with 10 or later,But not later than 12 as previously tested
        tft.print(":");
      }
    }
  else{
    if (myRTC.hours <= 21){   //Display hours with a space if less than 22 and greater than 12 as previously tested
      tft.print(" ");
      tft.print(myRTC.hours - 12);
      tft.print(":");
    }
    else{                     //Display raw hours when 22 and over
      tft.print(myRTC.hours - 12);
      tft.print(":");
    }
    }
  if (myRTC.minutes < 10){   //Display minutes less than 10 with a leading zero
    tft.print("0");
    tft.print(myRTC.minutes);
    }
  else{
    tft.print(myRTC.minutes); //Display minutes 10 and over with no spaces
    }
  if (myRTC.hours <= 11){  //Display "AM" from 1AM to 11AM
    tft.print(" AM  ");
    }
  else{
    if (myRTC.hours <=23){ //Display "PM" from 1PM to 11PM
    tft.print(" PM  ");
    }
    else{
      tft.print(" AM  ");  //Don't forget 12AM, that weirdo
    }
    }
}

//16x35
const unsigned char PROGMEM icon16_thermometer[] =
{ B00000011, B11000000,
  B00000110, B01100000,
  B00001100, B00110000,
  B00001100, B00110000,
  B00001111, B00110000,
  B00001100, B00110000,
  B00001100, B00110000,
  B00001111, B00110000,
  B00001100, B00110000,
  B00001100, B00110000,
  B00001111, B00110000,
  B00001100, B00110000,
  B00001100, B00110000,
  B00001111, B00110000,
  B00001100, B00110000,
  B00001100, B00110000,
  B00001111, B00110000,
  B00001100, B00110000,
  B00001100, B00110000,
  B00001111, B00110000,
  B00001100, B00110000,
  B00011100, B00111000,
  B00111100, B00111100,
  B01110000, B00001110,
  B01100000, B00000110,
  B11100000, B00000111,
  B11000000, B00000011,
  B11000000, B00000011,
  B11100000, B00000111,
  B01100000, B00000110,
  B01110000, B00001110,
  B00111000, B00011100,
  B00011110, B01111000,
  B00001111, B11110000,
  B00000011, B11000000
};

//16*16
static unsigned char PROGMEM icon16_coolant[] =
{ B00000000,B00000000,
  B00000001,B00000000,
  B00000001,B11100000,
  B00000001,B00000000,
  B00000001,B11100000,
  B00000001,B00000000,
  B00000001,B11100000,
  B00000001,B00000000,
  B01100011,B10001100,
  B10011011,B10110011,
  B00000000,B00000000,
  B01100110,B01100110,
  B10011001,B10011001,
  B00000000,B00000000,
  B00000000,B00000000,
  B00000000,B00000000
};

//180*27
static unsigned char PROGMEM icon_nissan[] =
//160*24
{
B11111111,B11000000,B00000111,B11100000,B11111100,B00000111,B11111111,B11111111,B11100000,B01111111,B11111111,B11111111,B00000000,B00011111,B11111110,B00000000,B00011111,B11111110,B00000000,B01111111,
B11111111,B11100000,B00000111,B11100000,B11111100,B00001111,B11111111,B11111111,B11100000,B11111111,B11111111,B11111111,B00000000,B00011111,B11111110,B00000000,B00011111,B11111110,B00000000,B01111111,
B11111111,B11110000,B00000111,B11100000,B11111100,B00011111,B11111111,B11111111,B11100000,B11111111,B11111111,B11111111,B00000000,B00111111,B11111111,B00000000,B00011111,B11111111,B00000000,B01111111,
B11111111,B11111000,B00000111,B11100000,B11111100,B00111111,B11111111,B11111111,B11100001,B11111111,B11111111,B11111111,B00000000,B00111111,B11111111,B00000000,B00011111,B11111111,B00000000,B01111111,
B11111111,B11111000,B00000111,B11100000,B11111100,B00111111,B11111111,B11111111,B11100001,B11111111,B11111111,B11111111,B00000000,B01111111,B11111111,B00000000,B00011111,B11111111,B10000000,B01111111,
B11111111,B11111100,B00000111,B11100000,B11111100,B00111111,B10000000,B00000000,B00000001,B11111100,B00000000,B00000000,B00000000,B01111111,B00111111,B00000000,B00011111,B11111111,B10000000,B01111111,
B11111111,B11111100,B00000111,B11100000,B11111100,B00111111,B00000000,B00000000,B00000001,B11111100,B00000000,B00000000,B00000000,B01111111,B00011111,B10000000,B00011111,B10111111,B11000000,B01111111,
B11111101,B11111110,B00000111,B11100000,B11111100,B00111111,B10000000,B00000000,B00000001,B11111100,B00000000,B00000000,B00000000,B11111111,B00011111,B11000000,B00011111,B10011111,B11100000,B01111111,
B11111100,B11111111,B00000111,B11100000,B11111100,B00111111,B11111111,B11111110,B00000001,B11111111,B11111111,B11110000,B00000000,B11111110,B00001111,B11000000,B00011111,B10001111,B11110000,B01111111,
B11111100,B11111111,B00000111,B11100000,B11111100,B00111111,B11111111,B11111111,B10000001,B11111111,B11111111,B11111110,B00000000,B11111100,B00001111,B11100000,B00011111,B10000111,B11111000,B01111111,
B11111100,B01111111,B10000111,B11100000,B11111100,B00011111,B11111111,B11111111,B11000000,B11111111,B11111111,B11111111,B00000001,B11111100,B00001111,B11100000,B00011111,B10000111,B11111000,B01111111,
B11111100,B01111111,B10000111,B11100000,B11111100,B00011111,B11111111,B11111111,B11100000,B11111111,B11111111,B11111111,B00000001,B11111100,B00000111,B11100000,B00011111,B10000011,B11111100,B01111111,
B11111100,B00111111,B11000111,B11100000,B11111100,B00000111,B11111111,B11111111,B11100000,B01111111,B11111111,B11111111,B00000001,B11111000,B00000111,B11110000,B00011111,B10000001,B11111110,B01111111,
B11111100,B00111111,B11100111,B11100000,B11111100,B00000001,B11111111,B11111111,B11110000,B00001111,B11111111,B11111111,B00000011,B11111000,B00000111,B11110000,B00011111,B10000001,B11111110,B01111111,
B11111100,B00001111,B11110111,B11100000,B11111100,B00000000,B00000000,B11111111,B11110000,B00000000,B00000001,B11111111,B00000011,B11111111,B11111111,B11111000,B00011111,B10000000,B11111111,B01111111,
B11111100,B00001111,B11110111,B11100000,B11111100,B00000000,B00000000,B00000111,B11110000,B00000000,B00000000,B00111111,B00000111,B11111111,B11111111,B11111000,B00011111,B10000000,B11111111,B01111111,
B11111100,B00000111,B11111111,B11100000,B11111100,B00000000,B00000000,B00000011,B11110000,B00000000,B00000000,B00111111,B00000111,B11111111,B11111111,B11111100,B00011111,B10000000,B01111111,B11111111,
B11111100,B00000011,B11111111,B11100000,B11111100,B00111111,B11111111,B11111111,B11110000,B11111111,B11111111,B11111111,B00001111,B11111111,B11111111,B11111110,B00011111,B10000000,B01111111,B11111111,
B11111100,B00000011,B11111111,B11100000,B11111100,B00111111,B11111111,B11111111,B11110000,B11111111,B11111111,B11111111,B00011111,B11000000,B00000000,B11111110,B00011111,B10000000,B00111111,B11111111,
B11111100,B00000001,B11111111,B11100000,B11111100,B00111111,B11111111,B11111111,B11100000,B11111111,B11111111,B11111111,B00011111,B11000000,B00000000,B11111110,B00011111,B10000000,B00011111,B11111111,
B11111100,B00000000,B11111111,B11100000,B11111100,B00111111,B11111111,B11111111,B11100000,B11111111,B11111111,B11111111,B00011111,B10000000,B00000000,B11111111,B00011111,B10000000,B00011111,B11111111,
B11111100,B00000000,B11111111,B11100000,B11111100,B00111111,B11111111,B11111111,B11000000,B11111111,B11111111,B11111110,B00111111,B10000000,B00000000,B01111111,B00011111,B10000000,B00001111,B11111111,
B11111100,B00000000,B01111111,B11100000,B11111100,B00111111,B11111111,B11111111,B11000000,B11111111,B11111111,B11111110,B00111111,B10000000,B00000000,B01111111,B00011111,B10000000,B00001111,B11111111,
B11111100,B00000000,B00111111,B11100000,B11111100,B00011111,B11111111,B11111111,B10000000,B11111111,B11111111,B11100000,B00111111,B00000000,B00000000,B00111111,B00011111,B10000000,B00000111,B11111111,
};

void tftdrawCoolant(int x, int y, int main, int back){
  tft.drawBitmap(x, y,  icon16_thermometer, 16, 35, main, back);
}

void tftFadeUp(int first, int last, int value, int dly){
  for(int fadeValue = first ; fadeValue >= last; fadeValue -= value) { 
    // sets the value (range from first to last):
    analogWrite(Backlight, fadeValue);         
    // wait for dly milliseconds to see the dimming effect    
    delay(dly);                            
  } 
}

void tftFadeDown(int first, int last, int value, int dly){
for(int fadeValue = first ; fadeValue <= last; fadeValue += value) { 
    // sets the value (range from 0 to 255):
    analogWrite(Backlight, fadeValue);         
    // wait for 30 milliseconds to see the dimming effect    
    delay(dly);                            
  } 
}

void tftdrawNissan(int x, int y, int main, int back)
{
  tft.drawBitmap(x, y,  icon_nissan, 160, 24, main, back);
}

void tftdrawJPD(int x, int y, int main, int back)
{
  tft.fillTriangle(12+x, 24+y, 0+x, 12+y, 12+x, 12+y, main);
  tft.drawRect(18+x, 0+y, 5, 25, main);
  tft.drawRect(28+x, 0+y, 5, 25, main);
  tft.fillTriangle(38+x, 0+y, 38+x, 12+y, 50+x, 12+y, main);
  tft.drawRect(55+x, 0+y, 5, 25, main);
  tft.fillTriangle(65+x, 0+y, 65+x, 24+y, 77+x, 12+y, main);
}
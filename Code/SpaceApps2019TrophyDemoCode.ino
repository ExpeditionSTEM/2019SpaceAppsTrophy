
////////////////////////////////////////////////////////////////
//////////  2019 SPACE APPS HUNTSVILLE - TROPHY DEMO CODE - BY ANDREW DENIO 
////////////////////////////////////////////////////////////////

/*
  THIS CODE WAS USED TO DEMO THE TROPHY FOR THE AWARD CEREMONY ON SUNDAY, OCTOBER 20, 2019.
  THERE WERE A LOT OF LESSONS LEARNED DURING THIS PROCESS, AND ONE OF THEM WAS THAT I PROBABLY SHOULD HAVE 
  USED A DIFFERENT MICROCONTROLLER BECAUSE THIS CODE REACHES 99% OF THE MEMORY SPACE OF THE 32U4.
  THEREFORE, I WAS UNABLE TO INCLUDE ALL FUNCTIONALITY OF THE MICROCONTROLLER AND ACCESSORIES IN A SINGLE SKETCH.

  HOWEVER, MOST FUNCTIONALITY IS HERE.  WHAT IS MISSING IS SD CARD USE AND USE OF THE BLUETOOTH MODULE.

  ALL LIBRARIES FOR COMPONENTS WILL BE INCLUDED IN GITHUB LINK, SO FEEL FREE TO TINKER AROUND!
  HTTPS://GITHUB.COM/EXPEDITIONSTEM/2019SPACEAPPSTROPHY

  LIBRARIES FOR QUAD ALPHA-NUMERIC DISPLAY, FEATHER 32U4, EINK DISPLAY, NEOPIXELS, ACCELEROMETER, TEMPERATURE SENSOR WERE PROVIDED BY ADAFRUIT

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!
  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
  
 */




////////////////////////////////////////////////////////////////
//////////  ADAFRUIT FEATHER 32U4 PINOUT
////////////////////////////////////////////////////////////////

/*

THESE ARE THE PINS USED BY THIS DEMO AND COMPONENTS

A0 - BUTTON 1
A1 - BUTTON 2
A2 - BUTTON 3
A3 - BUTTON 4
A4 - LIGHT SENSOR
A5 - FREE TO USE

5 - EPD - SD CARD CHIP SELECT (NOT USED IN DEMO)
6 - EPD - SRAM CHIP SELECT
9 - EPD - EPD CHIP SELECT
10 - EPD - DC
11 - FREE TO USE
12 - NEOPIXEL DATA OUT
13 - FREE TO USE

 */


////////////////////////////////////////////////////////////////
//////////  VARIABLES AND LIBRARIES
////////////////////////////////////////////////////////////////

/*

AWARD TYPE:
0 - HIGH SCHOOL FIRST PLACE
1 - HIGH SCHOOL SECOND PLACE
2 - PROFESSIONAL FIRST PLACE
3 - PROFESSIONAL SECOND PLACE
4 - BEST DISPLAY OF TEAMWORK
5 - CHAMPIONS OF INNOVATION
6 - LEAN FORWARD FAIL SMART
7 - BEST USE OF CREATIVITY
8 - STEAM WORKS
9 - SPACE APPS
*/
 
byte awardtype = 9; //CHANGE THIS VALUE TO CHANGE THE AWARD TYPE

#include "Adafruit_EPD.h"  // Library for Eink Display
#define SRAM_CS     6
#define EPD_CS      9
#define EPD_DC      10  
#define EPD_RESET   -1 // can set to -1 and share with microcontroller Reset!
#define EPD_BUSY    -1 // can set to -1 to not use a pin (will wait a fixed delay)
/* Uncomment the following line if you are using 2.13" monochrome 250*122 EPD */
Adafruit_SSD1675 epd(250, 122, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);


//NEOPIXEL SETUP
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#define PIN 12
// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      10
// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.


// Assign Input Pins (Buttons and Light sensor)
#define BUTTON_1 A0
#define BUTTON_2 A1
#define BUTTON_3 A2
#define BUTTON_4 A3
#define LIGHT_SENSOR_PIN A4

// New and Old state variables used for Buttons
bool button1OldState = HIGH;
bool button2OldState = HIGH;
bool button3OldState = HIGH;
bool button4OldState = HIGH;
bool button1NewState;
bool button2NewState;
bool button3NewState;
bool button4NewState;


// Quad-Alpha Numeric Display Setup
#include <Wire.h>
//#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();


// Variables used for scrolling through menus.  
byte NewMenuVar = 1;
byte NewerMenuVar = 0;
byte MenuCount = 6;
byte OldMenuVar = MenuCount + 1; 


// Temperature Sensor Setup
//#include <Wire.h>  // already declared
#include "Adafruit_ADT7410.h"
// Create the ADT7410 temperature sensor object
Adafruit_ADT7410 tempsensor = Adafruit_ADT7410();


// Accelerometer Setup
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL343.h>
/* Assign a unique ID to this sensor at the same time */
/* Uncomment following line for default Wire bus      */
Adafruit_ADXL343 accel = Adafruit_ADXL343(12345);


// Variables Used for menu options
bool epdMainMenu = 0;
char displaybuffer[4] = {' ', ' ', ' ', ' '};
bool NeoPixelMenuSelected = 0;
bool LightSensorMenuSelected = 0;
bool TemperatureMenuSelected = 0;
bool AccelMenuSelected = 0;
bool DemoMenuSelected = 0;


////////////////////////////////////////////////////////////////
//////////  SETUP LOOP
////////////////////////////////////////////////////////////////

void setup() {

  //Start Serial for feedback on Serial Monitor
  Serial.begin(9600);

  //Setup input pins
  pinMode(BUTTON_1, INPUT_PULLUP);
  pinMode(BUTTON_2, INPUT_PULLUP);
  pinMode(BUTTON_3, INPUT_PULLUP);
  pinMode(BUTTON_4, INPUT_PULLUP);
  pinMode(LIGHT_SENSOR_PIN, INPUT);

  //Start EPD 
  epd.begin();
  epd.setTextWrap(true);
  epd.setRotation(0);
  epd.setTextColor(EPD_BLACK);

  //Show award menu.  0 will display button assignments, 1 will display 'demo mode'
  goToAwardMenu2(0);

  //begin neopixels
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  colorWipe(strip.Color(0, 0, 0), 0); // Turn Off

  //begin quad alpha numeric display
  alpha4.begin(0x70);  // pass in the address
  alpha4.clear();

  //begin accelerometer
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL343 ... check your connections */
    Serial.println("Ooops, no ADXL343 detected ... Check your wiring!");
    while(1);
  }
  /* Set the range to whatever is appropriate for your project */
  //accel.setRange(ADXL343_RANGE_16_G);
  // accel.setRange(ADXL343_RANGE_8_G);
   accel.setRange(ADXL343_RANGE_4_G);
  // accel.setRange(ADXL343_RANGE_2_G);

  //begin temperature sensor
  if (!tempsensor.begin()) {
    Serial.println("Couldn't find ADT7410!");
    while (1);
  }

  //display a 'starting' message
  quadDisplayString("STARTING...");
   
  // sensor takes 250 ms to get first readings
  delay(250);

  
}

////////////////////////////////////////////////////////////////
//////////  MAIN LOOP
////////////////////////////////////////////////////////////////

void loop() {
  // Check Each button to see if they have been pressed
  CheckButtons(); 

  //Asssignment needed for menu
  OldMenuVar = NewMenuVar; 
}


////////////////////////////////////////////////////////////////
//////////  OTHER FUNCTIONS
////////////////////////////////////////////////////////////////

//Check Each Button
void CheckButtons(){
  CheckButton1();
  CheckButton2();
  CheckButton3();
  CheckButton4();
}

////////////////////////////////////////////////////////////////

void CheckButton1(){

    // Get current button state.
  button1NewState = digitalRead(BUTTON_1);

  // Check if state changed from high to low (button press).
  if (button1NewState == LOW && button1OldState == HIGH) {
    // Short delay to debounce button.
    delay(20);
    // Check if button is still low after debounce.
    button1NewState = digitalRead(BUTTON_1);
    if (button1NewState == LOW) {
      // BUTTON PRESSED
     //Serial.println("BUTTON 1 PRESSED");
      if (epdMainMenu == 1 || NeoPixelMenuSelected == 1 || TemperatureMenuSelected == 1 || AccelMenuSelected == 1 || DemoMenuSelected == 1){
        NewMenuVar = OldMenuVar + 1;
          if (NewMenuVar == MenuCount+1) {
              NewMenuVar = 1;
          }
          startQuadMenuCounter();
        }
     //Serial.println("button done");
    }
    else {
      // BUTTON NOT PRESSED 
    }
  }

  // Set the last button state to the old state.
  button1OldState = button1NewState;

}

////////////////////////////////////////////////////////////////

void CheckButton2(){

    // Get current button state.
  button2NewState = digitalRead(BUTTON_2);

  // Check if state changed from high to low (button press).
  if (button2NewState == LOW && button2OldState == HIGH) {
    // Short delay to debounce button.
    delay(20);
    // Check if button is still low after debounce.
    button2NewState = digitalRead(BUTTON_2);
    if (button2NewState == LOW) {
      // BUTTON PRESSED
     //Serial.println("BUTTON 2 PRESSED");
      if (epdMainMenu == 1 || NeoPixelMenuSelected == 1 || TemperatureMenuSelected == 1 || AccelMenuSelected == 1 || DemoMenuSelected == 1){        
        NewMenuVar = OldMenuVar - 1;
          if (NewMenuVar == 0) {
              NewMenuVar = MenuCount;
          }
          startQuadMenuCounter();
        }
     //Serial.println("button done");
    }
    else {
      // BUTTON NOT PRESSED 
    }
  }

  // Set the last button state to the old state.
  button2OldState = button2NewState;

}

////////////////////////////////////////////////////////////////

void CheckButton3(){

    // Get current button state.
  button3NewState = digitalRead(BUTTON_3);

  // Check if state changed from high to low (button press).
  if (button3NewState == LOW && button3OldState == HIGH) {
    // Short delay to debounce button.
    delay(20);
    // Check if button is still low after debounce.
    button3NewState = digitalRead(BUTTON_3);
    if (button3NewState == LOW) {
      // BUTTON PRESSED
    // Serial.println("BUTTON 3 PRESSED");
    printLEDString("XXXX");
     selectMenuItem();
     printLEDString("    ");
     //epdMainMenu = 0;
     //Serial.println("done"); 
    }
    else {
      // BUTTON NOT PRESSED 
    }
  }

  // Set the last button state to the old state.
  button3OldState = button3NewState;

}

////////////////////////////////////////////////////////////////

void CheckButton4(){

    // Get current button state.
  button4NewState = digitalRead(BUTTON_4);

  // Check if state changed from high to low (button press).
  if (button4NewState == LOW && button4OldState == HIGH) {
    // Short delay to debounce button.
    delay(20);
    // Check if button is still low after debounce.
    button4NewState = digitalRead(BUTTON_4);
    if (button4NewState == LOW) {
      // BUTTON PRESSED
    // Serial.println("BUTTON 4 PRESSED");
    printLEDString("XXXX");
    epdMainMenu = 1;
    //changeEPDToMainMenu();
    printEPDMenu("MAIN", "NEOPIXELS", "LIGHT SENSOR", "TEMPERATURE SENSOR", "ACCELEROMETER", "DEMO MODE", " ", 1);
    startQuadMenuCounter();
    printLEDString("    ");
   // Serial.println("button done");
    }
    else {
      // BUTTON NOT PRESSED 
    }
  }

  // Set the last button state to the old state.
  button4OldState = button4NewState;

}

////////////////////////////////////////////////////////////////
    
void selectMenuItem(){

  if (epdMainMenu == 1){
          if (NewMenuVar == 1) {
            NeoPixelMenuSelected = 1;
            LightSensorMenuSelected = 0;
            TemperatureMenuSelected = 0;
            AccelMenuSelected = 0;
            DemoMenuSelected = 0;
            
            //goToNeoPixelMenu();
            printEPDMenu("NEOPIXEL", "COLOR WIPE", "RAINBOW", "RAINBOW CYCLE", "THEATER CHASE", "RAINBOW CHASE", "SHOW MODE", 1);
          } 
        else if (NewMenuVar == 2) {
            NeoPixelMenuSelected = 0;
            LightSensorMenuSelected = 1;
            TemperatureMenuSelected = 0;
            AccelMenuSelected = 0;
            DemoMenuSelected = 0;
            
            //goToLightSensorMenu();
            printEPDMenu("LIGHT SENSOR", "SEE DISPLAY BELOW", "FOR SENSOR VALUE", " ", "0 IS MIN", "1024 IS MAX", " ", 0);
          }     
        else if (NewMenuVar == 3) {
            NeoPixelMenuSelected = 0;
            LightSensorMenuSelected = 0;
            TemperatureMenuSelected = 1;
            AccelMenuSelected = 0;
            //QuadMenuSelected = 0;
            DemoMenuSelected = 0;

            //goToTemperatureMenu();
            printEPDMenu("TEMPERATURE", "SEE DISPLAY BELOW", "FOR TEMPERATURE", " ", "(IN DEGREES F)", " ", " ", 0);
          }  
         else if (NewMenuVar == 4) {

            NeoPixelMenuSelected = 0;
            LightSensorMenuSelected = 0;
            TemperatureMenuSelected = 0;
            AccelMenuSelected = 1;
            //QuadMenuSelected = 0;
            DemoMenuSelected = 0;
            
            //goToAccelMenu();
            printEPDMenu("ACCELEROMETER", "X ACCELERATION", "Y ACCELERATION", "Z ACCELERATION", " ", " ", " ", 1);
          }              
          else if (NewMenuVar == 5) {
            NeoPixelMenuSelected = 0;
            LightSensorMenuSelected = 0;
            TemperatureMenuSelected = 0;
            AccelMenuSelected = 0;
            //QuadMenuSelected = 0;
            DemoMenuSelected = 1;
            
            goToAwardMenu2(1);
          }   
          
          else if (NewMenuVar == 6) {
              return;
          }               
           else  {
               // done
          }  
     epdMainMenu = 0;
  }
   
  if (NeoPixelMenuSelected == 1){
          if (NewMenuVar == 1) {
              colorWipe(strip.Color(255, 0, 0), 50);  // Red
              colorWipe(strip.Color(0, 0, 0), 50);    // Black/off
              colorWipe(strip.Color(0, 255, 0), 50);  // Green
              colorWipe(strip.Color(0, 0, 0), 50);    // Black/off
              colorWipe(strip.Color(0, 0, 255), 50);  // Blue
              colorWipe(strip.Color(0, 0, 0), 50);    // Black/off
              colorWipe(strip.Color(255, 255, 255), 50);  // Blue
              colorWipe(strip.Color(0, 0, 0), 0);    // Black/off
          } 
        else if (NewMenuVar == 2) {
              rainbow(20);
              colorWipe(strip.Color(0, 0, 0), 0);    // Black/off
          }     
        else if (NewMenuVar == 3) {
              rainbowCycle(20);
              colorWipe(strip.Color(0, 0, 0), 0);    // Black/off
          }  
         else if (NewMenuVar == 4) {
              theaterChase(strip.Color(127, 127, 127), 50); // White
              colorWipe(strip.Color(0, 0, 0), 0);    // Black/off
              theaterChase(strip.Color(127,   0,   0), 50); // Red
              colorWipe(strip.Color(0, 0, 0), 0);    // Black/off
              theaterChase(strip.Color(  0,   0, 127), 50); // Blue
              colorWipe(strip.Color(0, 0, 0), 0);    // Black/off
          }              
          else if (NewMenuVar == 5) {
              theaterChaseRainbow(50);
              colorWipe(strip.Color(0, 0, 0), 0);    // Black/off
          }   
          else if (NewMenuVar == 6) {
              showModeAndQuad(50,0);
              colorWipe(strip.Color(0, 0, 0), 0);    // Black/off
          }                      
           else  {
               // done
          }  
    quadDisplayString("----");    
  }
  if (LightSensorMenuSelected == 1){
    for (int i = 0; i < 100; i++)  {
       printLEDNumber(analogRead(LIGHT_SENSOR_PIN)); 
       delay(150);     
    }
    quadDisplayString("----");   
  }
  if (TemperatureMenuSelected == 1){
    for (int i = 0; i < 100; i++)  {
       printLEDNumber(checkTempF()); 
       delay(150);     
    }
    quadDisplayString("----");   
  }
    if (AccelMenuSelected == 1){
          if (NewMenuVar == 1) {
               for (int i = 0; i < 100; i++)  {
                 printLEDNumber(checkXaccel()); 
                 delay(150);     
              }
          } 
        else if (NewMenuVar == 2) {
               for (int i = 0; i < 100; i++)  {
                 printLEDNumber(checkYaccel()); 
                 delay(150);     
              }
          }     
        else if (NewMenuVar == 3) {
              for (int i = 0; i < 100; i++)  {
                 printLEDNumber(checkZaccel()); 
                 delay(150);     
              }
          }
        else if (NewMenuVar == 4) {
          }  
        else if (NewMenuVar == 5) {
          }  
        else if (NewMenuVar == 6) {
          }                        
           else  {
               // done
          }  
    quadDisplayString("----");    
  }
}

////////////////////////////////////////////////////////////////

void startQuadMenuCounter(){
          if (NewMenuVar == MenuCount+1) {
              NewMenuVar = 1;
             }
         
         if (NewMenuVar == 0) {
            NewMenuVar = MenuCount;
          }
        if (NewMenuVar == 1) {
            printLEDString("1");
          } 
        else if (NewMenuVar == 2) {
            printLEDString("2");
          }     
        else if (NewMenuVar == 3) {
            printLEDString("3");
          }  
         else if (NewMenuVar == 4) {
            printLEDString("4");
          }              
          else if (NewMenuVar == 5) {
            printLEDString("5");
          }   
          else if (NewMenuVar == 6) {
            printLEDString("6");
          }                      
           else  {
               // done
          }   
}

////////////////////////////////////////////////////////////////

void printEPDMenu(String MenuTitle, String Line1, String Line2, String Line3, String Line4, String Line5, String Line6, bool flag){
    epd.clearBuffer();
    epd.setCursor(0, 0);
    epd.setTextSize(2);
    epd.print(MenuTitle);
    epd.println(" MENU");

    if (flag == 1){
        epd.setCursor(0, 23);
        epd.println("1");
        epd.setCursor(0, 39);
        epd.println("2");
        epd.setCursor(0, 56);
        epd.println("3");
        epd.setCursor(0, 73);
        epd.println("4");
        epd.setCursor(0, 90);
        epd.println("5");
        epd.setCursor(0, 107);
        epd.println("6");
    }    
    
    epd.setCursor(20, 23);
    epd.println(Line1);
    epd.setCursor(20, 39);
    epd.println(Line2);
    epd.setCursor(20, 56);
    epd.println(Line3);
    epd.setCursor(20, 73);
    epd.println(Line4);
    epd.setCursor(20, 90);
    epd.println(Line5);
    epd.setCursor(20, 107);
    epd.println(Line6);
    
    epd.display(); 
}


////////////////////////////////////////////////////////////////

//This function isn't used in the demo sketch, but is available
float checkTempC(){
  //printLEDNumber(checkTempF());
  float c = tempsensor.readTempC();
  return(tempsensor.readTempC());
}


////////////////////////////////////////////////////////////////

float checkTempF(){
  //printLEDNumber(checkTempF());
  float f = tempsensor.readTempC() * 9.0 / 5.0 + 32;
  return(f);
}

////////////////////////////////////////////////////////////////

float checkXaccel(){
  //printLEDNumber(checkXaccel());
  sensors_event_t event;
  accel.getEvent(&event);
  Serial.println(event.acceleration.x);
  return(event.acceleration.x);
}

////////////////////////////////////////////////////////////////

float checkYaccel(){
  //printLEDNumber(checkYaccel());
  sensors_event_t event;
  accel.getEvent(&event);
  Serial.println(event.acceleration.y);
  return(event.acceleration.y);
}

////////////////////////////////////////////////////////////////

float checkZaccel(){
  //printLEDNumber(checkZaccel());
  sensors_event_t event;
  accel.getEvent(&event);
  Serial.println(event.acceleration.z);
  return(event.acceleration.z);
}

////////////////////////////////////////////////////////////////

void printLEDString(String inputstring) { 
  
  int strlength = inputstring.length();
  char str [strlength+1];
  inputstring.toCharArray(str, strlength+1);
  
  byte shift = 0;
 
  for (int i = 0; i < strlen(str); i++)      //for each character in str
  {
    if (str[i] == '.')  // if there is a decimal point
    {
      if (i == 0){
        alpha4.writeDigitAscii(0, '0', true);
      }
      else { 
      alpha4.writeDigitAscii(i-1, str[i-1], true);  // go back and rewrite the last digit with the dec point
      shift++;  // increment the shift counter
      }
    }
    else
    {
      alpha4.writeDigitAscii(i-shift, str[i]);  //write the character
    }
  }
  alpha4.writeDisplay();  //write to the display.
}

////////////////////////////////////////////////////////////////

void quadDisplayString(String inputstring){

      inputstring.concat("    ");
    
     byte strlength = inputstring.length();
     char inputchar [strlength+1];
     inputstring.toCharArray(inputchar, strlength+1);
     
     for(byte i = 0; i < sizeof(inputchar) - 1; i++){

        displaybuffer[0] = displaybuffer[1];
        displaybuffer[1] = displaybuffer[2];
        displaybuffer[2] = displaybuffer[3];
        displaybuffer[3] = inputchar[i];
       
        // set every digit to the buffer
        alpha4.writeDigitAscii(0, displaybuffer[0]);
        alpha4.writeDigitAscii(1, displaybuffer[1]);
        alpha4.writeDigitAscii(2, displaybuffer[2]);
        alpha4.writeDigitAscii(3, displaybuffer[3]);
       
        // write it out!
        alpha4.writeDisplay();
        
        delay(200);
     }  
}

////////////////////////////////////////////////////////////////

void printLEDNumber(double inputdoublenumber) { 
  
  //printLEDNumber(12345);

  String inputstring = String(inputdoublenumber);
  int strlength = inputstring.length();
  char str [strlength+1];
  inputstring.toCharArray(str, strlength+1);
  
  byte shift = 0;
 
  for (int i = 0; i < strlen(str); i++)      //for each character in str
  {
    if (str[i] == '.')  // if there is a decimal point
    {
      if (i == 0){
        alpha4.writeDigitAscii(0, '0', true);
      }
      else { 
      alpha4.writeDigitAscii(i-1, str[i-1], true);  // go back and rewrite the last digit with the dec point
      shift++;  // increment the shift counter
      }
    }
    else
    {
      alpha4.writeDigitAscii(i-shift, str[i]);  //write the character
    }
  }
  alpha4.writeDisplay();  //write to the display.
}

////////////////////////////////////////////////////////////////

void goToAwardMenu2(bool demoflag){
  epd.clearBuffer();
  epd.setTextSize(2);
  if (demoflag == 0){
    epd.setCursor(30, 0);
    epd.println("CONGRATULATIONS!");
  }
  else {
    epd.setCursor(65, 0);
    epd.println("DEMO MODE");
  }

  if (awardtype == 0){
    //HIGH SCHOOL FIRST PLACE
    epd.setCursor(30, 25);
    epd.setTextSize(3);
    epd.println("HIGH SCHOOL");
    epd.setTextSize(4);
    epd.setCursor(20, 55);
    epd.println("1st PLACE");
  }
  else if (awardtype == 1){
    //HIGH SCHOOL SECOND PLACE
    epd.setCursor(30, 25);
    epd.setTextSize(3);
    epd.println("HIGH SCHOOL");
    epd.setTextSize(4);
    epd.setCursor(20, 55);
    epd.println("2nd PLACE");
  }
  else if (awardtype == 2){
  //PROFESSIONAL FIRST PLACE
  epd.setCursor(21, 25);
  epd.setTextSize(3);
  epd.println("PROFESSIONAL");
  epd.setTextSize(4);
  epd.setCursor(20, 55);
  epd.println("1st PLACE");
  }
  else if (awardtype == 3){
  //PROFESSIONAL SECOND PLACE
  epd.setCursor(21, 25);
  epd.setTextSize(3);
  epd.println("PROFESSIONAL");
  epd.setTextSize(4);
  epd.setCursor(20, 55);
  epd.println("2nd PLACE");
  }
  else if (awardtype == 4){
  //BEST DISPLAY OF TEAMWORK
  epd.setCursor(26, 30);
  epd.setTextSize(2);
  epd.println("BEST DISPLAY OF");
  epd.setTextSize(4);
  epd.setCursor(20, 55);
  epd.println("TEAMWORK");
  }
  else if (awardtype == 5){
  //CHAMPIONS OF INNOVATION
  epd.setCursor(15, 27);
  epd.setTextSize(3);
  epd.println("CHAMPIONS OF");
  epd.setTextSize(4);
  epd.setCursor(0, 55);
  epd.println("INNOVATION");
  }
  else if (awardtype == 6){
  //LEAN FORWARD FAIL SMART
  epd.setCursor(15, 27);
  epd.setTextSize(3);
  epd.println("LEAN FORWARD");
  epd.setTextSize(4);
  epd.setCursor(5, 55);
  epd.println("FAIL SMART");
  }
  else if (awardtype == 7){
  //BEST USE OF CREATIVITY
  epd.setCursor(27, 27);
  epd.setTextSize(3);
  epd.println("BEST USE OF");
  epd.setTextSize(4);
  epd.setCursor(5, 55);
  epd.println("CREATIVITY");
  }  
  else if (awardtype == 8){
  //STEAM WORKS
  epd.setCursor(25, 40);
  epd.setTextSize(3);
  epd.println("STEAM WORKS");
  }
  else if (awardtype == 9){
  //SPACE APPS
  epd.setCursor(5, 40);
  epd.setTextSize(4);
  epd.println("SPACE APPS");
  }
  else {
    
  }

  if (demoflag == 0){
    epd.setTextSize(1);
    epd.setCursor(0, 100);
    epd.println("BUTTON 1 SCROLL UP");
    epd.setCursor(140, 100);
    epd.println("BUTTON 3 SELECT"); 
    epd.setCursor(0, 110);
    epd.println("BUTTON 2 SCROLL DOWN");
    epd.setCursor(140, 110);
    epd.println("BUTTON 4 MAIN MENU");
  }
  else {
    epd.setTextSize(1);
    epd.setCursor(50, 100);
    epd.println("PRESS RESET TO CONTINUE");
  }
  epd.display();

  if (demoflag == 1){
     while(1){
      showModeAndQuad(100,1);
      }
  }         
}

////////////////////////////////////////////////////////////////
 
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

////////////////////////////////////////////////////////////////

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

////////////////////////////////////////////////////////////////

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

////////////////////////////////////////////////////////////////

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

////////////////////////////////////////////////////////////////

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

////////////////////////////////////////////////////////////////

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

////////////////////////////////////////////////////////////////

void showModeAndQuad(uint8_t wait, bool quadflag) {

 char inputchar[] = "SPACE APPS HUNTSVILLE    SPACE APPS HUNTSVILLE    SPACE APPS HUNTSVILLE    SPACE APPS HUNTSVILLE    SPACE APPS HUNTSVILLE    SPACE APPS HUNTSVILLE    SPACE APPS HUNTSVILLE    SPACE APPS HUNTSVILLE    SPACE APPS HUNTSVILLE    SPACE APPS HUNTSVILLE    SPACE APPS HUNTSVILLE    ";
         
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    //Serial.println(j);
    if (quadflag == 1){

        displaybuffer[0] = displaybuffer[1];
        displaybuffer[1] = displaybuffer[2];
        displaybuffer[2] = displaybuffer[3];
        displaybuffer[3] = inputchar[j];
       
        // set every digit to the buffer
        alpha4.writeDigitAscii(0, displaybuffer[0]);
        alpha4.writeDigitAscii(1, displaybuffer[1]);
        alpha4.writeDigitAscii(2, displaybuffer[2]);
        alpha4.writeDigitAscii(3, displaybuffer[3]);
       
        // write it out!
        alpha4.writeDisplay();
    }
    
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
      delay(wait);
      
    }
  }
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////


/*************************************************************************************************************
  This is an example for Hover.
  Hover is a development kit that lets you control your hardware projects in a whole new way.
  Wave goodbye to physical buttons. Hover detects hand movements in the air for touch-less interaction.
  It also features five touch-sensitive regions for even more options.
  Hover uses I2C and 2 digital pins. It is compatible with Particle, Arduino, Raspberry Pi and more.
  Hover can be purchased here: http://www.hoverlabs.co
  Written by Emran Mahbub and Jonathan Li for Hover Labs.
  BSD license, all text above must be included in any redistribution
  ===========================================================================
#   HOOKUP GUIDE (For Arduino)
HOVER ORIGINAL ---------------
  #  PIN 1 - HOST_V+    ----    5V Pin or 3v3 Pin depending on what Arduino is running on.
  #  PIN 2 - RESET      ----    Any Digital Pin.  This example uses Pin 6.
  #  PIN 3 - SCL        ----    SCL pin
  #  PIN 4 - SDA        ----    SDA pin
  #  PIN 5 - GND        ----    Ground Pin
  #  PIN 6 - 3V3        ----    3V3 pin
  #  PIN 7 - TS         ----    Any Digital Pin.  This example uses Pin 5.
HOVER 2.0 --------------------
   # PIN 1 - HOST_V+    ----    5V Pin or 3v3 Pin depending on what Arduino is running on.
   # PIN 2 - 3.3V       ----    3V3 pin
   # PIN 3 - GND        ----    Ground Pin
   # PIN 4 - RESET      ----    Any Digital Pin.  This example uses Pin 6.
   # PIN 5 - TS         ----    Any Digital Pin.  This example uses Pin 5.
   # PIN 6 - LED        ----    Optional.  Connect to Resistor and LED for debugging.
   # PIN 7 - SCL        ----    SCL pin
   # PIN 8 - SDA        ----    SDA pin
  =============================================================================
#   OUTPUT DEFINITION
    The following table defines the event map.
    =============================================
    | Gesture Type | Gesture ID | Gesture Value |
    =============================================
    | Invalid      | 0x00       | 0x00          |
    | Right Swipe  | 0x01       | 0x01          |
    | Left Swipe   | 0x01       | 0x02          |
    | Up Swipe     | 0x01       | 0x03          |
    | Down Swipe   | 0x01       | 0x04          |
    | Airspin      | 0x02       | 0x00 to 0xFF  |
    ---------------------------------------------
    =============================================
    | Touch Type   | Touch ID   | Touch Value   |
    =============================================
    | Invalid      | 0x00       | 0x00          |
    | Touch East   | 0x01       | 0x01          |
    | Touch West   | 0x01       | 0x02          |
    | Touch North  | 0x01       | 0x03          |
    | Touch South  | 0x01       | 0x04          |
    | Touch Center | 0x01       | 0x05          |
    | Tap East     | 0x02       | 0x01          |
    | Tap West     | 0x02       | 0x02          |
    | Tap North    | 0x02       | 0x03          |
    | Tap South    | 0x02       | 0x04          |
    | Tap Center   | 0x02       | 0x05          |
    | 2x Tap East  | 0x03       | 0x01          |
    | 2x Tap West  | 0x03       | 0x02          |
    | 2x Tap North | 0x03       | 0x03          |
    | 2x Tap South | 0x03       | 0x04          |
    | 2x Tap Center| 0x03       | 0x05          |
    ---------------------------------------------
    =============================================================
    | Position Type|   x        |   y           |   z           |
    =============================================================
    | 3D Position  | 0 to 100   | 0 to 100      |   0 to 100    |
    -------------------------------------------------------------
#  HISTORY
    v1.0  -  Initial Release
    v2.0  -  Standardized Output Definition, On Github
    v2.1  -  Fixed Count Issue, Update Output String with examples
    v3.0  -  Major library update -- added 3D Position, Touch, Double Tap
    v3.1  -  Added Airspin Gesture support
#  INSTALLATION
    The 3 library files (Hover.cpp, Hover.h) are required to run Hover.
    Run the HoverDemo.ino file.
#  SUPPORT
    For questions and comments, email us at support@hoverlabs.co
*********************************************************************************************************/

#include "application.h"
#include "Hover.h"
#include "neopixel/neopixel.h"

SYSTEM_MODE(AUTOMATIC);

/* ======================= Neopixel declarations ======================== */

// IMPORTANT: Set pixel COUNT, PIN and TYPE
#define PIXEL_COUNT 24
#define PIXEL_PIN D3
#define PIXEL_TYPE WS2812B

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);


/* ======================= Hover declarations ======================== */

// pin declarations for Hover
int ts = 5;
int res = 6;

int delay1 = 40;
int delay2 = 20;

// Enable or disable different modes. Only gestures and taps are enabled by default.
// Set the following four options to 0x01 if you want to capture every event. Note that the Serial console will be 'spammy'.
#define GESTUREMODE 0x01    //0x00 = disable gestures, 0x01 = enable gestures
#define TOUCHMODE 0x00      //0x00 = disable touch, 0x01 = enable touch
#define TAPMODE 0x01        //0x00 = disable taps, 0x01 = enable taps, 0x02 = single taps only, 0x03 = double taps only
#define POSITIONMODE 0x00   //0x00 = disable position tracking, 0x01 = enable position tracking

// initialize Hover
Hover hover = Hover(ts, reset, GESTUREMODE, TOUCHMODE, TAPMODE, POSITIONMODE );

// used when printing 3D position coordinates. Using a smaller interval will result in a 'spammy' console. Set to update every 150ms by default.
long interval = 150;
long previousMillis = 0;

void setup() {
  Serial.begin(9600);
  delay(2000);
  Serial.println("Initializing Hover...please wait.");
  hover.begin();
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop(void) {

  unsigned long currentMillis = millis();    // used for updating 3D position coordinates. Set to update every 150ms by default.

  // read incoming data stream from Hover
  hover.readI2CData();

  // retreive a gesture, touch, or position
  Gesture g = hover.getGesture();
  Touch t = hover.getTouch();
  Position p = hover.getPosition();

  // print Gesture data
  if ( g.gestureID != 0){
    Serial.print("Event: "); Serial.print(g.gestureType); Serial.print("\t");
    Serial.print("Gesture ID: "); Serial.print(g.gestureID,HEX); Serial.print("\t");
    Serial.print("Value: "); Serial.print(g.gestureValue,HEX); Serial.println("");
    
    if (g.gestureID == 1){
        switch (g.gestureValue){
            case 0x01:
              Particle.publish("lifxleft", "swipeleft");
              waveLeft();
              break;
            case 0x02:
              Particle.publish("lifxright", "swiperight");
              waveRight();
              break;
            case 0x03:
              Particle.publish("lifxup","swipeup");            
              waveDown();
              break;
            case 0x04:
              Particle.publish("lifxdown","swipedown");            
              waveUp();
              break;
            default:
              break;
        }
    }
    
  }
  
}





void waveUp(){
  int brightness = 135;    
    
  for(uint16_t i=4; i<12; i++) {
    
    brightness = (i > 7) ? 255 : 135;
    
    strip.setColorDimmed(i, 0, 0, 200, brightness);
    strip.setColorDimmed(23-i, 0, 0, 200, brightness);

    strip.show();
    delay(delay1);
  }
  
  for(uint16_t i=4; i<12; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
    strip.setPixelColor(23-i, strip.Color(0, 0, 0));
    
    strip.show();
    delay(delay2);
  }    
    
}

void waveDown(){
  int brightness = 135;    
    
  for(uint16_t i=8; i>0; i--) {
    
    brightness = (i < 4) ? 255 : 135;
    
    strip.setColorDimmed(i, 0, 0, 200, brightness);
    strip.setColorDimmed(23-i, 0, 0, 200, brightness);

    strip.show();
    delay(delay1);
  }
  
  for(uint16_t i=8; i>0; i--) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
    strip.setPixelColor(23-i, strip.Color(0, 0, 0));
    
    strip.show();
    delay(delay2);
  }    
    
}



void waveRight(){
  int brightness = 135;    
    
  for(uint16_t i=0; i<6; i++) {
    
    brightness = (i > 2) ? 255 : 135;
    
    strip.setColorDimmed(12+i, 0, 0, 200, brightness);
    strip.setColorDimmed(23-i, 0, 0, 200, brightness);

    strip.show();
    delay(delay1);
  }
  
  for(uint16_t i=0; i<6; i++) {
    strip.setPixelColor(12+i, strip.Color(0, 0, 0));
    strip.setPixelColor(23-i, strip.Color(0, 0, 0));
    
    strip.show();
    delay(delay2);
  }    
    
}

void waveLeft(){
  int brightness = 135;    
    
  for(uint16_t i=0; i<6; i++) {
    
    brightness = (i > 2) ? 255 : 135;
    
    strip.setColorDimmed(12-i, 0, 0, 200, brightness);
    strip.setColorDimmed(i, 0, 0, 200, brightness);

    strip.show();
    delay(delay1);
  }
  
  for(uint16_t i=0; i<6; i++) {
    strip.setPixelColor(12-i, strip.Color(0, 0, 0));
    strip.setPixelColor(i, strip.Color(0, 0, 0));
    
    strip.show();
    delay(delay2);
  }    
    
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
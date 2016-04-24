
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

// pin declarations for Hover
int ts = 5;
int res = 6;

// Enable or disable different modes. Only gestures and taps are enabled by default.
// Set the following four options to 0x01 if you want to capture every event. Note that the Serial console will be 'spammy'.
#define GESTUREMODE 0x01    //0x00 = disable gestures, 0x01 = enable gestures
#define TOUCHMODE 0x00      //0x00 = disable touch, 0x01 = enable touch
#define TAPMODE 0x00        //0x00 = disable taps, 0x01 = enable taps, 0x02 = single taps only, 0x03 = double taps only
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
}



void loop(void) {

    // read incoming data stream from Hover
    hover.readI2CData();
    
    // retreive a gesture, touch, or position
    Gesture g = hover.getGesture();
    
    if (g.gestureID == 0x01){
        
        // if swipe gesture was detected
        switch (g.gestureValue) {
            case 0x01:
                //swiped right
                Particle.publish("lifx_action", "swipe_right", 60, PRIVATE);
                Serial.println("swipe right");
                break;
            case 0x02:
                //swiped left
                Particle.publish("lifx_action", "swipe_left", 60, PRIVATE);
                Serial.println("swipe left");
                break;
            case 0x03:
                //swiped up
                Particle.publish("lifx_action", "swipe_up", 60, PRIVATE);
                Serial.println("swipe up");
                break;
            case 0x04:
                //swiped down
                Particle.publish("lifx_action", "swipe_down", 60, PRIVATE);
                Serial.println("swipe down");
                break;
            default: 
                break;
        }    
        
    }


}
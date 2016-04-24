
/*************************************************************************************************************
#  This is an example for Hover. 
#  
#  Hover is a development kit that lets you control your hardware projects in a whole new way.  
#  Wave goodbye to physical buttons. Hover detects hand movements in the air for touch-less interaction.  
#  It also features five touch-sensitive regions for even more options.
#  Hover uses I2C and 2 digital pins. It is compatible with Arduino, Raspberry Pi and Spark Core.
#
#  Hover can be purchased here: http://www.justhover.com
#
#  Written by Emran Mahbub and Jonathan Li for Gearseven Studios.  
#  BSD license, all text above must be included in any redistribution
#  ===========================================================================
#
#  HOOKUP GUIDE (For Arduino)
#  
#    =============================
#   | 1 2 3 4 5 6 7               |                               
#   |                      HOVER  |
#   |                             |
#   | +++++++++++++++++++++++++++ |
#   | +                         + |
#   | +                         + |
#   | *                         + |
#   | *                         + |
#   | *                         + |
#   |_+++++++++++++++++++++++++++_|
#   
#  PIN 1 - HOST_V+    ----    3V3 Pin 
#  PIN 2 - RESET      ----    Any Digital Pin.  This example uses D2. 
#  PIN 3 - SCL        ----    D1 pin
#  PIN 4 - SDA        ----    D0 pin
#  PIN 5 - GND        ----    Ground Pin
#  PIN 6 - 3V3        ----    3V3 pin
#  PIN 7 - TS         ----    Any Digital Pin.  This example uses D3.
#   
#  =============================================================================
#
#  OUTPUT DEFINITION
#  The message variable outputs an 8-bit binary value to indicate the event type, gesture direction, and tap location.  
#  Upper 3 bits indicates the event type: gesture or tap.
#  Lower 5 bits indicates the gesture direction or tap location. 
#
#    EVENT TYPE     DIRECTION 
#       000           00000
#  ---------------------------------------------------------
#    GESTURES       DIRECTION FOR GESTURE
#       001            00010 - Right Swipe
#                      00100 - Left Swipe
#                      01000 - Up Swipe 
#                      10000 - Down Swipe
#
#    TAP            DIRECTION FOR TAP
#       010            00001 - South Tap
#                      00010 - West Tap
#                      00100 - North Tap
#                      01000 - East Tap
#                      10000 - Center Tap
#  ----------------------------------------------------------
#                         
#  HISTORY
#  v1.0  -  Initial Release
#  v1.1  -  Bug Fix
#  
#
#  SUPPORT
#  For questions and comments, email us at support@gearseven.com
#
*********************************************************************************************************/


#include "application.h"
#include "hover/hover.h"
//Pin declarations for Hover
int ts = D3;
int reset = D2;

Hover hover = Hover();
byte eventtype;
String outputstring;
char *message = "my name is spark";
int outputinteger;

void setup() {
    Spark.variable("mess", message, STRING);
    //Spark.function("getEvent", sendEvent);

    Serial.begin(9600);
    delay(4000);
    Serial.print("Initializing Hover...please wait.");
    hover.begin(ts, reset);

}

void sendEvent(String eventMessage)
{
   Spark.publish("getEvent", eventMessage);    
}

void loop(void) {
  
  // Check if Hover is ready to send gesture or touch events
  if (hover.getStatus(ts) == 0) {
    
    //Get the event over i2c and print it
    eventtype = hover.getEvent();
    outputstring = "";
    
    //This section can be commented out if you don't want to see the event in text format
    String temp = hover.getEventString(eventtype);
    outputstring = temp;
    if (outputstring != ""){
      Serial.print(eventtype,BIN);
      Serial.println(" = " + outputstring);
      String stringOne =  String(eventtype); 
      sendEvent(stringOne);
    }

    
    //Reset Hover for next event
    hover.setRelease(ts);
  }
}

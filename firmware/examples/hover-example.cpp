#include "examples/Hover.h"
//Pin declarations for Hover
int ts = D3;
int reset = D2;

Hover hover = Hover();
byte event;

void setup() {
    Serial.begin(9600);
    delay(4000);
    Serial.print("Initializing Hover...please wait.");
    hover.begin(ts, reset);
}

void loop(void) {
  
  // Check if Hover is ready to send gesture or touch events
  if (hover.getStatus(ts) == 0) {
    
    //Get the event over i2c and print it
    event = hover.getEvent();
    
    //This section can be commented out if you don't want to see the event in text format
    output_string = hover.getEventString(event);
    if (output_string != ""){
      Serial.print(event,BIN);
      Serial.println(" = " + output_string);
    }

    
    //Reset Hover for next event
    hover.setRelease(ts);
  }
}

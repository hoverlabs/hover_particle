// This #include statement was automatically added by the Particle IDE.
#include "application.h"
#include "Hover.h"


byte sonosip[] = { 192, 168, 1, 128 };
 
//#define SONOS_PAUSE "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:Pause xmlns:u=\"urn:schemas-upnp-org:service:AVTransport:1\"><InstanceID>0</InstanceID></u:Pause></s:Body></s:Envelope>\r\n"
//#define SONOS_PLAY  "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:Play xmlns:u=\"urn:schemas-upnp-org:service:AVTransport:1\"><InstanceID>0</InstanceID><Speed>1</Speed></u:Play></s:Body></s:Envelope>\r\n"

#define SOAP_HEADER "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body>"
#define SOAP_FOOTER "</s:Body></s:Envelope>\r\n"

#define SONOS_PAUSE "<u:Pause xmlns:u=\"urn:schemas-upnp-org:service:AVTransport:1\"><InstanceID>0</InstanceID></u:Pause>" 
#define SONOS_PLAY "<u:Play xmlns:u=\"urn:schemas-upnp-org:service:AVTransport:1\"><InstanceID>0</InstanceID><Speed>1</Speed></u:Play>"
#define SONOS_NEXT "<u:Next xmlns:u=\"urn:schemas-upnp-org:service:AVTransport:1\"><InstanceID>0</InstanceID></u:Next>"
#define SONOS_PREVIOUS "<u:Previous xmlns:u=\"urn:schemas-upnp-org:service:AVTransport:1\"><InstanceID>0</InstanceID></u:Previous>"
#define SONOS_SETVOLUME_HEADER "<u:SetVolume xmlns:u=\"urn:schemas-upnp-org:service:RenderingControl:1\"><InstanceID>0</InstanceID><Channel>Master</Channel><DesiredVolume>"
#define SONOS_SETVOLUME_FOOTER "</DesiredVolume></u:SetVolume></s:Body></s:Envelope>"
#define SONOS_TRANSPORT_INFO "<u:GetPositionInfo xmlns:u=\"urn:schemas-upnp-org:service:AVTransport:1\"><InstanceID>0</InstanceID></u:GetPositionInfo>"
#define SONOS_GETVOLUME "<u:GetVolume xmlns:u=\"urn:schemas-upnp-org:service:RenderingControl:1\"><InstanceID>0</InstanceID><Channel>Master</Channel></u:GetVolume>"

#define PAUSE 0
#define PLAY 1
#define NEXT 2
#define PREVIOUS 3
#define VOLUME 4
#define TRANSPORTINFO 5
#define GETVOLUME 6

int ts = D3;
int res = D2;

TCPClient client;
unsigned long timeout;
int sonosVolume;
char songTitle[32];
char songArtist[32];
char songVolume[12];
//char songAlbum[32];

// Enable or disable different modes. Only gestures and taps are enabled by default. 
// Set the following four options to 0x01 if you want to capture every event. Note that the Serial console will be 'spammy'. 
#define GESTUREMODE 0x01    //0x00 = disable gestures, 0x01 = enable gestures
#define TOUCHMODE 0x00      //0x00 = disable touch, 0x01 = enable touch 
#define TAPMODE 0x01        //0x00 = disable taps, 0x01 = enable taps, 0x02 = single taps only, 0x03 = double taps only
#define POSITIONMODE 0x00   //0x00 = disable position tracking, 0x01 = enable position tracking

Hover hover = Hover(ts, res, GESTUREMODE, TOUCHMODE, TAPMODE, POSITIONMODE );    

// used when printing 3D position coordinates. Using a smaller interval will result in a 'spammy' console. Set to update every 150ms by default.  
long interval = 150;        
long previousMillis = 0;

void setup()
{
    Serial.begin(9600);
    delay(4000);
    Serial.print("Initializing Hover...please wait.");
    hover.begin();
}
 
void loop()
{
    
  unsigned long currentMillis = millis();    // used for updating 3D position coordinates. Set to update every 150ms by default. 

  // Check if Hover is ready to send gesture or touch events
  hover.readI2CData();
  
  Gesture g = hover.getGesture();
  Touch t = hover.getTouch();
  Position p = hover.getPosition();  
  
  if ( g.gestureID != 0){
    if ( g.gestureValue == 0x1){
        sonos(NEXT);
    } else if ( g.gestureValue == 0x2) {
        sonos(PREVIOUS);
    } else if ( g.gestureValue == 0x3) {
        sonos(PLAY);
    } else if ( g.gestureValue == 0x4) {
        sonos(PAUSE);
    }
    
    
    Serial.print("Event: "); Serial.print(g.gestureType); Serial.print("\t");          
    Serial.print("Gesture ID: "); Serial.print(g.gestureID,HEX); Serial.print("\t");
    Serial.print("Value: "); Serial.print(g.gestureValue,HEX); Serial.println("");
  } 
  
  /*
    Serial.println("");
    Serial.println("Enter a key");
    while(!Serial.available()) SPARK_WLAN_Loop();
    char inChar = Serial.read();
    switch (inChar) {
        case 'a':    // connection open
            Serial.println("sonos play");
            sonos(PLAY);
            break;
        case 's':    // connection closed
            Serial.println("sonos pause");
            sonos(PAUSE);
            break;
        case 'n':    // connection closed
            Serial.println("sonos next");
            sonos(NEXT);
        break;
        case 'p':    // connection closed
            Serial.println("sonos previous");
            sonos(PREVIOUS);
        break;
        case 'v':    // connection closed
            Serial.println("sonos volume");
            sonos(VOLUME);
        break;
        case 'i':    // connection closed
            Serial.println("sonos get transport info");
            sonos(TRANSPORTINFO);
        break;
        case 'g':    // connection closed
            Serial.println("get volume");
            sonos(GETVOLUME);
        break;
        case 'c':
            Serial.println("Connecting...");
            if (client.connect(sonosip, 1400))
            {
                Serial.println("Connected to Sonos");
            }
            else
            {
                Serial.println("Connection failed");
            }
            break;
        case 'x':
            client.stop();
            Serial.println("Disconnected");
            break;
        case 'd':
            out("M-SEARCH * HTTP/1.1\r\n");
            out("HOST: 239.255.255.250:1900\r\n");
            out("MAN: \"ssdp:discover\"\r\n");
            out("MX: 2\r\n");
            out("ST: upnp:rootdevice\r\n");
            out("Connection:close\r\n");

    }
    //while (client.available()) {
    //    char c = client.read();
    //    Serial.print(c);
    //}*/
}




void out(const char *s)
{
    client.write( (const uint8_t*)s, strlen(s) );
    Serial.println(s);
}




void sonos(int cmd)
{
  char buf[512];
  char soapmsg[1024];
  char cmdtag[20];
   
  sonosVolume = 35;
  
  if (client.connect(sonosip, 1400)) {
    Serial.println("client is connected successfully");

    switch (cmd) {
        case PAUSE:
            sprintf(soapmsg, "%s%s%s", SOAP_HEADER, SONOS_PAUSE, SOAP_FOOTER); 
            strcpy(cmdtag, "Pause");
        break;
        case PLAY:
            sprintf(soapmsg, "%s%s%s", SOAP_HEADER, SONOS_PLAY, SOAP_FOOTER); 
            strcpy(cmdtag, "Play");
        break;
        case NEXT:
            sprintf(soapmsg, "%s%s%s", SOAP_HEADER, SONOS_NEXT, SOAP_FOOTER); 
            strcpy(cmdtag, "Next");
        break;
        case PREVIOUS:
            sprintf(soapmsg, "%s%s%s", SOAP_HEADER, SONOS_PREVIOUS, SOAP_FOOTER); 
            strcpy(cmdtag, "Previous");
        break;
        case VOLUME:
            sprintf(soapmsg, "%s%s%i%s", SOAP_HEADER, SONOS_SETVOLUME_HEADER, sonosVolume, SONOS_SETVOLUME_FOOTER); 
            strcpy(cmdtag, "SetVolume");
        break;
        case TRANSPORTINFO:
            sprintf(soapmsg, "%s%s%s", SOAP_HEADER, SONOS_TRANSPORT_INFO, SOAP_FOOTER); 
            strcpy(cmdtag, "GetPositionInfo");
        break;
        case GETVOLUME:
            sprintf(soapmsg, "%s%s%s", SOAP_HEADER, SONOS_GETVOLUME, SOAP_FOOTER); 
            strcpy(cmdtag, "GetVolume");
        break;
    }
    
    if (String(cmdtag) == "SetVolume" || String(cmdtag) == "GetVolume") {
        out("POST /MediaRenderer/RenderingControl/Control HTTP/1.1\r\n");
    } else {
        out("POST /MediaRenderer/AVTransport/Control HTTP/1.1\r\n");
    }

    sprintf(buf, "Host: %d.%d.%d.%d:1400\r\n", sonosip[0], sonosip[1], sonosip[2], sonosip[3]);
    out(buf);
    sprintf(buf, "Content-Length: %d\r\n", strlen(soapmsg));
    out(buf);
    out("Content-Type: text/xml; charset=\"utf-8\"\r\n");
    
    if (String(cmdtag) == "SetVolume" || String(cmdtag) == "GetVolume") {
        sprintf(buf, "Soapaction: \"urn:schemas-upnp-org:service:RenderingControl:1#%s\"\r\n", cmdtag);
    } else {
        sprintf(buf, "Soapaction: \"urn:schemas-upnp-org:service:AVTransport:1#%s\"\r\n", cmdtag);
    }
    out(buf);

    out("\r\n");
    out(soapmsg);

    
    /*wait 1s for timeout*/
    timeout = millis();
    while ((!client.available()) && ((millis() - timeout) < 1000));



    int i = 0;
    char sonosResponse[3072];
    while (client.available()) {
        char c = client.read();
        Serial.print(c);
        
        if (String(cmdtag) == "GetPositionInfo" || String(cmdtag) == "GetVolume") {
            sonosResponse[i] = c;
            Serial.print(sonosResponse[i]);       //This line prints the entire response
            i++;
        }
        
    }

    if (String(cmdtag) == "GetPositionInfo") {
        sonosResponse[i] = '\0';

        /* Get the song title */
        char *p1 = strcasestr(sonosResponse,"dc:title&gt;");
        char *p2 = strcasestr(sonosResponse,"&lt;/dc:title");

        int c = 0;
        for (p1 = p1 + 12; p1 != p2; p1++) {
            songTitle[c] = *p1;
            c++;
        }
        songTitle[c] = '\0';
        Serial.println(songTitle);

        /* Get the Artist */
        p1 = strcasestr(sonosResponse,"dc:creator&gt;");
        p2 = strcasestr(sonosResponse,"&lt;/dc:creator");
        c = 0;
        
        for (p1 = p1 + 14; p1 != p2; p1++) {
            songArtist[c] = *p1;
            c++;
        }
        songArtist[c] = '\0';
        Serial.println(songArtist);

        /* Get the Album */
        /*p1 = strcasestr(sonosResponse,";upnp:album&gt;");
        p2 = strcasestr(sonosResponse,"&lt;/upnp:album&gt");
        c = 0;
        
        for (p1 = p1 + 15; p1 != p2; p1++) {
            songAlbum[c] = *p1;
            c++;
        }
        songAlbum[c] = '\0';
        Serial.println(songAlbum);*/

        
    } else if (String(cmdtag) == "GetVolume") {
        sonosResponse[i] = '\0';
        
        char *p1 = strcasestr(sonosResponse,"<CurrentVolume>");
        char *p2 = strcasestr(sonosResponse,"</CurrentVolume>");

        int c = 0;
        for (p1 = p1 + 15; p1 != p2; p1++) {
            songVolume[c] = *p1;
            c++;
        }
        songVolume[c] = '\0';
        Serial.println(songVolume);
        
        sscanf(songVolume, "%d", &sonosVolume);
        Serial.println(sonosVolume);
        
    }
    
    //while (client.available()) client.read(); 
    //delay(100);
    client.stop();

    
  } else {
      Serial.println("Not yet connected to Sonos.");
  }
}


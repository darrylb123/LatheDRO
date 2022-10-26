/*
 WiFi Web Server LED Blink

 A simple web server that lets you blink an LED via the web.
 This sketch will print the IP address of your WiFi Shield (once connected)
 to the Serial monitor. From there, you can open that address in a web browser
 to turn on and off the LED on pin 5.

 If the IP address of your shield is yourAddress:
 http://yourAddress/H turns the LED on
 http://yourAddress/L turns it off

 This example is written for a network using WPA2 encryption. For insecure
 WEP or WPA, change the Wifi.begin() call and use Wifi.setMinSecurity() accordingly.

 Circuit:
 * WiFi shield attached
 * LED attached to pin 5

 created for arduino 25 Nov 2012
 by Tom Igoe

ported for sparkfun esp32 
31.01.2017 by Jan Hendrik Berlin
 
 */

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

WebServer server(80);

//Set up webserver
void initialiseWebUI(){
  // Allocate web page memory
  server.on("/", handleRoot);
  server.on("/postform/", handleForm);
  server.onNotFound(handleRoot);
  server.begin();
}

void webloopHandler(){
  server.handleClient();
}
void handleRoot() {
  Serial.println("handleRoot()");
  char tempstr[1024];

  strcpy(tempstr, "<!DOCTYPE html><html><head>\
                      <title>Rotary DRO</title></head><body>\
                      <style>\
                      table {\
  border-collapse: collapse;\
  width: 100%;\
  font-size: 30px;\
}\
\
table, th, td {\
  border: 1px solid black;\
}\
</style>\
<H1> Rotary DRO 2.54mm per turn</H1>\
<UL>\
<LI>Measure the diameter of the object to be machined\
<LI>Enter the measured diameter into the data field\
<LI>Move the tip of the tool to just touch the measured object\
<LI>Press the zero button\
</UL>\
This will change the mode to diameter display. Position of the cross slide movement will display the new diameter<BR>\
<B>Measured Diameter: </B> <form method=\"post\" enctype=\"application/x-www-form-urlencoded\" action=\"/postform/\">\
<input type=\"text\" name=\"diameter\" value=\"0\">\
<input type=\"submit\" value=\"Submit\">\
    </form>\n</body></html>\n");
    server.send(200, "text/html", tempstr);
}

void handleForm() {
  Serial.println("handleForm()");
  server.send(200, "text/plain", "Form Handling");
}


// Configure wifi using ESP Smartconfig app on phone
int mySmartConfig() {
  // Wipe current credentials
  WiFi.disconnect(true); // deletes the wifi credentials
  
  WiFi.mode(WIFI_STA);
  delay(2000);
  WiFi.begin();
  WiFi.beginSmartConfig();

  //Wait for SmartConfig packet from mobile
  Serial.println("Waiting for SmartConfig.");
  while (!WiFi.smartConfigDone()) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("SmartConfig received.");

  //Wait for WiFi to connect to AP
  Serial.println("Waiting for WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
}


String connectWifi()
{
    Serial.begin(115200);
    pinMode(7, OUTPUT);      // set the LED pin mode

    delay(10);

    // We start by connecting to a WiFi network

    WiFi.begin();
    WiFi.setTxPower(WIFI_POWER_8_5dBm);
    // Connected to network, if not connected after 5 attempts, start SmartConfig
    for(int i= 0;i < 5; i++) {
      if(WiFi.status() == WL_CONNECTED) break;
      delay(500);
      Serial.print('.');
    }
    if(WiFi.status() != WL_CONNECTED) {
      Serial.println("Starting SmartConfig - Use ESPTouch App to set Wifi Credentials");
      mySmartConfig();
    }
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    server.begin();
    return(WiFi.localIP().toString());
}

int value = 0;

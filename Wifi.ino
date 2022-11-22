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
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
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
  ArduinoOTA.handle();
}
void handleRoot() {
  Serial.println("handleRoot()");
  char tempstr[1024];

  sprintf(tempstr, "<!DOCTYPE html><html><head>\
                      <title>Rotary DRO</title></head><body>\
                      <style>\
                      table {\
  border-collapse: collapse;\
  width: 100%;\
  font-size: 15px;\
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
<input type=\"text\" name=\"diameter\" value=\"%f\">\
<input type=\"submit\" value=\"Submit\">\
    </form>\n</body></html>\n",diameter);
    server.send(200, "text/html", tempstr);
}

void handleForm() {
  Serial.println("handleForm()");
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
  } else 
  // server.send(200, "text/plain", "Form Handling");
    String message = "POST form was:\n";
    for (uint8_t i = 0; i < server.args(); i++) {
      String name = server.argName(i);
      if (name == "diameter") {
        Serial.print("Setting Diameter to: ");
        String val = server.arg(i);
        diameter = val.toFloat();
        Serial.println(diameter,6);
        String str = "Diameter set to: ";
        str = str + diameter;
        server.send(200, "text/plain", str);
        measureMode = 1;
      } else {
        server.send(405, "text/plain", "Unknown paramater");
      }
        
   }
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

  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  // ArduinoOTA.setHostname("myesp32");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();   

    
    return(WiFi.localIP().toString());
}

int value = 0;

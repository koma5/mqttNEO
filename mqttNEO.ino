#include <Ethernet.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <Adafruit_NeoPixel.h>

#include <EEPROM.h>
#include "EEPROMAnything.h"

struct config_led
{
    int brightness;
    uint32_t color;
} ledConfig;

byte mac[]    = {  0x90, 0xA2, 0xDA, 0x00, 0x5B, 0x26 };
IPAddress ip(172, 16, 42, 147);
IPAddress server(172, 16, 42, 70);

#define NEOPIN 6

Adafruit_NeoPixel strip = Adafruit_NeoPixel(40, NEOPIN, NEO_GRB + NEO_KHZ800);

unsigned long lastChange = 0;
long randomPixelToTurnOn, randomPixelToTurnOff;
int turnedOnPixels = 0;
int brightness;
uint32_t color;
String r,g,b;

void callback(char* topic, byte* payload, unsigned int length)
{
  
  payload[length] = '\0';
  String myPayload = (char *) payload;
  String myTopic =  topic;

  if(myTopic == "vw/neo/brightness")
  {
    int newBrightness = myPayload.toInt();
    if(newBrightness > -1 && newBrightness <= strip.numPixels())
    {
      ledConfig.brightness = newBrightness;
      EEPROM_writeAnything(0, ledConfig);
      brightness = newBrightness;
    }
  }
  else if (myTopic == "vw/neo/color")
  {

    int commaIndex = myPayload.indexOf(',');
    int secondCommaIndex = myPayload.indexOf(',', commaIndex+1);
    String r = myPayload.substring(0, commaIndex);
    String g = myPayload.substring(commaIndex+1, secondCommaIndex);
    String b = myPayload.substring(secondCommaIndex+1);

    uint32_t newColor = strip.Color(r.toInt(), g.toInt(), b.toInt());
    if (newColor != 0) {
        ledConfig.color = newColor;
        EEPROM_writeAnything(0, ledConfig);
        color = newColor;
    }
  }
}


EthernetClient ethClient;
PubSubClient client(ethClient);

void setup() {
  //Serial.begin(57600);

  EEPROM_readAnything(0, ledConfig);
  brightness = ledConfig.brightness;
  color = ledConfig.color;


  strip.begin();
  strip.show();
  
  randomSeed(analogRead(A0));

  client.setServer(server, 1883);
  client.setCallback(callback);

  Ethernet.begin(mac, ip);

  delay(1500);
}

void loop() {

  if(client.connected())
  {
      client.loop();   
  }
  else
  {
    client.connect("mqttNEO");
    delay(5000);
    client.subscribe("vw/neo/brightness");
    client.subscribe("vw/neo/color");
  }



  if (millis() < lastChange || millis() - lastChange >= 1000)
  {
    //needs more pixels turn one random pixel on wich is off
    if (turnedOnPixels < brightness) {
      //Serial.println("More");
      
      do {
        //Serial.println("more: get random pixel");
        randomPixelToTurnOn = random(strip.numPixels());
      } while(strip.getPixelColor(randomPixelToTurnOn) > 0);
      
      //Serial.println("more: set random pixel");
      strip.setPixelColor(randomPixelToTurnOn, color);
      turnedOnPixels++;
    }
  
    //needs less pixels turn one random pixel off wich is on
    else if (turnedOnPixels > brightness) {
      //Serial.println("less");
      
      do {
        //Serial.println("less get random pixel");
        randomPixelToTurnOff = random(strip.numPixels());
      } while(strip.getPixelColor(randomPixelToTurnOff) == 0);
      
      //Serial.println("less set random pixel");
      strip.setPixelColor(randomPixelToTurnOff, 0);
      turnedOnPixels--;
    }
  
    // brightness is good turn one random pixel off which is on and one on which is off
    else if (turnedOnPixels == brightness && turnedOnPixels != strip.numPixels() && brightness != 0) {
     //Serial.println("equal");
     
     do  {
      //Serial.println("equal get random pixel");
       randomPixelToTurnOn = random(strip.numPixels());
       randomPixelToTurnOff = random(strip.numPixels());
     } while(strip.getPixelColor(randomPixelToTurnOn) > 0 || strip.getPixelColor(randomPixelToTurnOff) == 0);
     
      //Serial.println("equal set random pixel");
      strip.setPixelColor(randomPixelToTurnOn, color);
      strip.setPixelColor(randomPixelToTurnOff, 0);
    }
  
    //change one if color changed
    else if (turnedOnPixels == brightness && turnedOnPixels == strip.numPixels()) { 
      //Serial.println("equal all on");
      randomPixelToTurnOn = random(strip.numPixels());
      //Serial  .println("equal set random pixel");
      strip.setPixelColor(randomPixelToTurnOn, color);
    }
    strip.show();
    lastChange = millis();
  }

}


#include <ESP8266WiFi.h>
#include <WiFiClient.h>
//#include <ESP8266WebServer.h>

#include <PubSubClient.h>
#include <EEPROM.h>

// Arduino pins for RGB Lamp control
// These pins are in order on huzzah for convinience
#define ONERED 13
#define ONEGREEN 12 
#define ONEBLUE 14                   
#define TWORED 15 
#define TWOGREEN 4
#define TWOBLUE 5                  

// Set the broker server IP
const byte server[] = { 192,168,1,5 };

const char* ssid     = "This one";
const char* password = "datlftp!";

void callback(char*, byte*, unsigned int); 

WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);

byte LampOneR;
byte LampOneG;
byte LampOneB;
byte LampTwoR;
byte LampTwoG;
byte LampTwoB;

// both lamp color persistance functions use the global variables
void writeRGBtoEEPROM(void)
{
  byte addr = 0;
  EEPROM.write(addr, LampOneR);
  addr += 1;
  EEPROM.write(addr, LampOneG);
  addr += 1;
  EEPROM.write(addr, LampOneB);
  addr += 1;
  EEPROM.write(addr, LampTwoR);
  addr += 1;
  EEPROM.write(addr, LampTwoG);
  addr += 1;
  EEPROM.write(addr, LampTwoB);
  addr += 1;

  EEPROM.commit(); // ESP8266 requires this call before values are stored
  
}
void readRGBfromEEPROM(void)
{
  byte addr = 0;
  LampOneR = EEPROM.read(addr);
  addr += 1;
  LampOneG = EEPROM.read(addr);
  addr += 1;
  LampOneB = EEPROM.read(addr);
  addr += 1;
  LampTwoR = EEPROM.read(addr);
  addr += 1;
  LampTwoG = EEPROM.read(addr);
  addr += 1;
  LampTwoB = EEPROM.read(addr);
  addr += 1;
}


void setup()
{
  pinMode(ONERED, OUTPUT);
  pinMode(ONEGREEN, OUTPUT);
  pinMode(ONEBLUE, OUTPUT);
  pinMode(TWORED, OUTPUT);
  pinMode(TWOGREEN, OUTPUT);
  pinMode(TWOBLUE, OUTPUT);

  EEPROM.begin(512);

  // get the previous colors of the lamps
  readRGBfromEEPROM();

  // write the read values to the lamps
  analogWrite(ONERED, LampOneR);
  analogWrite(ONEGREEN, LampOneG);
  analogWrite(ONEBLUE, LampOneB);
  analogWrite(TWORED, LampTwoR);
  analogWrite(TWOGREEN, LampTwoG);
  analogWrite(TWOBLUE, LampTwoB); 

  // Open serial communications
  Serial.begin(115200);
/*  
   Serial.print("Lamp One Red = ");
   Serial.println(LampOneR);
   Serial.print("Lamp One Green = ");
   Serial.println(LampOneG);
   Serial.print("Lamp One Blue = ");
   Serial.println(LampOneB);
   Serial.print("Lamp Two Red = ");
   Serial.println(LampTwoR);
   Serial.print("Lamp Two Green = ");
   Serial.println(LampTwoG);
   Serial.print("Lamp Two Blue = ");
   Serial.println(LampTwoB);
*/

  // Connect to WiFi network
  WiFi.begin(ssid, password);
  Serial.print("\n\r \n\rWorking to connect");
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("RGB Lamp control");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
   
  // Connect to Broker, give it arduino as the name
  if (client.connect("arduino_Chandelier")) 
  {  
    Serial.println("Connected to MQTT client");
    // Publish a message to the status topic
    client.publish("status/arduino_Chandelier","Arduino Chandelier is now online");
    
    // Listen for messages on the control topic
    client.subscribe("control/arduino_Chandelier/#");
   }
/*
   digitalWrite(ONERED, HIGH);
   delay(500);
   digitalWrite(ONERED, LOW);
   digitalWrite(ONEGREEN, HIGH);
   delay(500);
   digitalWrite(ONEGREEN, LOW);
   digitalWrite(ONEBLUE, HIGH);
   delay(500);
   digitalWrite(ONEBLUE, LOW);

   digitalWrite(TWORED, HIGH);
   delay(500);
   digitalWrite(TWORED, LOW);
   digitalWrite(TWOGREEN, HIGH);
   delay(500);
   digitalWrite(TWOGREEN, LOW);
   digitalWrite(TWOBLUE, HIGH);
   delay(500);
   digitalWrite(TWOBLUE, LOW);
*/


}

void loop()
{
  client.loop();
}

void callback(char* topic, byte* payload, unsigned int length) 
{
  // check for messages on subscribed topics
  payload[length] = '\0';
  Serial.print("Topic: ");
  Serial.println(String(topic));
  
  // check topic to identify type of content
  if(String(topic) == "control/arduino_Chandelier/entry/lamp1/color") 
  {
    // convert payload to String
    String value = String((char*)payload);
    //value.trim();
    Serial.println (value);
    //Serial.flush();
    // split string at every "," and store in proper variable
    // convert final result to integer
    LampOneR = value.substring(0,value.indexOf(';')).toInt();
    LampOneG = value.substring(value.indexOf(';')+1,value.lastIndexOf(';')).toInt();
    LampOneB = value.substring(value.lastIndexOf(';')+1).toInt();
/*
    // Openhab outputs RGB from 1 to 100 so convert to 0 to 255
    if(LampOneR == 1)
      LampOneR = 0;
    else
      LampOneR = LampOneR * 2.55;
    if(LampOneG == 1)
      LampOneG = 0;
    else
      LampOneG = LampOneG * 2.55;
    if(LampOneB == 1)
      LampOneB = 0;
    else
      LampOneB = LampOneB * 2.55;
*/
    // print obtained values for debugging
    Serial.print("Lamp 1 RED: ");
    Serial.println(LampOneR);
    //client.publish("status/arduino_LED", SoffitR);
    
    Serial.print("Lamp 1 GREEN: ");
    Serial.println(LampOneG);
    //client.publish("status/arduino_LED", SoffitG);
    
    Serial.print("Lamp 1 BLUE: ");
    Serial.println(LampOneB);
    //client.publish("status/arduino_LED/soffit/color/blue", int SoffitB);
    //Serial.flush();

    // save these colors so they will be the same after power off
    writeRGBtoEEPROM();
       
    analogWrite(ONERED, LampOneR);
    analogWrite(ONEGREEN, LampOneG);
    analogWrite(ONEBLUE, LampOneB);
    
//    while(Serial.available())
//      Serial.read();
    
  }
  else if(String(topic) == "control/arduino_Chandelier/entry/lamp2/color") 
  {
    // convert payload to String
    String value = String((char*)payload);
    //value.trim();
    Serial.println (value);
    //Serial.flush();
    // split string at every "," and store in proper variable
    // convert final result to integer
    LampTwoR = value.substring(0,value.indexOf(';')).toInt();
    LampTwoG = value.substring(value.indexOf(';')+1,value.lastIndexOf(';')).toInt();
    LampTwoB = value.substring(value.lastIndexOf(';')+1).toInt();
/*
    // Openhab outputs RGB from 1 to 100 so convert to 0 to 255
    if(LampTwoR == 1)
      LampTwoR = 0;
    else
      LampTwoR = LampTwoR * 2.55;
    if(LampTwoG == 1)
      LampTwoG = 0;
    else
      LampTwoG = LampTwoG * 2.55;
    if(LampTwoB == 1)
      LampTwoB = 0;
    else
      LampTwoB = LampTwoB * 2.55;
*/
    // print obtained values for debugging
    Serial.print("Lamp 2 RED: ");
    Serial.println(LampTwoR);
    //client.publish("status/arduino_LED", SoffitR);
    
    Serial.print("Lamp 2 GREEN: ");
    Serial.println(LampTwoG);
    //client.publish("status/arduino_LED", SoffitG);
    
    Serial.print("Lamp 2 BLUE: ");
    Serial.println(LampTwoB);
    //client.publish("status/arduino_LED/soffit/color/blue", int SoffitB);
    //Serial.flush();

    // save these colors so they will be the same after power off
    writeRGBtoEEPROM();
       
    analogWrite(TWORED, LampTwoR);
    analogWrite(TWOGREEN, LampTwoG);
    analogWrite(TWOBLUE, LampTwoB);
    
//    while(Serial.available())
//      Serial.read();
    
  }

}

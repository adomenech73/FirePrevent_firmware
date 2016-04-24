#include <ESP8266WiFi.h> //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

#include <DHT.h>                  // DHT11 & DHT22 Temperature and humidity sensor
#include <Wire.h>                 // 
#include <WiFiUdp.h>              // Wifi UDP packets generation
#include <max6675.h>
#include <LiquidCrystal_I2C.h>    // I2c LCD display

/*-----( Declare Constants & objects)-----*/
int thermoDO = 14; // D5
int thermoCS1 = 12; // D6
int thermoCLK = 13; // D7
int thermoCS2 = 16; // D0

MAX6675 thermocouple1(thermoCLK, thermoCS1, thermoDO);
MAX6675 thermocouple2(thermoCLK, thermoCS2, thermoDO);

// make a cute degree symbol
//uint8_t degree[8]  = {140,146,146,140,128,128,128,128};

// objects to store 
uint8_t MAC_array[6];
char MAC_char[18];

// InfluxDB server ip
byte host[] = {192, 168, 1, 101}; 
// the port that the InfluxDB UDP plugin is listening on
int port = 8888;

#define DHTPIN D4 // what pin we connect DHT to
// note GPIO5 is D1 at NodeMCU - http://www.14core.com/wp-content/uploads/2015/06/Node-MCU-Pin-Out-Diagram1.png
// note GPIO5 is D15 http://www.wemos.cc/wiki/doku.php?id=en:d1
//#define DHTTYPE DHT11   // DHT 11 
#define DHTTYPE DHT22   // DHT 22 
DHT dht(DHTPIN, DHTTYPE);

WiFiUDP udp;


// set the LCD address to 0x27 for a 20 chars 4 line display
// Set the LCD I2C address

LiquidCrystal_I2C lcd(0x27, 20, 4);

// Callback definition for WiFiManager
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
}


void setup() {
  Serial.begin(115200);
  Serial.println("Wemos sensors sketch");

  WiFiManager wifiManager;

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if(!wifiManager.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }
  
  // Devices initialitation

  lcd.begin();
  lcd.clear();
  lcd.leftToRight();
  lcd.home();

  dht.begin();
  
  // create MAC string
  WiFi.macAddress(MAC_array);
  for (int i = 0; i < sizeof(MAC_array); ++i){
    sprintf(MAC_char,"%s%02x:",MAC_char,MAC_array[i]);
  }

  Serial.print("MAC @:");
  Serial.println(MAC_char);

  Serial.println("");
  Serial.println("");

  //lcd.createChar(0, degree);
  delay(3000); 
}
    
void loop() {

  // Get sensors data
  float f_dhtemp = dht.readTemperature();
  float f_humidity = dht.readHumidity();
  float f_therm_c1 = thermocouple1.readCelsius();
  float f_therm_f1 = thermocouple1.readFahrenheit();
  float f_therm_c2 = thermocouple2.readCelsius();
  float f_therm_f2 = thermocouple2.readFahrenheit();

  // Generating line protocol
  String temperature = String("dhtemp,host=") \
    + MAC_char + String(" value=") + String(f_dhtemp, 2) + "\n";
  String humidity = String("humidity,host=") \
    + MAC_char + String(" value=") + String(f_humidity,2) + "\n";
  String thermc1 = String("therm_C,host=") \
    + MAC_char + String(" value=") + String(f_therm_c1,2) + "\n";
  String thermf1 = String("therm_F,host=") \
    + MAC_char + String(" value=") + String(f_therm_f1,2) + "\n";
  String thermc2 = String("therm_C,host=") \
    + MAC_char + String(" value=") + String(f_therm_c2,2) + "\n";
  String thermf2 = String("therm_F,host=") \
    + MAC_char + String(" value=") + String(f_therm_f2,2) + "\n";    

  // Print serial output (format validation purpose)
  Serial.println();
  Serial.println("SENSORS VALUE:");
  Serial.print(temperature);
  Serial.print(humidity);
  Serial.print(thermc1);
  Serial.print(thermf1);
  Serial.print(thermc2);
  Serial.print(thermf2);

  // Send UDP packet
  Serial.println();
  Serial.println("Preparing UDP packet...");
  udp.beginPacket(host, port);
  udp.print(temperature);
  udp.print(humidity);
  udp.print(thermc1);
  udp.print(thermf1);
  udp.print(thermc2);
  udp.print(thermf2);
  udp.endPacket(); 
  Serial.println("UDP packet sent...");
  Serial.println();
  Serial.println("Waiting...");

  // Print LCD output
  lcd.setCursor(0x00,0);
  lcd.print("tmp1:");
  lcd.print(f_dhtemp);
  lcd.print(" hum:");
  lcd.print(f_humidity);
  
  lcd.setCursor(0x14,0);
  lcd.print("1C:");
  lcd.print(f_therm_c1);
  lcd.print(" 1F:");
  lcd.print(f_therm_f1);

  lcd.setCursor(0x54,0);
  lcd.print("2C:");
  lcd.print(f_therm_c2);
  lcd.print(" 2F:");
  lcd.print(f_therm_f2);  

/*  lcd.print(thermocouple.readCelsius());
#if ARDUINO >= 100
  lcd.write((byte)0);
#else
  lcd.print(0, BYTE);
#endif
  lcd.print("C ");
  lcd.print(thermocouple.readFahrenheit());
#if ARDUINO >= 100
  lcd.write((byte)0);
#else
  lcd.print(0, BYTE);
#endif
  lcd.print('F');*/

  delay(3000);  
}
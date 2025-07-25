#include <WiFi.h>
#include "ThingSpeak.h" 
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"

#define DHTPIN 4     // Pin ESP yang tersambung ke pin Data DHT22 (pin 2)
#define DHTTYPE DHT22   // jenis sensor DHT 22  (AM2302), AM2321

DHT dht(DHTPIN, DHTTYPE);

float hum = 0; //variabel untuk menyimpan nilai kelembaban 
float temp = 0; // variabel untuk menyimpan nilai suhu
float hic = 0; // heat index dalam celcius

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

char ssid[] = "Wokwi-GUEST";   // Wokwi Wifi default
char pass[] = "";   //Blank Password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = 3015622;
const char * myWriteAPIKey = "6DVGJX7UGAOT9N7K";

unsigned long previousMillis = 0;  
const long ts_update_interval = 20000;  //update data setiap 20s

// 'thermometer', 55x55px
const unsigned char thermo [] PROGMEM = {
  0x00, 0x00, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 
  0x07, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x07, 0x83, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x01, 
  0xe0, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0xe0, 0x00, 
  0x00, 0x00, 0x00, 0x0e, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x1f, 0xe0, 0x00, 0x00, 0x00, 
  0x00, 0x0c, 0x1f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x1f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x0c, 
  0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x1f, 0xe0, 
  0x00, 0x00, 0x00, 0x00, 0x0c, 0x1f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x1f, 0xe0, 0x00, 0x00, 
  0x00, 0x00, 0x0c, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 
  0x0c, 0x1f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x1f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x1f, 
  0xe0, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x38, 0x60, 0x00, 
  0x00, 0x00, 0x00, 0x0c, 0x7c, 0x60, 0x00, 0x00, 0x00, 0x00, 0x0c, 0xfe, 0x60, 0x00, 0x00, 0x00, 
  0x00, 0x0c, 0xfe, 0x60, 0x00, 0x00, 0x00, 0x00, 0x0c, 0xfe, 0x60, 0x00, 0x00, 0x00, 0x00, 0x0c, 
  0xfe, 0x60, 0x00, 0x00, 0x00, 0x00, 0x0c, 0xfe, 0x60, 0x00, 0x00, 0x00, 0x00, 0x0c, 0xfe, 0x60, 
  0x00, 0x00, 0x00, 0x00, 0x0c, 0xfe, 0x60, 0x00, 0x00, 0x00, 0x00, 0x0c, 0xfe, 0x60, 0x00, 0x00, 
  0x00, 0x00, 0x0c, 0xfe, 0x60, 0x00, 0x00, 0x00, 0x00, 0x1c, 0xfe, 0x70, 0x00, 0x00, 0x00, 0x00, 
  0x3c, 0xfe, 0x78, 0x00, 0x00, 0x00, 0x00, 0x7c, 0xfe, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 
  0xbc, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0xf7, 0xff, 0xde, 0x00, 
  0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xef, 0xff, 0xef, 0x00, 0x00, 0x00, 
  0x00, 0xef, 0xff, 0xef, 0x00, 0x00, 0x00, 0x00, 0xef, 0xff, 0xef, 0x00, 0x00, 0x00, 0x00, 0xef, 
  0xff, 0xef, 0x00, 0x00, 0x00, 0x00, 0xef, 0xff, 0xef, 0x00, 0x00, 0x00, 0x00, 0xef, 0xff, 0xef, 
  0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xf7, 0xff, 0xde, 0x00, 0x00, 
  0x00, 0x00, 0xfb, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x7d, 0xff, 0x3c, 0x00, 0x00, 0x00, 0x00, 
  0x7e, 0xfe, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x01, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 
  0xf0, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0x00, 0x00, 
  0x00
};

// 'wifi connected', 16x16px
const unsigned char wifi_icon [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x1f, 0xf8, 0x7c, 0x3e, 0xe7, 0xe7, 0xdf, 0xfb, 0x39, 0x1c, 
  0x37, 0xec, 0x0f, 0xf0, 0x0f, 0xf0, 0x03, 0xc0, 0x03, 0xc0, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00
};

// 'wifi disconnected', 16x16px
const unsigned char wifi_dc [] PROGMEM = {
	0x00, 0x00, 0x00, 0x04, 0x03, 0xce, 0x1f, 0xdc, 0x7f, 0xba, 0xf8, 0x77, 0x60, 0xe6, 0x05, 0xc0, 
	0x0b, 0xb0, 0x07, 0x70, 0x0e, 0x20, 0x1c, 0x00, 0x1b, 0x80, 0x03, 0xc0, 0x01, 0x80, 0x00, 0x00
};

const unsigned char Update_OK [] PROGMEM = {
	0x01, 0x80, 0x0f, 0xf2, 0x10, 0x18, 0x20, 0x0c, 0x40, 0x1e, 0x40, 0x32, 0x40, 0x62, 0x84, 0x63, 
	0x83, 0xc3, 0x41, 0x82, 0x40, 0x02, 0x40, 0x06, 0x20, 0x04, 0x10, 0x08, 0x0e, 0x70, 0x01, 0x80
};

const unsigned char Update_NOK [] PROGMEM = {
	0x01, 0x80, 0x0e, 0x70, 0x10, 0x08, 0x20, 0x04, 0x48, 0x32, 0x44, 0x62, 0xc2, 0xc2, 0x81, 0x82, 
	0x83, 0x82, 0xc6, 0x42, 0x4c, 0x22, 0x48, 0x16, 0x20, 0x04, 0x10, 0x18, 0x0e, 0xf0, 0x01, 0x80
};

void setup() {
  Serial.begin(9600);
  Serial.println(F("Temp & Humid Monitoring System"));
  dht.begin();

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); 
  }
  
  // just intro
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(38,20);  display.println(F("ThinkSpeak"));
  display.setCursor(25,35);  display.println(F("Temp-Hum Monitor"));
  display.display(); //tampilkan data
  delay(3000);
  WiFi.mode(WIFI_STA);   
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  ConnectWIFI();
}

void loop() {
  hum = dht.readHumidity();
  temp = dht.readTemperature(); // membaca suhu dalam Celsius

  // temp = dht.readTemperature(true);  //suhu dalam Fahrenheit 

  // Check if any reads failed and exit early (to try again).
  if (isnan(hum) || isnan(temp)) {
    Serial.println(F("Gagal mendapat data dari sensor DHT!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  // float hif = dht.computeHeatIndex(f, h);

  // Compute heat index in Celsius (isFahreheit = false)
  hic = dht.computeHeatIndex(temp, hum, false);
  //Serial.print(F("Kelembaban: "));Serial.print(hum); Serial.print(F("%  "));
  //Serial.print(F("Suhu: "));  Serial.print(temp);   Serial.print(F("°C  "));
  //Serial.print(F("Heat index: ")); Serial.print(hic); Serial.println(F("°C "));
  //Serial.print(F("Heat index: ")); Serial.print(hif); Serial.println(F("°F "));
  
  display.clearDisplay();
  display.drawBitmap(0, 5, thermo, 55, 55, WHITE);
  display.setTextSize(1);
  display.setCursor(55,25);  display.print(temp); display.print(" C");
  display.setCursor(55,40);  display.print(hum);  display.print(" %");

  if(WiFi.status() != WL_CONNECTED){
    display.drawBitmap(110, 0, wifi_dc, 16, 16, WHITE);
  }else{
    display.drawBitmap(110, 0, wifi_icon, 16, 16, WHITE);
  }


  unsigned long currentMillis = millis(); 
  if (currentMillis - previousMillis >= ts_update_interval) { 
    previousMillis = currentMillis;
    if(WiFi.status() != WL_CONNECTED){
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);
        WiFi.begin(ssid, pass);
        int i=0;
        while (WiFi.status() != WL_CONNECTED) {
          delay(500);
          Serial.print(".");
          ++i;

          if (i==20){
            i=0;
            Serial.println("\n Failed to Connect.");
            break;
          } 
        }
    }

    ThingSpeak.setField(1, hum);  //field humidity
    ThingSpeak.setField(2, temp); //field temperature 
    ThingSpeak.setField(3, hic);  //field heat_index
    
    // write to the ThingSpeak channel
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    if(x == 200){
      Serial.println("Sukses Update Chanel.");
      display.drawBitmap(90, 0, Update_OK, 16, 16, WHITE);
    }else{
      Serial.println("Update Chanel Bermasalah. HTTP error code " + String(x));
      display.drawBitmap(90, 0, Update_NOK, 16, 16, WHITE);
    }
  }

  display.display();
  delay(2000);
}


void ConnectWIFI(){
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    //WiFi.mode(WIFI_STA);  
    WiFi.begin(ssid, pass); //Untuk Simulasi >> Spesifik channel 6 
    int i=0;
    int a=0;
    while(WiFi.status() != WL_CONNECTED){ 
      Serial.print(".");
      display.clearDisplay(); 
      display.setTextSize(1);
      if (a==0){
        display.drawBitmap(52, 15, wifi_icon, 16, 16, WHITE);
        a=1;
        
      }else{
        display.drawBitmap(52, 15, wifi_icon, 16, 16, BLACK);
        a=0;
      }
      display.setCursor(25,38);display.print("Connecting ...");
      display.display();
      delay(1000); 
      ++i;
      if (i==30){
        i=0;
        Serial.println("\n Failed to Connect.");
        break;
      }    
    }

    if(WiFi.status() == WL_CONNECTED){
      Serial.println("\n Connected!"); 
      display.clearDisplay(); 
      display.setTextSize(1);
      display.drawBitmap(52, 15, wifi_icon, 16, 16, WHITE);
      display.setCursor(33,38);display.print("Connected!");
      display.display();
    }else{
      display.clearDisplay(); 
      display.setTextSize(1);
      display.drawBitmap(52, 15, wifi_dc, 16, 16, WHITE);
      display.setCursor(20,38);display.print("Wifi Connection");
      display.setCursor(36,48);display.print("Failed!");
      display.display();
    }
    delay(3000);
  }
}

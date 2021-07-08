#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include<Wire.h>
#include "MAX30100_PulseOximeter.h"
#define MAX_READING_TIME 5000
#define REPORTING_PERIOD_MS 500
#define TIME_PER_READING 15000
PulseOximeter pox;

// include the library code:
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = D0, en = D3, d4 = D4, d5 = D5, d6 = D6, d7 = D7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
String readString;
const char* ssid = "Sriguru";
const char* password = "SRIGURU2000";
const char* host = "oxydata2sql.herokuapp.com";
const int httpsPort = 443;
WiFiClientSecure client;
const char* fingerprint = "46 B2 C3 44 9C 59 09 8B 01 B6 F8 BD 4C FB 00 74 91 2F EF F6";

uint32_t tsLastReport = 0;
static int i=1,j=0,k=0;
static int bpm=0,SpO2=0,oldbpm=0,oldSpO2=0,sumBPM=0,sumSpO2=0;
float avgBPM,avgSpO2;
static int sensorValues[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
void(* resetFunc) (void) = 0;//declare reset function at address 0
void onBeatDetected()
{
 Serial.println("BEAT");
}
void sendData(int SpO2, int bpm)
{
 client.setInsecure();
 Serial.print("connecting to ");
 Serial.println(host);
 if (!client.connect(host, httpsPort)) {
   Serial.println("connection failed");
   resetFunc();
   return;
  }
 String string_SpO2     =  String(SpO2, DEC);
 String string_bpm     =  String(bpm, DEC);
 String url ="/" + string_SpO2 + "/" + string_bpm;
 Serial.print("requesting URL: ");
 Serial.println(url);
 client.print(String("GET ") +url+ " HTTP/1.1\r\n" +
        "Host: " + host + "\r\n" +
        "User-Agent: BuildFailureDetectorESP8266\r\n" +
        "Connection: close\r\n\r\n");
 Serial.println("request sent");
 while (client.connected()) {
 String line = client.readStringUntil('\n');
 if (line == "\r") {
   Serial.println("headers received");
   break;
 }
 }
 String line = client.readStringUntil('\n');
 Serial.println("reply was:");
 Serial.println("==========");
  Serial.println(line);
lcd.clear();
lcd.setCursor(0,0);
lcd.print("-Sent to SQL DB-");
lcd.setCursor(0,1);
lcd.print(line);

 Serial.println(line);
 Serial.println("==========");
 Serial.println("closing connection");
}

void placeFingerLCD(){
    lcd.setCursor(0,0);
    lcd.print(" Place your ");
    lcd.setCursor(0,1);
    lcd.print("<--finger here");
}


void setup()
{
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);  
  placeFingerLCD();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  
  Serial.begin(9600);
  Serial.print("Initializing pulse oximeter..");
    // Initialize the PulseOximeter instance
    // Failures are generally due to an improper I2C wiring, missing power supply
    // or wrong target chip
    if(!pox.begin()){
      Serial.println("FAILED");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Sensor FAILED");
      lcd.setCursor(0,1);
      lcd.print("Try Rebooting...");
      exit(0);
    }
    else
    {
      Serial.println("SUCCESS");
    }
    pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
    // Register a callback for the beat detection
    pox.setOnBeatDetectedCallback(onBeatDetected);
}
void loop()
{
  pox.update();
  bpm=pox.getHeartRate();
  SpO2=pox.getSpO2();
      if(SpO2>65 && bpm >45)
      {
        if(i%5==0&&k==0){
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("....Updating....");
        }

        i++;
        if(oldSpO2!=SpO2 || oldbpm!=bpm)
        {
            if(j<10){
            sensorValues[j] = SpO2;
            sensorValues[j+10] = bpm;
            Serial.print(j);
            Serial.print(" ");
            Serial.print(SpO2);
            Serial.print(" ");
            Serial.println(bpm);
            j++;
            }
            else if(j==10)
            {
              Serial.print("  ARRAY: ");
              for(int i=0;i<20;i++){
                Serial.print(sensorValues[i]);
                Serial.print(" ");
              }
              Serial.println("");
              
              for(int i=0;i<10;i++)
              {
                sumSpO2+=sensorValues[i];
                sumBPM+=sensorValues[i+10];
              }
              avgSpO2=sumSpO2/10;
              avgBPM = sumBPM/10;
              sumSpO2=0;
              sumBPM=0;             
              Serial.print("DATA-> bpm: ");
              Serial.print(int(avgBPM));
              Serial.print("; SpO2: ");
              Serial.println(int(avgSpO2));
              //sendData(int(avgSpO2),int(avgBPM));
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.noBlink();
              k=1;
              lcd.print(" --- RESULTS--- ");
              lcd.setCursor(0,1);
              lcd.print("SPO2:");
              lcd.print(int(avgSpO2));
              lcd.print(" ");
              lcd.print("BPM:");
              lcd.print(int(avgBPM));
              sendData(int(avgSpO2),int(avgBPM));
              delay(1000);
              resetFunc();
              j=0;
              sensorValues[j] = SpO2;
              sensorValues[j+10] = bpm;
              j++;              
            }
            oldbpm=bpm;
            oldSpO2=SpO2;   
        }
        tsLastReport = millis();
      }
      if(millis()-tsLastReport>MAX_READING_TIME){
        placeFingerLCD();
        bpm=0,SpO2=0,oldbpm=0,oldSpO2=0,sumBPM=0,sumSpO2=0,i=0,j=0,k=0;
        avgBPM=0,avgSpO2=0;
      }
}

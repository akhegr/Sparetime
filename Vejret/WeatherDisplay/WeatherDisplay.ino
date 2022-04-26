// ESP32 with GxEPD and data from WiFi
// Created by Aksel Heidemann Gregersen

/***  GxEPD  Includes         ***/
// Based on ESP32 part of GxEPD_Example by Jean-Marc Zingg for e-Paper displays
// mapping suggestion for ESP32, e.g. LOLIN32, see .../variants/.../pins_arduino.h for your board
// NOTE: there are variants with different pins for SPI ! CHECK SPI PINS OF YOUR BOARD
// BUSY -> 4, RST -> 16, DC -> 17, CS -> SS(5), CLK -> SCK(18), DIN -> MOSI(23), GND -> GND, 3.3V -> 3.3V

// include library, include base class, make path known
#include <GxEPD.h>

// select the display class to use, only one
#include <GxGDEW042T2/GxGDEW042T2.h>      // 4.2" b/w

#include GxEPD_BitmapExamples
#include "Images.h"

// FreeFonts from Adafruit_GFX
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>

#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>

//Watchdog for low power energy mode
#include <Adafruit_SleepyDog.h>

#if defined(ESP32)

GxIO_Class io(SPI, /*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16); // arbitrary selection of 17, 16
GxEPD_Class display(io, /*RST=*/ 16, /*BUSY=*/ 4); // arbitrary selection of (16), 4

#endif
/***  End GxEPD  Includes     ***/

/***  WiFi  Includes         ***/
#include <ArduinoJson.h>
#include <SPI.h>
#include <WiFi.h>

const char* ssid     = "TN-LX2040";
const char* password = "8ombackHivra";
const char* host = "api.akselhg.dk";
const char* lastResult = "";

WiFiClient client;
/***  End WiFi  Includes     ***/

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");

  display.init(115200); // enable diagnostic output on Serial

  Serial.println("setup done");

  display.fillScreen(GxEPD_WHITE);
  
  showFont("Vejrstation", &FreeMonoBold24pt7b, 50, 50);
  showFont("Aksel H. Gregersen", &FreeMonoBold9pt7b, 110, 110);
  display.drawBitmap(gImage_WEATHER, 20, 80, 50, 50, GxEPD_WHITE); 
  display.drawBitmap(gImage_WEATHER, 330, 80, 50, 50, GxEPD_WHITE); 
  
  showFont("Tidspunkt:",      &FreeMonoBold12pt7b, 15, 170);
  showFont("          ",      &FreeMonoBold12pt7b, 250, 170);
  
  showFont("Nedboer:",        &FreeMonoBold12pt7b, 15, 200);
  showFont("          ",      &FreeMonoBold12pt7b, 250, 200);
  
  showFont("Vindretning:",    &FreeMonoBold12pt7b, 15, 230);
  showFont("          ",      &FreeMonoBold12pt7b, 250, 230);
  
  showFont("Vindhastighed:",  &FreeMonoBold12pt7b, 15, 260);
  showFont("          ",      &FreeMonoBold12pt7b, 250, 260);
  
  showFont("Tryk:",           &FreeMonoBold12pt7b, 15, 290);
  showFont("          ",      &FreeMonoBold12pt7b, 250, 290);
  
  display.update();

  WiFi.disconnect(true);
  WiFi.begin(ssid, password);

  Serial.print("Loading ");
  while (WiFi.status() != WL_CONNECTED)
  {
      delay(500);
      Serial.print(".");
  }
  Serial.println();
}

void loop()
{
  getData();
  Watchdog.sleep(600000);
}

void showFont(const char text[], const GFXfont* font, int x, int y)
{
  display.setCursor(x, y);
  display.setTextColor(GxEPD_BLACK, GxEPD_WHITE);
  display.setFont(font);
  display.println(text);
}

void getData()
{
    const int httpPort = 80;
    if (!client.connect(host, httpPort))
    {
        Serial.println("connection failed");
        return;
    }

    // We now create a URI for the request
    String url = "/weather/readOneWeather.php";

    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "User-Agent: ArduinoWiFi/1.1\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0)
    {
        if (millis() - timeout > 5000)
        {
            Serial.println(">>> Client Timeout !");
            client.stop();
            return;
        }
    }

    // Read all the lines of the reply from server and print them to Serial
    const char* result = "";
    bool startRead = false;
    while(client.available())
    {
      String line = client.readStringUntil('\r');
      line.trim();
      if(line.startsWith("{"))
      {
        startRead = true;
      }

      if(startRead)
      {
        char buf[200];
        char* lineBuffer = string2char(line);

        strcpy(buf, result);
        strcat(buf, lineBuffer);
        result = buf; 
      }
    }

    if(result != lastResult)
    {
      lastResult = result;
      parseData();
    }
	
	client.stop();
}

void parseData()
{
  char result[200];
  strcpy(result, lastResult);
  
  const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60;
  DynamicJsonDocument doc(capacity);

  // Parse JSON object
  DeserializationError error = deserializeJson(doc, result);
  if (error)
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    client.stop();
    return;
  }

  // Extract values
  const char* unitMillimeter =          "mm";
  const char* unitMeterPerSecond =      "m/s";
  const char* unitPascal =              "Pa";
  
  const char* datetime =                doc["datetime"].as<const char*>();
  const char* rainAmount =              doc["rainAmount"].as<const char*>();
  const char* windDir =                 doc["windDir"].as<const char*>();
  const char* windSpeed =               doc["windSpeed"].as<const char*>();
  const char* pressure =                doc["pressure"].as<const char*>();
  const char* clockTime =               lastLetters(datetime, 8);
  
  String rainString =                   getDataWithUnit(rainAmount, unitMillimeter);
  String windString =                   getDataWithUnit(windSpeed, unitMeterPerSecond);
  String pressureString =               getDataWithUnit(pressure, unitPascal);

  showFont("          ",      &FreeMonoBold12pt7b, 250, 170);
  showFont(clockTime,         &FreeMonoBold12pt7b, 250, 170);
  
  showFont("          ",      &FreeMonoBold12pt7b, 250, 200);
  showFont(string2char(rainString),     &FreeMonoBold12pt7b, 250, 200);
  
  showFont("          ",      &FreeMonoBold12pt7b, 250, 230);
  showFont(getDirection(windDir),       &FreeMonoBold12pt7b, 250, 230);
  
  showFont("          ",      &FreeMonoBold12pt7b, 250, 260);
  showFont(string2char(windString),     &FreeMonoBold12pt7b, 250, 260);
  
  showFont("          ",      &FreeMonoBold12pt7b, 250, 290);
  showFont(string2char(pressureString), &FreeMonoBold12pt7b, 250, 290);
  display.update();
}

char* string2char(String command)
{
    if(command.length()!=0)
    {
        char* p = const_cast<char*>(command.c_str());
        return p;
    }
}

const char* lastLetters(const char* str, int amount)
{
  int len = strlen(str);
  return len > 0 ? str + len - amount : str;
}

const char* getDirection(const char* str)
{
  if(strcmp(str, "N") == 0) return "Nord";
  else if(strcmp(str, "NE") == 0) return "Nordoest";
  else if(strcmp(str, "E") == 0) return "Oest";
  else if(strcmp(str, "SE") == 0) return "Sydoest";
  else if(strcmp(str, "S") == 0) return "Syd";
  else if(strcmp(str, "SW") == 0) return "Sydvest";
  else if(strcmp(str, "W") == 0) return "Vest";
  else if(strcmp(str, "NW") == 0) return "Nordvest";
  return "?";
}

String getDataWithUnit(const char* inputText, const char* inputUnit)
{
  String value = "";

  value.concat(inputText);
  value.concat(" ");
  value.concat(inputUnit);
  
  return value;
}

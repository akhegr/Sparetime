#include <ETH.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiClient.h>
#include <WiFiGeneric.h>
#include <WiFiMulti.h>
#include <WiFiScan.h>
#include <WiFiServer.h>
#include <WiFiSTA.h>
#include <WiFiType.h>
#include <WiFiUdp.h>

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

#define mS_TO_S_FACTOR 100000  /* Conversion factor for milliseconds to seconds */
#define TIME_TO_SLEEP  900        /* Time ESP32 will go to sleep (in seconds) */

const char* ssid     = "yourWifi";
const char* password = "yourPassword";
const char* host = "yourWebsite.com";
const char* lastResult = "";
const char* result = "";
RTC_DATA_ATTR char lastId[25];
RTC_DATA_ATTR int bootCount = 0;
bool loadState = false;

WiFiClient client;
/***  End WiFi  Includes     ***/

void setup()
{
  Serial.begin(115200);
  display.init(115200); // enable diagnostic output on Serial
  Serial.println("setup");
  
  //Increment boot number and print it on reboot
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  //Print the wakeup reason for ESP32
  print_wakeup_reason();

  //Configure the wake up source
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * mS_TO_S_FACTOR);
  Serial.println("ESP32 sleeps for " + String(TIME_TO_SLEEP) + " seconds");

  //Get WiFi connection
  initWiFi();  

  //Get and parse data
  getData();
  parseData();
}

void sleep(bool error)
{
  if(error)
  {
    initHeader();
    memset(lastId, 0, sizeof(lastId));
    display.update();
  }
  
  Serial.println("Sleeping");
  delay(5000);
  Serial.flush(); 
  esp_deep_sleep_start();
}

void loop() { }

/*
  Method to print the reason by which ESP32
  has been awaken from sleep
*/
void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0:
      Serial.println("Wakeup caused by external signal using RTC_IO");
      break;
    case ESP_SLEEP_WAKEUP_EXT1:
      Serial.println("Wakeup caused by external signal using RTC_CNTL");
      break;
    case ESP_SLEEP_WAKEUP_TIMER:
      Serial.println("Wakeup caused by timer");
      break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD:
      Serial.println("Wakeup caused by touchpad");
      break;
    case ESP_SLEEP_WAKEUP_ULP:
      Serial.println("Wakeup caused by ULP program");
      break;
    default:
      Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
      break;
  }
}

/*
 * Set WiFi ready to use
 */
void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
  loadState = true;
  Serial.println("Connected to AP successfully!");
}

/*
 * Set WiFi ready to use, and tell its IP-address
 */
void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
  loadState = true;
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

/*
 * Set WiFi not ready to use
 */
void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
  loadState = false;
  Serial.println("Disconnected from WiFi access point");
  Serial.print("WiFi lost connection. Reason: ");
  Serial.println(info.disconnected.reason);
  Serial.println("Trying to Reconnect");
  WiFi.begin(ssid, password);
}

/*
 * Prepare WiFi before usage
 */
void initWiFi()
{
  // delete old config
  WiFi.disconnect(true);

  delay(1000);

  WiFi.onEvent(WiFiStationConnected, SYSTEM_EVENT_STA_CONNECTED);
  WiFi.onEvent(WiFiGotIP, SYSTEM_EVENT_STA_GOT_IP);
  WiFi.onEvent(WiFiStationDisconnected, SYSTEM_EVENT_STA_DISCONNECTED);
  
  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");

  unsigned long wifiConnectStart = millis();

  while (WiFi.status() != WL_CONNECTED) {
    // Check to see if
    int timespan = millis() - wifiConnectStart;
    if(loadState)
    {
      if(WiFi.status() == WL_CONNECTED)
      {
        break;
      }
    }
    else if (WiFi.status() == WL_CONNECT_FAILED)
    {
      Serial.println("Failed to connect to WiFi.");
      break;
    }
    else if (timespan > 60000)
    {
      Serial.println("Failed to connect to WiFi");
      break;
    }
    else if(WiFi.status() != WL_CONNECTED 
            && timespan % 15000 == 0)
    {
      Serial.print(".");
    }
  }

  if(!loadState)
  {
    sleep(true);
  }
  else
  {
    Serial.println("Continue loading :)");
  }
  Serial.println();
}

/*
 * Initialize header on display
 */
void initHeader()
{
  display.fillScreen(GxEPD_WHITE);
  showFont("Vejrstation", &FreeMonoBold24pt7b, 50, 50);
  showFont("Aksel H. Gregersen", &FreeMonoBold9pt7b, 110, 110);
  display.drawBitmap(gImage_WEATHER, 20, 80, 50, 50, GxEPD_WHITE); //left side
  display.drawBitmap(gImage_WEATHER, 330, 80, 50, 50, GxEPD_WHITE); //right side
}
/*
 * Initialize main data on display
 */
void initScreen()
{
  showFont("Tidspunkt:",     &FreeMonoBold12pt7b, 15, 170);
  showFont("Nedboer:",       &FreeMonoBold12pt7b, 15, 200);
  showFont("Vindretning:",   &FreeMonoBold12pt7b, 15, 230);
  showFont("Vindhastighed:", &FreeMonoBold12pt7b, 15, 260);
  showFont("Tryk:",          &FreeMonoBold12pt7b, 15, 290);
}

/*
 * Show incomming data on screen
 */
void showFont(const char text[], const GFXfont* font, int x, int y)
{
  display.setCursor(x, y);
  display.setTextColor(GxEPD_BLACK, GxEPD_WHITE);
  display.setFont(font);
  display.println(text);
}

/*
 * Read data on API
 */
void getData()
{
    const int httpPort = 80;
    if (!client.connect(host, httpPort))
    {
        Serial.println("connection failed");
        sleep(true);
    }

    // We now create a URI for the request
    String url = "/api/getWeather.php";

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
            sleep(true);
        }
    }

    // Read all the lines of the reply from server and print them to Serial
    const char* localResult = "";
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

        strcpy(buf, localResult);
        strcat(buf, lineBuffer);
        localResult = buf; 
      }
      result = localResult;
    }
}

/*
 * Parse JSON object from API, and manage it
 */
void parseData()
{ 
  char localResult[200];
  bool state = false;
  strcpy(localResult, result);
  
  const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60;
  DynamicJsonDocument doc(capacity);

  // Parse JSON object
  DeserializationError error = deserializeJson(doc, localResult);
  if (error)
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    state = true;
  }
  else
  {
    // Extract values
    const char* id = doc["id"].as<const char*>();

    char localTime[25];
    strcpy(localTime, id);
    
    if(strcmp(localTime, lastId) != 0)
    {
      const char* datetime = doc["datetime"].as<const char*>();
      const char* rainAmount = doc["rainAmount"].as<const char*>();
      const char* windDir = doc["windDir"].as<const char*>();
      const char* windSpeed = doc["windSpeed"].as<const char*>();
      const char* pressure = doc["pressure"].as<const char*>();
      printData(datetime, rainAmount, windDir, windSpeed, pressure);
      Serial.println("Updated screen");
      strcpy(lastId, localTime);
    }
    else
    {
      Serial.println("Screen shows the current data already");
    }
  }
  
  client.stop();
  sleep(state);
}

/*
 * Print incomming data
 */
void printData(const char* datetime, const char* rainAmount, const char* windDir, 
               const char* windSpeed, const char* pressure)
{
  initHeader();
  initScreen();
  const char* unitMillimeter = "mm";
  const char* unitMeterPerSecond = "m/s";
  const char* unitPascal = "Pa";
  
  const char* clockTime = lastLetters(datetime, 8);
  String rainString = getDataWithUnit(rainAmount, unitMillimeter);
  String windString = getDataWithUnit(windSpeed, unitMeterPerSecond);
  String pressureString = getDataWithUnit(pressure, unitPascal);
  
  showFont(clockTime, &FreeMonoBold12pt7b, 250, 170);
  showFont(string2char(rainString), &FreeMonoBold12pt7b, 250, 200);
  showFont(getDirection(windDir), &FreeMonoBold12pt7b, 250, 230);
  showFont(string2char(windString), &FreeMonoBold12pt7b, 250, 260);
  showFont(string2char(pressureString), &FreeMonoBold12pt7b, 250, 290);
  
  display.update();
}

/*
 * Convert String to char*
 */
char* string2char(String command)
{
  if(command.length()!=0)
  {
      char* p = const_cast<char*>(command.c_str());
      return p;
  }
}

/*
 * Return the x last character in the incomming char*
 */
const char* lastLetters(const char* str, int amount)
{
  int len = strlen(str);
  return len > 0 ? str + len - amount : str;
}

/*
 * Convert incomming direction to its word
 */
const char* getDirection(const char* str)
{
  if(strcmp(str, "N") == 0)
  {
    return "Nord";
  }
  else if(strcmp(str, "NE") == 0)
  {
    return "Nordoest";
  }
  else if(strcmp(str, "E") == 0)
  {
    return "Oest";
  }
  else if(strcmp(str, "SE") == 0)
  {
    return "Sydoest";
  }
  else if(strcmp(str, "S") == 0)
  {
    return "Syd";
  }
  else if(strcmp(str, "SW") == 0)
  {
    return "Sydvest";
  }
  else if(strcmp(str, "W") == 0)
  {
    return "Vest";
  }
  else if(strcmp(str, "NW") == 0)
  {
    return "Nordvest";
  }
  else
  {
    return "?"; 
  }
}

/*
 * Return the inputText followed by the inputUnit
 */
String getDataWithUnit(const char* inputText, const char* inputUnit)
{
  String value = "";

  value.concat(inputText);
  value.concat(" ");
  value.concat(inputUnit);
  
  return value;
}

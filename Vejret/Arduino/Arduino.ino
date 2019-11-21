#include <UnoWiFiDevEd.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer = { 0x28, 0xFF, 0xD7, 0x1E, 0x03, 0x15, 0x02, 0x21 };
  
//Wind vane
const float table[16] = {3.84, 1.98, 2.25, 0.41, 0.45, 0.32, 0.90, 0.62, 1.40, 1.19, 3.08, 2.93, 4.62, 4.32, 4.78, 3.43}; //charecter 13 is not correct, but is changed due to failure in windvane

char buffer[20];

//Anometer - windpower
volatile unsigned int windRotation = 0;
//Used for timing
float windTimer = 0;

//Rain gauge
float RainMeasurement = 0;
volatile byte hit = 1;

char myData[70];

void setup() {
  Serial.begin(9600);
  Ciao.begin(); // CIAO INIT
  sensors.begin();
  sensors.setResolution(insideThermometer, 10);
  
  initWind();
  initRain();
  delay(500);
}

void loop() {
  doRequest();
  delay(1800000);
} 

//Gets data about rain
int getRainData(void)
{
  return (int)RainMeasurement;
}

void doRequest(){
  
  sprintf(myData, "/addTKJ.php?key=myKey&dir=%d&speed=%d&amount=%d&temp=%d", printWindDirection(), getWindSpeed(), getRainData(), getTemp(insideThermometer));
  Ciao.println("Link: " + String(myData));
  CiaoData data = Ciao.write("rest", "vejret.akselhg.dk", myData, "GET");
  if (!data.isEmpty()){
    Ciao.println("State: " + String (data.get(1)));
    Ciao.println("Response: " + String (data.get(2)));  
    RainMeasurement = 0;
  }
  else{
    Ciao.println (F("Write Error"));
  }

  Ciao.println();
}

// Initializing processes 
void initWind(void)
{
  pinMode(3, INPUT);
  attachInterrupt(1, windSpeed, RISING);
  windTimer=millis();//start timing  
}

//initializing rainmeasure
void initRain(void)
{
  attachInterrupt(0, Rain_Measure, RISING); //analog port 0
}    

//Counts amount of rain
void Rain_Measure(void)
{
  if (hit == 1) {
    hit = 2;
  } else if (hit == 2) {
    hit = 3;
  } else if (hit == 3) {
    RainMeasurement = RainMeasurement + 0.2794;
    hit = 1;
  }
}

//Prints winddirection
int printWindDirection()
{
   int angle = 0;
   float voltage = analogRead(0)/204.6;   
   for (int i = 0; i < 16; i++) {
     if (voltage <= table[i]+0.03 && voltage >= table[i]-0.03) {
       angle = i;
       break;
     }
   }
  return angle;
}  

//Prints windspeed
int getWindSpeed()
{
  float windDtime =  millis()-windTimer;
  windTimer = millis();
  windDtime = windDtime/1000;
  float windSpeed = windRotation/windDtime;//rotation per second
  windRotation = 0;  
  windSpeed = windSpeed*2/3;//1 rotation per second equals 2.4 km/h = 2/3 m/s
  return (int)windSpeed;
}

int getTemp(DeviceAddress deviceAddress)
{
  sensors.requestTemperatures();
  float tempC = sensors.getTempC(deviceAddress);

  if (tempC == -127.00) {
    Serial.print("Error getting temperature");
  }
  return (int)tempC;
}

void windSpeed()
{
  windRotation++;
}
